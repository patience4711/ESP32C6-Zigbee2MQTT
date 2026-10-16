

#include "AAA_INCLUDES.h"

  // ==========================================
  // PRE-DECLARATIES (Prototypes)
  // ==========================================
  // Hier geef je de standaardwaarde (= -1) op!
  void loadPrefs(int idx = -1);

// Dwing de compiler om ZBOSS als pure C te behandelen (voorkomt de linker-fout)
extern "C" {
  #define ZB_ENABLE_HA
  #define ZB_ENABLE_ZCL
  #include "zboss_api.h"
} 


 
 ZigbeeSwitch  switchListener(1);
 ZigbeeContactSwitch motionListener(2);       // Luistert naar Temperatuur/Vochtigheid op EP 2
 //ZigbeeColorDimmableLight  rgbLampListener(3); 
 ZigbeeTempSensor  tempListener(4);
 ZigbeeColorDimmerSwitch rgblampListener(3);
/* we kunnen ook dit doen
 ZigbeeColorDimmerSwitch socketLuisteraar(1);
  Zo kunnen we de RGB_ON en RGB-OFF methodes gebruiken voor de socket
*/

// *****************************************************************************
// *                              SETUP
// *****************************************************************************
void setup() 
{
  Serial.begin(115200);

  //Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  //Serial.println(F("Serial Txd is on pin: 17"));
  //Serial.println(F("Serial Rxd is on pin: 16"));


  pinMode(knop, INPUT_PULLUP);  // de knop
  //pinMode(led_onb, OUTPUT);     // onboard led
  //pinMode(RGB_LED_PIN, OUTPUT);
  pinMode(4, OUTPUT);    //
  digitalWrite(4, LOW);  //

  my_wifiConnect.ledBlink(0,150,0,1,800);

  attachInterrupt(digitalPinToInterrupt(knop), isr, FALLING);

  SPIFFS_read();
  // we set deviceCount to the number of inverterfiles we find
  loadPrefs(); // now devCount is known
  
  Serial.println("\ndevCount = " + String(devCount));
  Serial.println("\ndiagNose = " + String(diagNose));
  if (diagNose != 0) printDevices();  // show all the device files

  loadPasswd(); // the adm passwd and security level
  
  // prefs.begin("my_data", false);  //open preferences for r/w
  // String key = "req";
  // prefs.getString(key.c_str(), requestUrl, sizeof(requestUrl));
  // Serial.println("requestUrl = " + String(requestUrl));
  // prefs.end();  // takes care for the return to the last webpage after reboot
   
  // FORCE the radio to prepare for simultaneous Wi-Fi and 802.15.4 (Zigbee) sharing
  esp_coex_wifi_i154_enable();  //?????? in case of troubles
  esp_coex_preference_set(ESP_COEX_PREFER_BALANCE);
  
  //start_wifi();  // start wifi and server
  my_wifiConnect.start_wifi();
  // ledBlink(0, 0, 150, 3, 250); // blink blue
  Serial.println(WiFi.localIP());
  delay(100);
  start_server();
  getTijd();  // retrieve time from the timeserver
  Update_Log(1, "boot up");

  // ****************** mqtt init *********************
  MQTT_Client.setKeepAlive(150);
  MQTT_Client.setServer(Mqtt_Broker, atoi(Mqtt_Port));
  // MQTT_Client.setBufferSize(300); //to avoid freeze ups
  MQTT_Client.setBufferSize(1024);
  MQTT_Client.setCallback(MQTT_Receive_Callback);

  if (Mqtt_Format != 0) {
    Serial.println(F("setup: mqtt configure"));
    mqttConnect();  // mqtt connect
  } else {
    Update_Log(3, "not enabled");
  }

  initWebSocket();
  
  Serial.println(F("booted up, checking coordinator"));
  //Serial.println(WiFi.localIP());

  delay(100);
  //ledblink(3, 500);
  //register the callback for join
   //Zigbee.onDeviceJoin(onDeviceJoin);
  //Zigbee.onEvent(onZigbeeEvent);
  //esp_zb_add_app_signal_cb(onZigbeeSignal);
  
  
  // register the callback for all incoming messages
  //esp_zb_raw_command_handler_register(my_raw_rx_handler);
  
  Zigbee.addEndpoint(&rgblampListener);
  Zigbee.addEndpoint(&switchListener);
  Zigbee.addEndpoint(&tempListener);
  Zigbee.addEndpoint(&motionListener); 
  
  Update_Log(1, "healthcheck");
  healthCheck();  // check the state of the zigbee system and starts it when neseccary
  if (zigbeeUp == 1) {
    Update_Log(2, "running");
  }
  esp_zb_aps_data_indication_handler_register((esp_zb_apsde_data_indication_callback_t)my_aps_data_interceptor);
    // Registreer de algemene actie-handler
  esp_zb_core_action_handler_register(zb_action_handler);

  //resetCounter = 0;
  //events.send( "reload", "message"); //getGeneral and getAll triggered
  
  
   initDevices(); // set all devices to a known stat and off
  
  eventSend(0);
  eventSend(1); // the general part of the page



}  // end setup()

