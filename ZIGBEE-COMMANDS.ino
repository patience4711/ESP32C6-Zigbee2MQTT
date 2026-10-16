
// // for the socket, but maybe we can use the methods of the lampLuisteraar
// void switchOnOff(uint8_t endpoint, uint16_t shortAddr, bool on)
// {
//   esp_zb_zcl_on_off_cmd_t cmd_req;
//   memset(&cmd_req, 0, sizeof(cmd_req));

//   cmd_req.zcl_basic_cmd.src_endpoint = 1;
//   cmd_req.zcl_basic_cmd.dst_endpoint = endpoint;
//   cmd_req.zcl_basic_cmd.dst_addr_u.addr_short = shortAddr;

//   cmd_req.address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT;

//   if (on) {
//     cmd_req.on_off_cmd_id = ESP_ZB_ZCL_CMD_ON_OFF_ON_ID;
//     Serial.printf("[CMD] ON -> 0x%04X EP %u\n", shortAddr, endpoint);
//   } else {
//     cmd_req.on_off_cmd_id = ESP_ZB_ZCL_CMD_ON_OFF_OFF_ID;
//     Serial.printf("[CMD] OFF -> 0x%04X EP %u\n", shortAddr, endpoint);
//   }

//   esp_zb_zcl_on_off_cmd_req(&cmd_req);
// }

// void sendLampOnOff(uint16_t shortAddr, uint8_t endpoint, bool on)
// {

//   if (on) {
//     rgblampListener.lightOn(endpoint, shortAddr);
//   } else {
//     rgblampListener.lightOff(endpoint, shortAddr);
//   }

// }
void switchOn(int which)
{
     //switchListener.lightOn(Dev_Prop[which].endpoint, Dev_Prop[which].shortAddr);
     // We geven het endpoint en het 8-byte IEEE-adres mee uit je struct
     switchListener.lightOn(Dev_Prop[which].endpoint, Dev_Prop[which].devIeee);
     
     lampState[which].onoff = true;
}

void switchOff(int which)
{
     Serial.printf("[SWITCH] Probeer UIT via IEEE voor index %d\n", which);
     //switchListener.lightOff(Dev_Prop[which].endpoint, Dev_Prop[which].shortAddr);
     // Idem voor uit
     switchListener.lightOff(Dev_Prop[which].endpoint, Dev_Prop[which].devIeee);
     
     lampState[which].onoff = false;
}
// void switchOff(int which)
//  {
//      switchListener.lightOff(Dev_Prop[which].endpoint, Dev_Prop[which].shortAddr);
//      lampState[which].onoff = false;
// }
// void switchOn(int which)
//  {
//      switchListener.lightOn(Dev_Prop[which].endpoint, Dev_Prop[which].shortAddr);
//      lampState[which].onoff = true;
// }

void switchOnOff(int which)
  {
      //this is a toggle, 
      bool currentState = lampState[which].onoff;
      if (currentState == false) {
      switchOn(which);
      } else {
      switchOff(which);
      }
  }

void bulbOff(int which)
 {
     rgblampListener.lightOff(Dev_Prop[which].endpoint, Dev_Prop[which].shortAddr);
     lampState[which].onoff = false;
}
void bulbOn(int which)
 {
     rgblampListener.lightOn(Dev_Prop[which].endpoint, Dev_Prop[which].shortAddr);
     lampState[which].onoff = true;
}

void bulbOnOff(int which)
{
  //this is a toggle, 
  bool currentState = lampState[which].onoff;
  if (currentState == false) {
     bulbOn;
   //rgblampListener.lightOn(Dev_Prop[which].endpoint, Dev_Prop[which].shortAddr);
  //  lampState[which].onoff;
  } else {
    bulbOff;
   // rgblampListener.lightOff(Dev_Prop[which].endpoint, Dev_Prop[which].shortAddr);
  //  lampState[which].onoff = false;
  }

}

void bulbSetBrightness() 
{
    if(lampState[devChoice].onoff == false) bulbOn(devChoice);  // need to put on first 
    rgblampListener.setLightLevel(
        lampState[devChoice].present[D],
        Dev_Prop[devChoice].endpoint, 
        Dev_Prop[devChoice].shortAddr
    );
}

