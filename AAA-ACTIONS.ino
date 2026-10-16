// these are operations that are triggered within an async webserver request. 
// a crash or wdt reset can happen when these actions take too long 
// or there are delays in the routines. asyncwebserver forbids that.

void test_actionFlag() {
    if(actionFlag == 0) return;
    
    consoleOut("testing actionFlag = " + String(actionFlag));
    
    //if(actionFlag != 0) Serial.println("test_actionFlag 1 val = " + String(actionFlag));  
    // ******************  reset the nework data and reboot in AP *************************
    if (actionFlag == 11 || value == 11) 
    { // 
            //DebugPrintln("erasing the wifi credentials, value = " + String(value) + "  actionFlag = " + String(actionFlag));
            delay(1000); //reserve time to release the button
            //eraseWifiFlash();
            WiFi.disconnect();
            consoleOut(F("wifi disconnected"));
            //we try to overwrite the wifi creentials     
            const char* ssid = "dummy";
            const char* password = "dummy";
            WiFi.begin(ssid, password);
            Serial.println(F("\nConnecting to dummy network"));
            int teller = 0;
            while(WiFi.status() != WL_CONNECTED)
            {
                Serial.print(F("wipe wifi credentials\n"));
                delay(100);         
                teller ++;
                if (teller > 2) break;
            }
            ESP.restart();
    }  

    if (actionFlag == 10) 
    { // the button was pressed a long time, start ap
            delay(2000); // give the server the time to send the confirm
            consoleOut("rebooting");
            // takes care for the opening of the frontpage
            strncpy(requestUrl, "/", sizeof(requestUrl) - 1);
            requestUrl[sizeof(requestUrl) - 1] = '\0'; // Zo
            String key = "req";
            prefs.putString(key.c_str(), requestUrl);
            ESP.restart();
    }
    
if (actionFlag >= 20 && actionFlag < 30) 
        { //triggered by device del
          
          int devNr = actionFlag - 20; 
          Serial.println("device to delete = " + String(devNr));
          deviceDelete(devNr);
          diagNose = 1;  // rest goes to the console
          //send the commands to interview device
          actionFlag = 0; //reset the actionflag
        }

if (actionFlag >= 30 && actionFlag < 40) 
        { //triggered by devices
          
           int devNr = actionFlag - 30; 
           //String bestand = "/Dev_Prop" + String(devNr) + ".str"; // /Inv_Prop0.str
           consoleOut("going to write struct " + String(devNr)  ); 
           savePrefs(devNr); // save in preferences
           diagNose = 1;  // rest goes to the console
           actionFlag = 0; //reset the actionflag
        }



// if (actionFlag == 35) { //triggered by the api (external)
//         actionFlag = 0; //reset the actionflag
//         if(diagNose == 10) diagNose = 3;
//         consoleOut("TEST actionFlag = 45 ");
//         cont_read(); // 
//         if(diagNose == 3) diagNose = 1;
//     }
    if (actionFlag == 44) { //healthcheck triggered by the webpage zbtest
        actionFlag = 0; //reset the actionflag
        healthCheck(); 
    }
    if (actionFlag == 45) { //triggered by the api (external)
        actionFlag = 0; //reset the actionflag
        if(diagNose == 10) diagNose = 3;
        consoleOut("TEST actionFlag = 45 ");
        //testMessage(); // the bool decides where to find the input
        if(diagNose == 3) diagNose = 1;
        return;
    }

    if (actionFlag == 46) { //triggered by the webpage zbtest
        actionFlag = 0; //reset the actionflag
        showDir(); 
    }
    
       if (actionFlag == 47) { //triggered by the api (external)
        actionFlag = 0; //reset the actionflag
        if(diagNose == 10) diagNose = 3;
        //consoleOut("TEST actionFlag = 47 ");
        startCoordinator(); // t
        if(diagNose == 3) diagNose = 1;
    }

      
    
        // if (actionFlag == 60) {
        //     actionFlag = 0; //reset the actionflag
        //     joinOnActionflag();
        // }
    
    //    if (actionFlag >= 80 && actionFlag < 90) 
    //     { //triggered by the api (external)
    //       // this function sends interview messages
    //       char mes[50]={"\0"};
    //       int devNr = actionFlag - 80; 
    //       if(diagNose == 10) diagNose = 3;  // write to the logfile
    //       // register for ZDO messages for cluster 8002
    //       strcpy(mes, "253E0280");
    //       //sendZB(mes);
    //       //empty_serial2();
    //       // register an endpoint
    //       strcpy(mes, "2400010401000000000000");
    //       //sendZB(mes);
    //       //empty_serial2();
    //       sprintf(mes, "2502%s%s", Dev_Prop[devNr].devAdr, Dev_Prop[devNr].devAdr);
    //       //sendZB(mes);
    //       //empty_serial2();
    //       diagNose = 1;  // rest goes to the console
    //       //send the commands to interview device
    //       actionFlag = 0; //reset the actionflag
    //     }

    //    if (actionFlag >= 70 && actionFlag < 80) 
    //     { //triggered by the api (external)
    //       char mes[50]={"\0"};
    //       int devNr = actionFlag - 70; 
    //       diagNose = 1;  // write to the console
    //       if(Dev_Prop[devNr].devType == 1 || Dev_Prop[devNr].devType == 2) 
    //       {  interviewFlag = devNr; // this waits untill device is awake
    //          consoleOut("set interviewFlag =  " + String(devNr));
    //          actionFlag = 0; 
    //          return;
    //       }
    //       interviewDevice(devNr);
    //       diagNose = 1;  // rest goes to the console
    //       //send the commands to interview device
    //       actionFlag = 0; //reset the actionflag
    //     }



// mosquitto test, drop connection, reconnect and sens testmessage
    if (actionFlag == 49) 
    { //triggered by console testmqtt
        actionFlag = 0; //reset the actionflag
        my_wifiConnect.ledBlink(150,0,0,1,100);
        // always first drop the existing connection
        if(diagNose == 10) diagNose = 3;
        MQTT_Client.disconnect();
        consoleOut("mqtt forced drop connection");
        delay(100);
        char Mqtt_send[26] = {0};
       
        if(mqttConnect() ) {
            String toMQTT=""; // if we are connected we do this
            strcpy( Mqtt_send , Mqtt_outTopic);
        
            //if(Mqtt_send[strlen(Mqtt_send -1)] == '/') strcat(Mqtt_send, String(Inv_Prop[0].invIdx).c_str());
            toMQTT = "{\"test\":\"" + String(Mqtt_send) + "\"}";
          
             if( MQTT_Client.publish (Mqtt_outTopic, toMQTT.c_str() ) ) {
                  consoleOut("sent mqtt message : " + toMQTT);
             } else {
                  consoleOut("sending mqtt message failed : " + toMQTT);    
             }
        } 
     // the not connected message is displayed by mqttConnect()
    }
actionFlag = 0;

} // end test actionflag
