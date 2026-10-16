

static esp_err_t zb_action_handler(esp_zb_core_action_callback_id_t callback_id, const void *message) {
    
    Serial.printf("[ACTION] callback_id = %d\n", callback_id);
    char logBuffer[128];
    uint16_t shortAddr = 0xFFFF;
    uint16_t cluster_id = 0x0000;
    uint16_t attribute_id = 0xFFFF;
    void *data_ptr = NULL;
    int idx;
    // 1. Haal de basisgegevens en de data-pointer op per callback-type
    switch (callback_id) {
        case ESP_ZB_CORE_REPORT_ATTR_CB_ID: { // ID: 8192
            esp_zb_zcl_report_attr_message_t *m = (esp_zb_zcl_report_attr_message_t *)message;
            shortAddr = m->src_address.u.short_addr;
            cluster_id = m->cluster;
            attribute_id = m->attribute.id;
            data_ptr = m->attribute.data.value;
            idx = deviceLookup(shortAddr, 0); // we want to know which device this is
            break;
        }
        case ESP_ZB_CORE_CMD_READ_ATTR_RESP_CB_ID: { // Read response
            esp_zb_zcl_cmd_read_attr_resp_message_t *m = (esp_zb_zcl_cmd_read_attr_resp_message_t *)message;
            // Let op: een read response kan een lijst van attributen bevatten. We pakken de eerste voor de log.
            if (m->info.status == ESP_ZB_ZCL_STATUS_SUCCESS && m->variables) {
                shortAddr = m->info.src_address.u.short_addr;
                cluster_id = m->info.cluster;
                // De variabelenlijst binnen de SDK verschilt per versie, 
                // we skippen hier de diepe data_ptr om crashes te voorkomen.
            }
            break;
        }

        default:
            
            return ESP_ERR_NOT_SUPPORTED;
    }

    // String om de betekenis in op te slaan
    String meaning = "unknown data";

    // 2. whem we have a valid data-pointer (especcially on reports), translate the value
    if (data_ptr != NULL) {
        consoleOut("we have a data_ptr");
        if (cluster_id == ESP_ZB_ZCL_CLUSTER_ID_ON_OFF && attribute_id == ESP_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID) {
            uint8_t waarde = *(uint8_t *)data_ptr;
            meaning = (waarde == 1) ? "Status: ON" : "Status: OFF";
        } 
        else if (cluster_id == ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT && attribute_id == ESP_ZB_ZCL_ATTR_TEMP_MEASUREMENT_VALUE_ID) {
            int16_t raw_temp = *(int16_t *)data_ptr;
            float temp = raw_temp / 100.0;
            meaning = "Temperature: " + String(temp, 2) + " °C";
            Dev_Data[idx].values[0] = temp;
        } 
        else if (cluster_id == ESP_ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT && attribute_id == ESP_ZB_ZCL_ATTR_REL_HUMIDITY_MEASUREMENT_VALUE_ID) {
            uint16_t raw_hum = *(uint16_t *)data_ptr;
            float hum = raw_hum / 100.0;
            meaning = "humidity: " + String(hum, 2) + " %";
            Dev_Data[idx].values[1] = hum;
            Dev_Prop[idx].toMqtt = true;
            eventSend(0);
        }
        else if (cluster_id == ESP_ZB_ZCL_CLUSTER_ID_OCCUPANCY_SENSING && attribute_id == ESP_ZB_ZCL_ATTR_OCCUPANCY_SENSING_OCCUPANCY_ID) {
            uint8_t occupancy = *(uint8_t *)data_ptr;
            meaning = (occupancy == 1) ? "motion detected" : "no motion";
        }

    }

    // 3. the final logmessage
    //Serial.printf("[ZIGBEE] Message | shortAddr: 0x%04X | callback: %d | cluster: 0x%04X | %s\n", 
    //              shortAddr, callback_id, cluster_id, meaning.c_str());
    snprintf(logBuffer, sizeof(logBuffer), "[ZIGBEE] Message | shortAddr: 0x%04X | callback: %d | cluster: 0x%04X | %s\n", 
                  shortAddr, callback_id, cluster_id, meaning.c_str());
    consoleOut(logBuffer);
    return ESP_OK;
}



