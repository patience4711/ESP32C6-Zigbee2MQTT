/* when a device has joined, we discover it and save the results in the structs 
and save that in prefs
*/

void discoverDevice(uint16_t shortAddr)
{
  char logBuffer[100];

  snprintf(logBuffer, sizeof(logBuffer), "[DISCOVER] Asking device 0x%04X for active endpoints...\n", shortAddr);
  pairOut(String(logBuffer));

  esp_zb_zdo_active_ep_req_param_t *req =
      (esp_zb_zdo_active_ep_req_param_t *)calloc(
          1, sizeof(esp_zb_zdo_active_ep_req_param_t));

  if (req == nullptr) {
    pairOut("[DISCOVER] ERROR: memory allocation failed");
    return;
  }

  req->addr_of_interest = shortAddr;

   if (esp_zb_lock_acquire(portMAX_DELAY)) {
  
           esp_zb_zdo_active_ep_req(req, activeEndpointCallback, req);

      // 3. Geef de lock direct daarna weer vrij
    esp_zb_lock_release();
  } else {
    pairOut("[DISCOVER] ERROR: Could not acquire Zigbee lock");
    free(req); // Voorkom memory leak als het misgaat
  }
}


void simpleDescriptorCallback(
    esp_zb_zdp_status_t status,
    esp_zb_af_simple_desc_1_1_t *simple_desc,
    void *user_ctx)
{
  
  char logBuffer[64];
  
  esp_zb_zdo_simple_desc_req_param_t *req =
      (esp_zb_zdo_simple_desc_req_param_t *)user_ctx;

  if (status != ESP_ZB_ZDP_STATUS_SUCCESS) {
    snprintf(logBuffer, sizeof(logBuffer), "[DISCOVER] Simple descriptor request failed: 0x%02X\n",
        status);
    pairOut(String(logBuffer));
    free(req);
    return;
  }
  if (simple_desc->endpoint == 242) {
    pairOut("[DISCOVER] Green Power endpoint (242) detected. We skip this!");
    free(req); // 
    return;    // Stop to prevent that data is overwritten 
  }
  snprintf(logBuffer, sizeof(logBuffer), "[DISCOVER] Profile ID : 0x%04X\n", simple_desc->app_profile_id);
  pairOut(String(logBuffer));

  snprintf(logBuffer, sizeof(logBuffer), "[DISCOVER] Input clusters : %u", simple_desc->app_input_cluster_count);
  pairOut(String(logBuffer));
  
  for (uint8_t i = 0;
       i < simple_desc->app_input_cluster_count;
       i++) {

       snprintf(logBuffer, sizeof(logBuffer), "[DISCOVER]   IN  cluster : 0x%04X", simple_desc->app_cluster_list[i]);
       pairOut(String(logBuffer));
  }

  snprintf(logBuffer, sizeof(logBuffer), "[DISCOVER] Output clusters : %u", simple_desc->app_output_cluster_count);
  pairOut(String(logBuffer));

  for (uint8_t i = 0;
       i < simple_desc->app_output_cluster_count;
       i++) {

     snprintf(logBuffer, sizeof(logBuffer), "[DISCOVER]   OUT cluster : 0x%04X", simple_desc->app_cluster_list[simple_desc->app_input_cluster_count + i]);
      pairOut(String(logBuffer));
   }

  // ==========================================
  // find out what nature of device it is from the device-ID
  // ==========================================

  Dev_Prop[toJoin].endpoint = simple_desc->endpoint; //we save this one in Dev_Prop

  uint16_t zigbeeDeviceId = simple_desc->app_device_id;
  snprintf(logBuffer, sizeof(logBuffer), "[DISCOVER] Device ID: 0x%04X", zigbeeDeviceId);
  pairOut(String(logBuffer));
  
  if (zigbeeDeviceId == 0x0009 || zigbeeDeviceId == 0x0107 || zigbeeDeviceId == 0x0051) {
          // 0x0009 = Mains Power Outlet, 0x0107 = On/Off Pluggable Appliance
          Dev_Prop[toJoin].devType = 4; 
          pairOut("[DISCOVER] found type: Smart Socket / Relais");
      } 
      else if (zigbeeDeviceId == 0x0402 ) {
          // 0x0402 = IAS Zone Sensor (beweging/rook), 0x0302 = Occupancy Sensor
          Dev_Prop[toJoin].devType = 2;
          pairOut("[DISCOVER] found type: motion sensor");
      } 
      else if (zigbeeDeviceId == 0x0100 || zigbeeDeviceId == 0x0101 || zigbeeDeviceId == 0x0102 || zigbeeDeviceId == 0x010D || zigbeeDeviceId == 0x0061 ) {
      // 0x0102 = Color Dimmable Light (RGB), 0x0101 = Dimmable, 0x0100 = On/Off Light
        Dev_Prop[toJoin].devType = 3;
        pairOut("[DISCOVER] found type: RGB / smart Lamp");
      }
      else if (zigbeeDeviceId == 0x0302) {
          // 0x0302 = Officieel Device ID voor Temperatuur Sensor
          Dev_Prop[toJoin].devType = 1;
          pairOut("[DISCOVER] found type: Temp & Hum sensor");
      }

       
      else {
          // Fallback: Als het apparaat een custom/onbekend ID stuurt, scannen we de clusters
          bool heeftBeweging = false;
          for (uint8_t i = 0; i < Dev_Data[toJoin].inputClusterCount; i++) {
              if (Dev_Data[toJoin].inputClusters[i] == 0x0500 || Dev_Data[toJoin].inputClusters[i] == 0x0406) {
                  heeftBeweging = true;
                  break;
              }
          }
          
          if (heeftBeweging) {
              Dev_Prop[toJoin].devType = 2;
              pairOut("[DISCOVER] found type (via Cluster scan): motion sensor");
          } else {
              Dev_Prop[toJoin].devType = 0;
              snprintf(logBuffer, sizeof(logBuffer), "[DISCOVER] found unknown (Device ID: 0x%04X)\n", zigbeeDeviceId);
              pairOut(String(logBuffer));
              //Serial.printf("[DISCOVERY] found type: Unknown (Device ID: 0x%04X)\n", zigbeeDeviceId);
          }
      }
  
  // ==========================================
  // set the vars in Dev_Data[toJoin] and save
  // ==========================================
  
  Dev_Data[toJoin].profileId =
      simple_desc->app_profile_id;

  Dev_Data[toJoin].deviceId =
      simple_desc->app_device_id;

  // Input clusters
  Dev_Data[toJoin].inputClusterCount =
      simple_desc->app_input_cluster_count;

  if (Dev_Data[toJoin].inputClusterCount > 20) {
    Dev_Data[toJoin].inputClusterCount = 20;
  }

  for (uint8_t i = 0;
       i < Dev_Data[toJoin].inputClusterCount;
       i++) {

    Dev_Data[toJoin].inputClusters[i] =
        simple_desc->app_cluster_list[i];
  }

  // Output clusters
  Dev_Data[toJoin].outputClusterCount =
      simple_desc->app_output_cluster_count;

  if (Dev_Data[toJoin].outputClusterCount > 20) {
    Dev_Data[toJoin].outputClusterCount = 20;
  }

  for (uint8_t i = 0;
       i < Dev_Data[toJoin].outputClusterCount;
       i++) {

    Dev_Data[toJoin].outputClusters[i] =
        simple_desc->app_cluster_list[
            simple_desc->app_input_cluster_count + i];
  }

  savePrefs(toJoin); // saves both Dev_Prop and Dev_Data
  pairOut("[DISCOVER] saved data for device " + String(toJoin));

   free(req);
 
}

