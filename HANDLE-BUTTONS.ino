// this function can handle all buttons on the various pages
// methods of ZigbeeColorDimmerSwitch
//setLightLevel 
//void ZigbeeColorDimmerSwitch::setLightColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t endpoint, uint16_t short_addr) 

void handleButtons(AsyncWebServerRequest *request) {
    if (!request->hasArg("BUTTON")) {
        consoleOut("handleButtons: Argument 'BUTTON' missing!");
        request->send(400, "text/plain", "Bad Request: Missing BUTTON");
        return;
    }
      int button = request->arg("BUTTON").toInt();
      //if(button == 0) { event = 0; } else { event = 1;} //manual switched on (for security)
      if (devChoice < 0 || devChoice >= 9) { 
        Serial.println("handleButtons Error: devChoice is out of bounds (" + String(devChoice) + ")!");
        request->send(400, "text/plain", "Bad Request: Invalid devChoice");
        return;
      } 
      
      
      Serial.println("handleButtons, argument = " + String(button));
      switch(button) 
      {
         case 0: // this is the on/off button bulb
            pendingFlag = 1;
            break;        
          case 1:  // this is the warm white button
            lampState[devChoice].mode = 2;  // warm white
            pendingFlag = 6;
            break;
          case 2: 
            lampState[devChoice].mode = 3; // neutral
            pendingFlag = 7;
            break;
          case 3:
            lampState[devChoice].mode = 4; // cold
            pendingFlag = 8;
            break;
          case 4:  // the color button color modus
             lampState[devChoice].mode = 5; // color           
             pendingFlag = 4;   
             break;
          case 5:  // the preset button
               lampState[devChoice].mode = 6; // preset set
              pendingFlag = 9;
              break;
          
          case 6:  // preset save
                lampState[devChoice].mode = 6; // preset save
                memcpy(
                    Dev_Data[devChoice].preset,   // Bestemming (de array waar het naartoe moet)
                    lampState[devChoice].present, // Bron (de array waar het nu in staat)
                    sizeof(Dev_Data[devChoice].preset) // Grootte in bytes van de bestemmings-array
                );
                pendingFlag = 10;
                break;     
          case 11:
            //lampState[devChoice].onoff = !lampState[devChoice].onoff; // this toggles the value
            pendingFlag = 20;
               
            break;                                               
       }
   eventSend(0);
}