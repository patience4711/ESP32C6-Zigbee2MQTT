// helper: convert hex string substring to uint16_t
uint16_t hexToUint16(const char* str) {
  char tmp[5] = {0};
  strncpy(tmp, str, 4); // 4 chars = 2 bytes
  return (uint16_t) strtoul(tmp, nullptr, 16);
}

// Wait for a device announce and query endpoints
bool waitForPairAndQuery(char* buffer, uint16_t &shortAddr, uint8_t ieeeAddr[8] ) {
  unsigned long start = millis();
  unsigned long timeoutMs = 60000;
  while (millis() - start < timeoutMs) {
    //readZB(buffer);  
    if (strlen(buffer) < 10) continue; // too short

    // Check if it's a Device Announce (45CA)
    if (strstr(buffer, "45CA") != nullptr) {
      consoleOut(String("Device announce: ") + buffer);

      // Extract short address (bytes after FE0C45CA)
      // buffer: FE0C45CA[ShortAddr(4)][IEEE(16)]...
      int idx = strstr(buffer, "45CA") - buffer;
      char shortStr[5] = { buffer[idx+4], buffer[idx+5], buffer[idx+6], buffer[idx+7], 0 };
      shortAddr = (uint16_t) strtoul(shortStr, nullptr, 16);

      // Extract IEEE (8 bytes = 16 chars, little endian order)
      for (int j = 0; j < 8; j++) {
        char byteStr[3] = { buffer[idx+8 + j*2], buffer[idx+9 + j*2], 0 };
        ieeeAddr[7-j] = (uint8_t) strtoul(byteStr, nullptr, 16); // flip endian
      }

      consoleOut("New device joined");
      consoleOut(" Short: " + String(shortAddr, HEX));
      char ieeeStr[32] = {0};
      for (int j = 0; j < 8; j++) {
        sprintf(&ieeeStr[strlen(ieeeStr)], "%02X", ieeeAddr[j]);
      }
      consoleOut(" IEEE: " + String(ieeeStr));

      // Build Active Endpoint Request: 2505 <shortLE><shortLE>
      char activeReq[20];
      sprintf(activeReq, "2505%02X%02X%02X%02X",
              shortAddr & 0xFF, (shortAddr >> 8) & 0xFF,
              shortAddr & 0xFF, (shortAddr >> 8) & 0xFF);
      //sendZB(activeReq);

      // Now wait for 4585
      unsigned long subStart = millis();
      while (millis() - subStart < 5000) {
        //readZB(buffer);
        if (strstr(buffer, "4585") != nullptr) {
          consoleOut(String("ActiveEP Response: ") + buffer);
          return true;
        }
      }
    }
  }

  consoleOut("No device joined during pairing window");
  return false;
}
void readZBbuffer()
{

    unsigned long start = millis();
    char buffer[256] = {0};
    bool gotResponse = false;

    while (millis() - start < 5000) 
    {  // wait up to 2 seconds
       //readZB(buffer);
        if (strlen(buffer) < 8) continue;

        // Look for ZCL Read Attributes Response (0x8100)
        if (strstr(buffer, "8100") != nullptr && strstr(buffer, "C021") != nullptr) {
            consoleOut(String("Attribute response: ") + buffer);
            gotResponse = true;
            break;
        }
    }

    if (!gotResponse) consoleOut("No attribute response yet");
}

bool waitForZclResponse(uint16_t shortAddr, char* resultBuffer ) {
    unsigned long start = millis();
    unsigned long timeoutMs = 5000;
    char fullBuffer[512] = {0};

    while (millis() - start < timeoutMs) {
        char temp[256] = {0};
        //readZB(temp);
        if (strlen(temp) > 0) {
            strncat(fullBuffer, temp, sizeof(fullBuffer) - strlen(fullBuffer) - 1);

            // Check if an 8100 frame with our short address appears
            char shortStr[5];
            sprintf(shortStr, "%02X%02X", shortAddr & 0xFF, shortAddr >> 8); // little-endian

            if (strstr(fullBuffer, "8100") && strstr(fullBuffer, shortStr)) {
                strcpy(resultBuffer, fullBuffer);
                return true;
            }
        }
        delay(50);
    }

    return false;
}