static bool my_raw_rx_handler(uint8_t bufid) {
    char logBuffer[64];
    // 1. Haal de header op om het cluster te controleren
    zb_zcl_parsed_hdr_t *cmd_info = ZB_BUF_GET_PARAM(bufid, zb_zcl_parsed_hdr_t);
    
    if (cmd_info == NULL) {
        return false; // Direct doorsturen naar de rest
    }
   
    if(ledSignal) my_wifiConnect.ledBlink(200,0,0,2,100);// VOOR DEBUGGING: Print welk cluster er daadwerkelijk binnenkomt
    //Serial.printf("[RAW INTERCEPTED] Cluster: 0x%04X | Command ID: 0x%02X\n", cmd_info->cluster_id, cmd_info->cmd_id);
    //snprintf(logBuffer, sizeof(logBuffer), "[RAW MESSAGE] Cluster: 0x%04X | Command ID: 0x%02X\n", cmd_info->cluster_id, cmd_info->cmd_id);
    snprintf(logBuffer, sizeof(logBuffer), 
         "[RAW MESSAGE] From: 0x%04X | Cluster: 0x%04X | Command ID: 0x%02X\n", 
         cmd_info->addr_data.common_data.source.u.short_addr,
         cmd_info->cluster_id, 
         cmd_info->cmd_id);
    
    consoleOut(logBuffer);
    // FILTER: We only want cluster 0x0500
    if (cmd_info->cluster_id == 0x0500) {
        
        // get shortAddr directly from cmd_info header
        uint16_t src_short_addr = cmd_info->addr_data.common_data.source.u.short_addr;
        //Serial.printf("  -> coming from: 0x%04X\n", src_short_addr);
        snprintf(logBuffer, sizeof(logBuffer), "  -> coming from: 0x%04X\n", src_short_addr);
        consoleOut(logBuffer);
        // 2. Check is shortAddr is one of our devices
        int idx = deviceLookup(src_short_addr, 0); 
        if (idx != -1) {
            consoleOut("shortAdr is known:" + String(src_short_addr, HEX));
            
            // only react when Zone Status Change (Command 0x00)
            if (cmd_info->cmd_id == 0x00) {
                uint8_t *payload = (uint8_t *)zb_buf_begin(bufid);
                uint16_t len = zb_buf_len(bufid);

                if (len >= 2) {
                    uint16_t zone_status = payload[0] | (payload[1] << 8);
                    bool det_motion = (zone_status & 0x0001); // Bit 0 is het bewegingsalarm
                    double nieuweStatus = det_motion ? 1.0 : 0.0;
                    // is the stat really changed relative to the saved values?
                    if (Dev_Data[idx].values[0] != nieuweStatus) {
                        // Update jouw specifieke data struct
                        Dev_Data[idx].values[0] = det_motion ? 1.0 : 0.0;
                        Dev_Prop[idx].toMqtt = true;

                        //Serial.printf("[INTERCEPTOR] motion detected on device %d: %s\n", 
                        //            idx, det_motion ? "🔴 Y" : "🟢 N");

                        snprintf(logBuffer, sizeof(logBuffer), "[RAW MESSAGE] motion detected on device %d: %s\n", 
                                    idx, det_motion ? "Y" : "N");
                        consoleOut(logBuffer);
                    } else {
                        //Serial.printf("[INTERCEPTOR] Duplicate message ignored genegeerd for device %d\n", idx);
                        snprintf(logBuffer, sizeof(logBuffer),"[RAW MESSAGE] Duplicate message ignored for device %d\n", idx);
                        consoleOut(logBuffer);
                    }
                    
                    zb_buf_free(bufid);
                    return true; // Bericht volledig afgehandeld, blokkeer voor actionhandler
                }
            }
        } else {
            Serial.println("  -> This shortAddr is unknown!");
        }
    }

    // VOOR AL HET ANDERE (andere clusters, onbekende adressen):
    // Geef false terug. De ESP32 stuurt het pakket direct door naar je normale actionhandler.
    return false; 
}

//  bool my_aps_data_interceptor(esp_zb_apsde_data_ind_t apsde_data) {

//     char logBuffer[120];
//     // Filter op het Device Announcement Cluster (0x0013 / 0x13)
//     if (apsde_data.cluster_id == 0x0013  && networkOpen == true) {
//         pairOut("received a device announce");
//         uint8_t *payload = apsde_data.asdu;
        
//         uint16_t shortAddr = payload[1] | (payload[2] << 8);
//         esp_zb_ieee_addr_t ieeeAddr;
//         for (int i = 0; i < 8; i++) {
//             ieeeAddr[i] = payload[3 + i];
//         }
//         onDeviceJoin(shortAddr, ieeeAddr);

