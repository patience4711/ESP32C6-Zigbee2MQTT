
// void ledBlink(uint8_t red, uint8_t green, uint8_t blue, int times, int duration_ms) {
  
//   for (int i = 0; i < times; i++) {
//     rgbLedWrite(RGB_LED_PIN, red, green, blue); // Turn LED ON with color
//     delay(duration_ms);
//     rgbLedWrite(RGB_LED_PIN, 0, 0, 0);          // Turn LED OFF
//     delay(duration_ms);
//   }
// }


void empty_serial() { // remove any remaining data in serial buffer
  while(Serial.available()) {
    Serial.read();
  }
}


void delayedReset() {
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  SPIFFS.end();       // Recommended before formatting
  SPIFFS.format();
  ESP.restart();
}

void flashErase(AsyncWebServerRequest *request) {
// call this from the portal to erase the wifi and the flash
     procesId = 2;
     confirm();
     request->send(200, "text/html", toSend);
     resetTicker.once(3.0, delayedReset); // 1-second delay
}

void eventSend(byte what) {
  if (what == 1) {
      events.send( "general", "message"); //getGeneral triggered            
  } else {
     events.send( "getdata", "message"); // both triggered
  }
}
// convert the type esp_zb_ieee_addr_t to a string
String ieeeToHexString(esp_zb_ieee_addr_t addr) {
    char buf[17]; // 16 karakters + \0 terminator
    snprintf(buf, sizeof(buf), "%02X%02X%02X%02X%02X%02X%02X%02X",
             addr[0], addr[1], addr[2], addr[3], 
             addr[4], addr[5], addr[6], addr[7]);
    return String(buf);
}

    

String getChipId(bool sec) {
    uint32_t chipId = 0;
    for(int i=0; i<17; i=i+8) {
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }
  if(sec) return String(chipId); else return "ESP32-ZIGBEE-" + String(chipId);
}

void showDir() {
    char temp[50]={0};
    File root = SPIFFS.open("/");
    File file = root.openNextFile();
    while (file) {
      sprintf(temp, "%s  size %u<br>" , file.name(), file.size() );
      consoleOut(String(temp));
      delay(100);
      file = root.openNextFile();
    }
  }

  // function to 
void consoleOut(String toLog) {

 switch (diagNose) 
    { 
      case 1:       
          delay(100); // otherwise the socket cannot keep up
          ws.textAll(toLog);
          break;
      case 2: 
          Serial.println(toLog);
          break;
      default:
      break;
    }
}

void pairOut(String toLog) {
      toLog += "\n";
      debugLog += toLog; 
}


// //  ************** preferences ****************

// void saveLamp(uint8_t index, lamps &lamp) 
// {
//   if (index >= 9) return;

//   prefs.begin("lampdata", false);

//   char key[8];
//   snprintf(key, sizeof(key), "lamp%u", index);

//   prefs.putBytes(key, &lamp, sizeof(lamps));
//   prefs.end();
// }

// void getLamps()
// {
//   for (uint8_t i = 0; i < 9; i++) {
//     loadLamp(i, lampState[i]);
//   }
// }

// void loadLamp(uint8_t index, lamps &lamp) 
// {
//   if (index >= 9) return;
//   prefs.begin("lampdata", true);

//   char key[8];
//   snprintf(key, sizeof(key), "lamp%u", index);

//   if (prefs.isKey(key)) {
//     prefs.getBytes(key, &lamp, sizeof(lamps));
//   } else {
//     // Defaults if never saved
//     lamp = { false, 0, 0, 0, 0 };
//   }

//   prefs.end();
// }


  bool structExists(int idx) {
  if (idx < 0 || idx >= 9) return false;
  
  // a device exists when isActive true
  if (!Dev_Prop[idx].isActive) {
    return false;
  }
  return true;
}

// // Helperfunction to find index in structs with specific shortAddr
// int deviceLookup(uint16_t shortAddr) {
//   for (int i = 0; i < 9; i++) {
//     if (Dev_Prop[i].shortAddr == shortAddr) {
//       return i;
//     }
//   }
//   return -1; // Niet gevonden
// }
// Helperfunction to find index in structs with specific shortAddr or idx
int deviceLookup(uint16_t shortAddr, uint16_t gezochtIdx) {
  
  for (int i = 0; i < devCount; i++) {
    
    // Optie A: seek on Short Address (if shortAddr not 0)
    if (shortAddr != 0 && Dev_Prop[i].shortAddr == shortAddr) {
      return i; // found on short
    }
    
    // Optie B: seek on Idx ( when not 0)
    if (gezochtIdx != 0 && Dev_Prop[i].devIdx == gezochtIdx) {
      return i; // found on idx!
    }
  }
  
  return -1; // no result
}


void initDevices() {
    
    consoleOut("reset devices to a known state");
    for(int x = 0; x < devCount; x++)
    {
    consoleOut("device nr " + String(x));
      lampState[x].onoff = false;
      lampState[x].present[R] = 100;
      lampState[x].present[G] = 100;
      lampState[x].present[B] = 100;
      lampState[x].present[D] = 100;
      lampState[x].mode = 5;
      lampState[x].colorTemp = 0;

      if(Dev_Prop[x].devType==3) bulbOff(x);
      if(Dev_Prop[x].devType==4) switchOff(x);
      //if(Dev_Prop[x].devType==3) rgblampListener.lightOff(Dev_Prop[x].endpoint, Dev_Prop[x].shortAddr);
    }
 } 

 uint16_t generateIeee(const esp_zb_ieee_addr_t ieee) {
  // 1. Combineer de laatste 4 bytes tot één groot getal
  uint32_t bigKey = ((uint32_t)ieee[4] << 24) | 
                          ((uint32_t)ieee[5] << 16) | 
                          ((uint32_t)ieee[6] << 8)  | 
                           (uint32_t)ieee[7];
                           
  // 2. Dwing het getal in de range 1000 t/m 9999
  // % 9000 geeft een waarde tussen 0 en 8999. + 1000 maakt daar 1000 t/m 9999 van.
  uint16_t foorDigits = (bigKey % 9000) + 1000;
  
  return foorDigits;
}

void loginBoth(AsyncWebServerRequest *request, String who) {
  String authFailResponse = "<h2>login failed <a href='/'>click here</a></h2>";
  if (who == "admin" ){
  const char* www_realm = "login as administrator."; 
    if (!request->authenticate("admin", pswd)) return request->requestAuthentication();
  }
  if (who == "both" ){
  const char* www_realm = "login as administrator or user."; 
    if (!request->authenticate("admin", pswd) && !request->authenticate("user", userPwd)) return request->requestAuthentication();
  }
}