void activeEndpointCallback(
       esp_zb_zdp_status_t status,
       uint8_t ep_count,
       uint8_t *ep_list,
       void *user_ctx
       )
{
  esp_zb_zdo_active_ep_req_param_t *req =
      (esp_zb_zdo_active_ep_req_param_t *)user_ctx;

  if (status != ESP_ZB_ZDP_STATUS_SUCCESS) {
    //Serial.printf(
    //    "[DISCOVERY] Active endpoint request failed: 0x%02X\n",
    //    status);
    pairOut("Active endpoint request failed");    
    free(req);
    return;
  }

  pairOut("[DISCOVER] this dev has endpoints # " + String(ep_count));
  for (uint8_t i = 0; i < ep_count; i++) {

    uint8_t endpoint = ep_list[i];

    pairOut("[DISCOVER] found endpoint nr " + String(endpoint));
    // Simple Descriptor opvragen
    esp_zb_zdo_simple_desc_req_param_t *desc_req =
        (esp_zb_zdo_simple_desc_req_param_t *)calloc(
            1,
            sizeof(esp_zb_zdo_simple_desc_req_param_t));

    if (desc_req == nullptr) {
      pairOut("[DISCOVER] ERROR: memory allocation failed");
      continue;
    }

    desc_req->addr_of_interest = req->addr_of_interest;
    desc_req->endpoint = endpoint;

    esp_zb_zdo_simple_desc_req(
        desc_req,
        simpleDescriptorCallback,
        desc_req);
  }

  free(req);
}