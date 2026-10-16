

void handleJoin(AsyncWebServerRequest *request) {
     //the debugLog is initialized with a close link
     debugLog = "<h2><a href='/DEV?welke=" + String(toJoin) + "'>close</a></h2>";
     
     pairOut("starting join process for device " + String(toJoin));
     joinSuccess = false;
     permitJoin(120);
     toSend=FPSTR(WAIT_JOIN);
     toSend.replace("{#}", String(toJoin));
     request->send(200, "text/html", toSend); //send the html code to the client
}


void permitJoin(uint8_t seconds) {

  if (!isCoordinatorActive()) {
    pairOut("coordinator is offline!");
    return;
  }

  if (seconds == 255) {
    Zigbee.openNetwork(255);
    networkOpen = true;
    pairOut("Network permanently open for joining.");
  } 
  else {
    pairOut("opening zigbee network 120 seconds for pairing ");
    Zigbee.openNetwork(seconds);
    //Serial.print("zigbee network opened");
    pairingWindowDurationSecs = seconds;
    networkOpenTime = millis();
    networkOpen = true;
  }

  my_wifiConnect.ledOn(0, 100, 100); //magenta led on
}

void onDeviceJoin(uint16_t shortAddr, esp_zb_ieee_addr_t ieeeAddr)
{
  pairOut("Device joined toJoin = " + String(toJoin));
  pairOut("closing network for joining");
  // close the network immediately
  pairingWindowDurationSecs = 0;
  networkOpenTime = millis();
  Zigbee.closeNetwork();
  networkOpen = false;
  pairingWindowDurationSecs = 1;
  networkOpenTime = millis();
  
  my_wifiConnect.ledOff(); //
  addDevice(shortAddr, ieeeAddr);
  joinSuccess = true;
  
  discoverDevice(shortAddr); // also binds

}

// =========================================================================
// GEPAIRD APPARAAT OPSLAAN IN JOUW DEV_PROP STRUCT
// =========================================================================
void addDevice(uint16_t shortAddr, esp_zb_ieee_addr_t ieeeAdd)
{
  char hexAddr[10];
  snprintf(hexAddr, sizeof(hexAddr), "%04X", shortAddr);

  String addrStr = String(shortAddr, HEX);
  addrStr.toUpperCase(); // 
  pairOut("obtained devAdr : " + addrStr);
  pairOut("Unique devIeee  : " + ieeeToHexString(ieeeAdd));

  Dev_Prop[toJoin].devIdx = generateIeee(ieeeAdd);
  pairOut("generated IDX : " + String(Dev_Prop[toJoin].devIdx));
   // save Short address
  Dev_Prop[toJoin].shortAddr = shortAddr;
  
  //declared in the struct esp_zb_ieee_addr_t devIeee;
  memcpy(Dev_Prop[toJoin].devIeee, ieeeAdd, sizeof(esp_zb_ieee_addr_t));
  
  Serial.println("==================================================");

// save it in preferences
  pairOut("saving the struct index " + String(toJoin));
  savePrefs(toJoin); // save in preferences
}



bool hasCluster(int devIdx, uint16_t gezocht_cluster) {
    // We scannen alle 20 plekken, ongeacht of er een 0 (Basic Cluster) tussen staat
    for (int i = 0; i < 20; i++) {
        uint16_t huidig_cluster = Dev_Data[devIdx].inputClusters[i];
        
        // Hebben we het gezochte cluster (bijv. 0x0500) gevonden?
        if (huidig_cluster == gezocht_cluster) {
            pairOut("the sought cluster is found");  
            return true; // Match! Direct stoppen en true teruggeven
        }
    }
    
    // Pas als we de hele array van 20 hebben doorgezocht zonder match:
    pairOut("the sought cluster is not present");
    return false; 
}


