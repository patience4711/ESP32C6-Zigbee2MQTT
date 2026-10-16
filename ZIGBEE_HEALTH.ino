// *************************************************************************
//                   system healtcheck 
//**************************************************************************

void healthCheck() {
    #ifdef TEST
    return;
    #endif
    //Serial.flush();

    if(Mqtt_Format != 0 && Mqtt_Format != 5) {
       if (mqttConnect() ) {
       consoleOut("health mosquitto ok");
       }
 
    }

    if(!timeRetrieved) { 
      getTijd();
      eventSend(1);
    }

 
    if(zigbeeUp == 1 && isCoordinatorActive()) 
    {
    Serial.println("healthcheck says coordinator up");
  
    } else {
      Serial.println("healthcheck says coordinator down. Starting it");
      startCoordinator(); // start the coordinator
    }
  
  if(zigbeeUp != 1) {
    consoleOut("coordinator still down");
  }
}


 // =========================================================================
// FUNCTION TO CHECK IF THE COORDINATOR IS ACTUALLY ALIVE & RUNNING
// =========================================================================
bool isCoordinatorActive() {
  // Query the underlying Zigbee radio stack for its current running role
  zigbee_role_t currentRole = Zigbee.getRole();
  
  // Return true only if it is configured and actively operating as the coordinator
  return (currentRole == ZIGBEE_COORDINATOR);
}   
