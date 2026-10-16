// void startCoordinator() {
//     Serial.println("[ZIGBEE INIT] start via pure SDK-functions...");

//     // 1. Basisconfiguratie aanmaken
//     esp_zb_cfg_t zb_nwk_cfg = {
//         .esp_zb_role = ESP_ZB_DEVICE_TYPE_COORDINATOR,
//         .install_code_policy = false
//     };

//     // 2. Initialiseer de stack (geeft void terug in deze SDK)
//     esp_zb_init(&zb_nwk_cfg);

//     // 3. Start de stack (Gecorrigeerd: vereist 'false' en geeft esp_err_t terug!)
//     esp_err_t err = esp_zb_start(false);
    
//     if (err == ESP_OK) {
//         Serial.println("[ZIGBEE SUCCESS] esp_zb_start succesfull !");
//     } else {
//         Serial.printf("[ZIGBEE ERROR] esp_zb_start failed with code: 0x%X\n", err);
//     }
// }


void startCoordinator() {

 Serial.println("Initialising the Universal Coordinator ---");
  Zigbee.setDebugMode(true);
  
  if (!Zigbee.begin(ZIGBEE_COORDINATOR, false)) {

    Serial.println(">>> ERROR coordinator not started!");

    my_wifiConnect.ledBlink(255, 0, 0, 5, 200);
    zigbeeUp = 0;

    return;
  }

  Serial.println("Universal Coordinator successfully started!");

  my_wifiConnect.ledBlink(0, 150, 0, 3, 250);

  zigbeeUp = 1;
 
  // testen van functies
  if( Zigbee.started() ) Serial.println("zigbee started");
  Serial.println("zb role = " + String(Zigbee.getRole() ));

}
  
  