//****************************************************************************
//*                         LOOP
//*****************************************************************************
void loop() {


  // ******************************************************************
  //              healthcheck every 10 minutes
  // ******************************************************************

  unsigned long nu = millis();          //  //
  if (nu - lastCheck >= 1000UL * 6000)  // =10min
  {
    Serial.println("6000 secs passed, healthcheck" + String(millis()));
    lastCheck += 1000UL * 6000;
    //we dont do healtcheck when stopped
    healthCheck();  // checks zb radio, mqtt and time, when false only message if error
  }

    // TIMING CHECK FOR NETWOK OPEN---
  static uint32_t last_timer_millis = 0;
  
  // Voer deze controle elke 10 seconden (10000 milliseconden) uit
  if (millis() - last_timer_millis > 10000) {
    last_timer_millis = millis();
    
    // Als het netwerk open staat, controleren we of de ingestelde tijd is verstreken
    if (networkOpen && (millis() - networkOpenTime > (pairingWindowDurationSecs * 1000))) {
      
      networkOpen = false; // Zet de vlag op gesloten
      
      pairOut("pairing-time is expired. Network is closed.");
      
      // Optional: put the onboard led off
      my_wifiConnect.ledOff(); 
    }
    
    // Optional: Print a live countdown as long as the port is open
        if (networkOpen) {
      uint32_t secondsLeft = pairingWindowDurationSecs - ((millis() - networkOpenTime) / 1000);
      pairOut("network is open for join! Remaining time: " + String(secondsLeft));
    }
  }
  // --- END OF TIMING CHECK ---
  
  
  
  
  // we recalcultate the switchtimes for this day when there is a new date
  // if retrieve fails, day will not be datum, so we keep trying by healthcheck
  if (day() != datum && hour() > 2)  // if date overflew and later then 2
  {
    getTijd();  // retrieve time and recalculate the switch times
                //delay(500);
                //ESP.restart();
                //DebugPrintln("date overflew, retrieve time");
  }


  // Check if 1 minute has passed since shouldDo was set true
  if (mqttFlag != 0 && millis() - triggerTime >= 1000UL * 60) {
    char toMQTT[100] = { 0 };

    snprintf(toMQTT, sizeof(toMQTT), "{\"mqttFlag\",\"idx\":%d,\"nvalue\":0,\"svalue\":\"%.1d\"}", mqttFlag, 0);
    // mqttConnect() checks first if we are connected, if not we connect anyway
    //consoleOut("toMQTT = " + String(toMQTT));
    if (mqttConnect()) MQTT_Client.publish(Mqtt_outTopic, toMQTT);
    mqttFlag = 0;  // reset the flag (optional)
  }



  // ***************************************************************************
  //                       m q t t
  // ***************************************************************************
  // before each transmission the connection is tested
  // so we don't do this in the loop
  if (Mqtt_Format != 0) MQTT_Client.loop();  //looks for incoming messages

  // //*********************************************************************
  // //             send null data at midnight
  // // ********************************************************************
  // if(hour() == 0 && timeRetrieved && midnightFlag == 250)
  // {
  //     if(second() > 0 )
  //     {
  //       // resetValues(true, true); //set all values to zero and sent mqtt
  //       // Update_Log(1, "values wipe");
  //       // midnightFlag = 0; // to prevent repetition
  //       // //events.send( "getall", "message"); // refresh the data
  //       // eventSend(2);
  //      }
  // }

  test_actionFlag();

  checkPendingflag();

  checkMqttflag();
  
  ws.cleanupClients();
  yield();  // to avoid wdt resets

  // SERIAL: *************** kijk of er data klaar staat op de seriele poort **********************
  if (Serial.available()) {
    handle_Serial();
  }
}
//****************  End Loop   *****************************
