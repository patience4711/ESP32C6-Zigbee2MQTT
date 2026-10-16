bool mqttConnect() {   // 
/* this function checks if we are connected to the broker, if not connect anyway */  
    if( MQTT_Client.connected() ) {
    //consoleOut("mqtt was connected");
    return true;
    }
    // we are here because w'r not connected. Signal with the LED
    consoleOut("mqtt connecting");
    my_wifiConnect.ledBlink(150,0,0,2,70);

    if (Mqtt_Port[0] == '\0' ) strcpy(Mqtt_Port, "1883");   // just in case ....
    uint8_t retry = 3;
    
    //String Clientid = getChipId(false); 

    while (!MQTT_Client.connected()) {

      if ( MQTT_Client.connect( getChipId(false).c_str(), Mqtt_Username, Mqtt_Password) )
      {
         //connected, so subscribe to inTopic (not for thingspeak)
        if(Mqtt_Format != 5 ) {
        //String clientid = getChipId(false) + "/in"; 
        //if(  MQTT_Client.subscribe ( clientid.c_str() ) ) {
        if(  MQTT_Client.subscribe ( Mqtt_inTopic ) ) { 
               consoleOut("subscribed to " + String(Mqtt_inTopic ));
               //consoleOut("subscribed to " + clientid);
           }
        }
         consoleOut(F("mqtt connected"));
         Update_Log(3, "connected");
      
       return true;

    } else {
        //String term = "connection failed state: " + String(MQTT_Client.state());
        Update_Log(3, "failed");
        if (!--retry) break; // stop when tried 3 times
        delay(500);
    }
  }
  // if we are here , no connection was made.

  consoleOut(F("mqtt connection failed"));
  return false;
}

// *************************************************************************
//                   process received mqtt
// *************************************************************************

void MQTT_Receive_Callback(char *topic, byte *payload, unsigned int length)
{
    JsonDocument doc;                                 
    String jsonString = "";

    DeserializationError error = deserializeJson(doc, payload, length); 
    if (error)            
    {
       consoleOut("mqtt no valid json ");
       return;
    } 
    
    int dev = -1; 
    int idx = 0;

    if (!doc["idx"].isNull())
    {
      idx = doc["idx"].as<int>();
      consoleOut("there is an idx " + String(idx));
      
      // Zoek het apparaat op
      dev = deviceLookup(0, idx);
      consoleOut("message is for devNr " + String(dev));
    } else {
      consoleOut("there is no idx, abort");
      return;
    }
   
    // check that the index is valid
    if (dev >= 0 && dev < devCount)
    {
        consoleOut("DevType for this device is: " + String(Dev_Prop[dev].devType));

        switch(Dev_Prop[dev].devType) 
        {
           case 3: // smartbulb
              if(doc.containsKey("state")) {
                 const char* state = doc["state"];
                 if (strcasecmp(state, "ON") == 0) bulbOn(dev); else bulbOff(dev);
                 consoleOut("MQTT: Command sent to (dev " + String(dev) + ")");
              }
              break;
        
           case 4: // smartsocket
              if(doc.containsKey("state")) {
                 const char* state = doc["state"];
                 if (strcasecmp(state, "ON") == 0) switchOn(dev); else switchOff(dev);
                 consoleOut("MQTT: Command sent to (dev " + String(dev) + ")");
              }
              break;

           default:
              consoleOut("MQTT: Device found, but devType " + String(Dev_Prop[dev].devType) + " is not supported in switch.");
              break;
        }
    } 
    else 
    {
        
        consoleOut("MQTT: Error no known device found for this IDX.");
    }
}


void sendMqttmessage(int devNr) 
{
    consoleOut("need to send an mqtt for device "+ String(devNr));   
    char toMQTT[120]; // Iets ruimer genomen voor de veiligheid
    
    // Begin de JSON string met de idx en zorg dat de syntax klopt (Dev_Prop[devNr].idx ipv devIdx, pas aan indien nodig)
    snprintf(toMQTT, sizeof(toMQTT), "{\"idx\":%d", Dev_Prop[devNr].devIdx);
    int currentLength = strlen(toMQTT); 

switch (Dev_Prop[devNr].devType) 
    {
        case 1:  // temp & hum
            snprintf(toMQTT + currentLength, sizeof(toMQTT) - currentLength, 
                     ",\"temp\":%.1f,\"hum\":%.1f}", 
                     Dev_Data[devNr].values[0], Dev_Data[devNr].values[1]); 
            break;
            
        case 2:  // motion (stuurt 1 bij beweging, 0 bij rust)
            snprintf(toMQTT + currentLength, sizeof(toMQTT) - currentLength, 
                     ",\"motion\":%d}", 
                     lampState[devNr].onoff ? 1 : 0); // Of Dev_Data[devNr].values[0] als je dat gebruikt
            break;
            
        case 3: // rgb lamp
        case 4: // socket (Beide sturen nu een pure 1 of 0)
            snprintf(toMQTT + currentLength, sizeof(toMQTT) - currentLength, 
                     ",\"state\":%d}", 
                     lampState[devNr].onoff ? 1 : 0);
            break;
        default:
            consoleOut("MQTT could not send this message");
    }

    //send the constructed message to the outtopic 
    consoleOut("Sending MQTT: " + String(toMQTT));
    MQTT_Client.publish(Mqtt_outTopic, toMQTT); 
}


 

void checkMqttflag() {
  //check for every device if there should be set a toMqtt
    for(int x=0; x <devCount; x++) 
    {
      if(Dev_Prop[x].toMqtt == true) {
        consoleOut("found a toMqtt = true for device " + String(x));
        // the flag is set so we reset it
        Dev_Prop[x].toMqtt = false;
        // the device nr is x
        sendMqttmessage(x);
      }

    }
}