//         // We gebruiken nu gewone Serial prints zodat het ALTIJD zichtbaar is!
//        // Serial.println("=============================================");
//         pairOut("   💥 INTERCEPTION: DEVICE JOINED! 💥   ");
//        // Serial.println("=============================================");
        
       
//         return false; // Return false zodat de Arduino-stack de pairing netjes afrondt!
//     }
//     // --- DEEL 2: IAS ZONE LOGICA (Cluster 0x0500) ---
//         if (apsde_data.cluster_id == 0x0500) {
        
//         // Haal het shortAddr RECHTSTREEKS uit de APS-layer metadata
//         uint16_t src_short_addr = apsde_data.src_short_addr;
        
//         snprintf(logBuffer, sizeof(logBuffer), "  -> coming from: 0x%04X\n", src_short_addr);
//         consoleOut(logBuffer);

//         // Check of het shortAddr bekend is in je structs
//         int idx = deviceLookup(src_short_addr, 0); 
//         if (idx != -1) {
//             consoleOut("shortAdr is known:" + String(src_short_addr, HEX));
            
//             // Controleer of de payload lang genoeg is voor de ZCL-header + data
//             // (FrameControl + TSN + CmdID + min. 2 bytes zone_status = 5 bytes)
//             if (apsde_data.asdu_length >= 5) {
//                 uint8_t *payload = apsde_data.asdu;
                
//                 // Haal het Command ID op uit de ZCL-header (byte 2)
//                 uint8_t cmd_id = payload[2];

//                 // Alleen actie ondernemen bij een Zone Status Change Notification (Command 0x00)
//                 if (cmd_id == 0x00) {
//                     // De eigenlijke status data (16-bit) begint vanaf byte 3 (Little Endian)
//                     uint16_t zone_status = payload[3] | (payload[4] << 8);
//                     bool det_motion = (zone_status & 0x0001); // Bit 0 is het bewegingsalarm
//                     double nieuweStatus = det_motion ? 1.0 : 0.0;

//                     // Controleer of de status daadwerkelijk is veranderd
//                     if (Dev_Data[idx].values[0] != nieuweStatus) {
//                         // Update jouw specifieke data structs
//                         Dev_Data[idx].values[0] = nieuweStatus;
//                         Dev_Prop[idx].toMqtt = true;

//                         snprintf(logBuffer, sizeof(logBuffer), "[APS MESSAGE] motion detected on device %d: %s\n", 
//                                     idx, det_motion ? "Y" : "N");
//                         consoleOut(logBuffer);
//                     } else {
//                         snprintf(logBuffer, sizeof(logBuffer), "[APS MESSAGE] Duplicate message ignored for device %d\n", idx);
//                         consoleOut(logBuffer);
//                     }
                    
//                     // HEEL BELANGRIJK: We retourneren true om aan te geven dat het bericht verwerkt is. 
//                     // De buffer HOEFT en MAG hier niet handmatig worden vrijgegeven!
//                     return true; 
//                 }
//             }
//         } else {
//             Serial.println("  -> This shortAddr is unknown!");
//         }
//     }
//     return false; // Laat alle overige data (zoals je cluster 0x1 meldingen) rustig doorstromen
// }