void bulbSetColor() 
{
    if(lampState[devChoice].onoff == false) bulbOn(devChoice);  // need to put on first 
    rgblampListener.setLightColor(
        lampState[devChoice].present[R],
        lampState[devChoice].present[G],
        lampState[devChoice].present[B],
        Dev_Prop[devChoice].endpoint, 
        Dev_Prop[devChoice].shortAddr);
}

void bulbSetPreset() 
{
// the lamp should be set on when it is not becous set colot etc
// wont work when the lamp is out
if(lampState[devChoice].onoff == false) bulbOn(devChoice);
    rgblampListener.setLightColor(
        Dev_Data[devChoice].preset[R],
        Dev_Data[devChoice].preset[G],
        Dev_Data[devChoice].preset[B],
        Dev_Prop[devChoice].endpoint, 
        Dev_Prop[devChoice].shortAddr);
        // also set the brightness
        rgblampListener.setLightLevel(
           Dev_Data[devChoice].preset[D],
           Dev_Prop[devChoice].endpoint, 
           Dev_Prop[devChoice].shortAddr
    );
    //bulbOn(devChoice);
}
void bulbSetWarm() {
      if(lampState[devChoice].onoff == false) bulbOn(devChoice);  // need to put on first    
      // sets a colortemperature warmwhite and puts the lamp on
      rgblampListener.setLightColor( 255, 197, 143, 
          Dev_Prop[devChoice].endpoint, 
          Dev_Prop[devChoice].shortAddr); 
      //bulbOn(devChoice);         
}
void bulbSetCold() {
      if(lampState[devChoice].onoff == false) bulbOn(devChoice);  // need to put on first 
      rgblampListener.setLightColor( 201, 226, 255, 
          Dev_Prop[devChoice].endpoint, 
          Dev_Prop[devChoice].shortAddr); 
      //bulbOn(devChoice); 
}
void bulbSetNeutral() {
  if(lampState[devChoice].onoff == false) bulbOn(devChoice);  // need to put on first 
      rgblampListener.setLightColor( 255, 241, 224, 
          Dev_Prop[devChoice].endpoint, 
          Dev_Prop[devChoice].shortAddr); 
      //bulbOn(devChoice);
}

void checkPendingflag() {
    if (pendingFlag == 0) return;    
    consoleOut ("checking pending command current is " + String(pendingFlag));

    switch(pendingFlag)  {

            case 1: // the on/off button
            bulbOff(devChoice);
            //if(lampState[devChoice].onoff == false) bulbOn(devChoice); else bulbOff(devChoice);
            Dev_Prop[devChoice].toMqtt = true;
            break;

            case 2:  // set when a slider is handled
                bulbSetColor();
                Dev_Prop[devChoice].toMqtt = true;
                break;
            
            case 3:  // set when the dim slider was handled
                bulbSetBrightness();
                Dev_Prop[devChoice].toMqtt = true;
                break;

            case 4:
               bulbSetColor();
               Dev_Prop[devChoice].toMqtt = true;
               break;            
            
            case 6: // 
                bulbSetWarm();
                Dev_Prop[devChoice].toMqtt = true;
                break;
            case 7: 
                bulbSetNeutral();
                Dev_Prop[devChoice].toMqtt = true;
                break;
            case 8: 
                bulbSetCold();
                Dev_Prop[devChoice].toMqtt = true;
                break; 
            case 9: 
                bulbSetPreset();
                Dev_Prop[devChoice].toMqtt = true;
                break;          
            case 10: 
                bulbSetPreset();
                // we need to save Dev_Data to remember the preset
                savePrefs(devChoice);
                break;  
                    
            case 20: // onoff socket
                 //switchOnOff(Dev_Prop[devChoice].endpoint, Dev_Prop[devChoice].shortAddr, lampState[devChoice].onoff); 
                 switchOnOff(devChoice);  // toggle 
                 Dev_Prop[devChoice].toMqtt = true;
                break;
        }
    pendingFlag = 0;
    
    eventSend(0);
    }