bool my_aps_data_interceptor(esp_zb_apsde_data_ind_t apsde_data) {
    char logBuffer[120];
    static const char *TAG = "ZB_PAYLOAD_ANALYSIS";
     // 1. Validate payload existence and bounds
    if (apsde_data.asdu_length == 0 || apsde_data.asdu == NULL) {

        snprintf(logBuffer, sizeof(logBuffer), "[interceptor] Empty payload received from Cluster 0x%04X", apsde_data.cluster_id);
        consoleOut(logBuffer);
        return false; // Let the stack handle it if empty
    }
 
    // 2. Isolate and copy the payload safely
    uint8_t local_payload[256]; // Ensure size fits your expected max MTU
    size_t payload_len = (apsde_data.asdu_length < sizeof(local_payload)) ? apsde_data.asdu_length : (sizeof(local_payload) - 1);
    
    memcpy(local_payload, apsde_data.asdu, payload_len);

    // 3. Analyze the data
    if(zbDebug==true) analyze_zigbee_payload(apsde_data, local_payload, payload_len);
    
    // Filter op het Device Announcement Cluster (0x0013 / 0x13)
    if (apsde_data.cluster_id == 0x0013 && networkOpen == true) {
        pairOut("received a device announce");
        //uint8_t *local_payload = apsde_data.asdu;
        
        uint16_t shortAddr = local_payload[1] | (local_payload[2] << 8);
        esp_zb_ieee_addr_t ieeeAddr;
        for (int i = 0; i < 8; i++) {
            ieeeAddr[i] = local_payload[3 + i];
        }
        onDeviceJoin(shortAddr, ieeeAddr);

        // We gebruiken nu gewone Serial prints zodat het ALTIJD zichtbaar is!
       // Serial.println("=============================================");
        pairOut("   INTERCEPTION: DEVICE JOINED!  ");
       // Serial.println("=============================================");
        
       
        return false; // Return false zodat de Arduino-stack de pairing netjes afrondt!
    }
    // --- DEEL 2: IAS ZONE LOGICA (Cluster 0x0500) ---
        if (apsde_data.cluster_id == 0x0500) {
        
        // Haal het shortAddr RECHTSTREEKS uit de APS-layer metadata
        uint16_t src_short_addr = apsde_data.src_short_addr;
        
        snprintf(logBuffer, sizeof(logBuffer), "  -> coming from: 0x%04X\n", src_short_addr);
        consoleOut(logBuffer);

        // Check of het shortAddr bekend is in je structs
        int idx = deviceLookup(src_short_addr, 0); 
        if (idx != -1) {
            consoleOut("shortAdr is known:" + String(src_short_addr, HEX));
            
            // Controleer of de payload lang genoeg is voor de ZCL-header + data
            // (FrameControl + TSN + CmdID + min. 2 bytes zone_status = 5 bytes)
            //if (apsde_data.asdu_length >= 5) {
            //    uint8_t *payload = apsde_data.asdu;
                
                // Haal het Command ID op uit de ZCL-header (byte 2)
                uint8_t cmd_id = local_payload[2];

                // Alleen actie ondernemen bij een Zone Status Change Notification (Command 0x00)
                if (cmd_id == 0x00) {
                    // De eigenlijke status data (16-bit) begint vanaf byte 3 (Little Endian)
                    uint16_t zone_status = local_payload[3] | (local_payload[4] << 8);
                    bool det_motion = (zone_status & 0x0001); // Bit 0 is het bewegingsalarm
                    double nieuweStatus = det_motion ? 1.0 : 0.0;

                    // Controleer of de status daadwerkelijk is veranderd
                    if (Dev_Data[idx].values[0] != nieuweStatus) {
                        // Update jouw specifieke data structs
                        Dev_Data[idx].values[0] = nieuweStatus;
                        Dev_Prop[idx].toMqtt = true;

                        snprintf(logBuffer, sizeof(logBuffer), "[APS MESSAGE] motion detected on device %d: %s\n", 
                                    idx, det_motion ? "Y" : "N");
                        consoleOut(logBuffer);
                        eventSend(0);
                    } else {
                        snprintf(logBuffer, sizeof(logBuffer), "[APS MESSAGE] Duplicate message ignored for device %d\n", idx);
                        consoleOut(logBuffer);
                    }
                    
                            // Optioneel: Batterij status checken (Bit 3)
                    if (zone_status & 0x0008) {
                        snprintf(logBuffer, sizeof(logBuffer), "WARNING: Battery of sensor 0x%04X is almost empty!\n", apsde_data.src_short_addr);
                        consoleOut(logBuffer);
                    }
                    // Very important: We return true to indicate that the message is processed. 
                    // The buffer cannot be freed manual!
                    return true; 
                }
            //}
        } else {
            Serial.println("  -> This shortAddr is unknown!");
        }
    }
    return false; // Laat alle overige data (zoals je cluster 0x1 meldingen) rustig doorstromen
}


void analyze_zigbee_payload(esp_zb_apsde_data_ind_t ind, uint8_t *payload, size_t len) {
    static const char *TAG = "ZB_PAYLOAD_ANALYSIS";

    Serial.println("ZB_PAYLOAD_ANALYSIS --- New APS Frame Received ---");
    Serial.printf("Src Addr: 0x%04X | Src EP: %d | Dst EP: %d\n", ind.src_short_addr, ind.src_endpoint, ind.dst_endpoint);
    Serial.printf("Cluster ID: 0x%04X | Profile ID: 0x%04X\n", ind.cluster_id, ind.profile_id);
    Serial.printf("Payload Length: %d bytes\n", len);

    // 2. De Hex Dump utility omgebouwd met Serial.print
    Serial.print("Payload Hex: ");
    for (int i = 0; i < len; i++) {
        Serial.printf("%02X ", payload[i]);
    }
    Serial.println(); // Sluit af met een nieuwe regel
}