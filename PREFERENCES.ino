
void loadPasswd() {
  prefs.begin("wifi_conf", true);
  
  String temp = prefs.getString("pswd", "0000");
  strncpy(pswd, temp.c_str(), sizeof(pswd) - 1);
  pswd[sizeof(pswd) - 1] = '\0'; // null-terminator
  
  securityLevel  = prefs.getInt("seclvl", 6);
  
  // temp = prefs.getString("req", "/",);
  // strncpy(requestUrl, temp.c_str(), sizeof(requestUrl) - 1);
  // requestUrl[sizeof(requestUrl) - 1] = '\0'; // null-terminator

  consoleOut("requestUrl = " + String(requestUrl));
  consoleOut("the read admin pswd = " + String(pswd));
  consoleOut("the read securityLevel = " + String(securityLevel));
   prefs.end();
}

void savePrefs(int idx) {

  if (idx < 0 || idx >= 9) {
    consoleOut("Fout: Index out of reach!");
    return;
  }

  // Open de namespace in write-mode
  prefs.begin("zigbee_storage", false);

  // Maak unieke sleutels aan op basis van de index (bijv. "prop_1" en "data_1")
  char propKey[10];
  char dataKey[10];
  sprintf(propKey, "prop_%d", idx);
  sprintf(dataKey, "data_%d", idx);

  // Sla alleen de struct op de specifieke index op
  prefs.putBytes(propKey, &Dev_Prop[idx], sizeof(Dev_Prop[idx]));
  prefs.putBytes(dataKey, &Dev_Data[idx], sizeof(Dev_Data[idx]));

  prefs.end();
  consoleOut("Dev_Prop and Dev_Data saved for index " + String(idx));
}

// ==========================================
// 2. SMART LOAD FUNCTION
// ==========================================
// Als idx wordt weggelaten (standaard -1), laadt hij alles op bij de boot.
void loadPrefs(int idx) {
  devCount = 0;
  prefs.begin("zigbee_storage", true); // Open in read-only mode

  int start = (idx == -1) ? 0 : idx;
  int end   = (idx == -1) ? 9 : idx + 1;

  for (int i = start; i < end; i++) {
    char propKey[16];
    char dataKey[16];
    sprintf(propKey, "prop_%d", i);
    sprintf(dataKey, "data_%d", i);

    // Load Dev_Prop
    if (prefs.isKey(propKey) && prefs.getBytesLength(propKey) == sizeof(Dev_Prop[i])) {
      prefs.getBytes(propKey, &Dev_Prop[i], sizeof(Dev_Prop[i]));
      //Count only when isActive true
      if (Dev_Prop[i].isActive) { 
        devCount++;
      } else {
        break;
      }
        
    } else if (idx != -1) {
      consoleOut("WARNING: no data for Dev_Prop " + String(i));
    }

    // Laad Dev_Data
    if (prefs.isKey(dataKey) && prefs.getBytesLength(dataKey) == sizeof(Dev_Data[i])) {
      prefs.getBytes(dataKey, &Dev_Data[i], sizeof(Dev_Data[i]));
    } else if (idx != -1) {
      consoleOut("WARNING: no data for Dev_Data " + String(i));
    }
  }

  prefs.end(); // Sluit netjes af
  if (idx == -1) {
    consoleOut("All dev data loaded, devCount =" + String(devCount));
  }
}

void printDeviceProp(int idx) {
  if (idx < 0 || idx >= 9) return;
  
  Serial.printf("\n================= DEV_PROP[%d] =================\n", idx);
  Serial.printf("  Name:         %s\n",    Dev_Prop[idx].devName);
  Serial.printf("  IEEE Adres:   %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\n", 
                Dev_Prop[idx].devIeee[0], Dev_Prop[idx].devIeee[1], Dev_Prop[idx].devIeee[2], Dev_Prop[idx].devIeee[3],
                Dev_Prop[idx].devIeee[4], Dev_Prop[idx].devIeee[5], Dev_Prop[idx].devIeee[6], Dev_Prop[idx].devIeee[7]);
  Serial.printf("  Short Addr:   0x%04X\n", Dev_Prop[idx].shortAddr);
  Serial.printf("  Dev Type:     %d\n",     Dev_Prop[idx].devType);
  Serial.printf("  Dev Idx:      %d\n",     Dev_Prop[idx].devIdx);
  Serial.printf("  Endpoint:     %d\n",     Dev_Prop[idx].endpoint);
  //Serial.printf("  Is Bound:     %s\n",     Dev_Prop[idx].isBound  ? "Y" : "N");
  Serial.printf("  toMqtt:       %s\n",     Dev_Prop[idx].toMqtt ? "Y" : "N");
  //Serial.printf("  Wrote CIE:    %s\n",     Dev_Prop[idx].wroteCie ? "Y" : "N");
  Serial.printf("  is Active:    %s\n",     Dev_Prop[idx].isActive ? "Y" : "N");
  Serial.println("================================================");
}

void printDeviceData(int idx) {
  if (idx < 0 || idx >= 9) return;

  Serial.printf("\n----------------- DEV_DATA[%d] -----------------\n", idx);
  Serial.printf("  Profile ID:   0x%04X\n", Dev_Data[idx].profileId);
  Serial.printf("  Device ID:    0x%04X\n", Dev_Data[idx].deviceId);
  
  // Print Input Clusters
  Serial.printf("  In Clusters (%d): ", Dev_Data[idx].inputClusterCount);
  for(int i = 0; i < Dev_Data[idx].inputClusterCount; i++) {
    Serial.printf("0x%04X ", Dev_Data[idx].inputClusters[i]);
  }
  Serial.println();

  // Print Output Clusters
  Serial.printf("  Out Clusters (%d): ", Dev_Data[idx].outputClusterCount);
  for(int i = 0; i < Dev_Data[idx].outputClusterCount; i++) {
    Serial.printf("0x%04X ", Dev_Data[idx].outputClusters[i]);
  }
  Serial.println();

  // Print Values
  Serial.print("  Values:       [");
  for (int i = 0; i < 4; i++) {
    Serial.printf("%.2f%s", Dev_Data[idx].values[i], (i < 3) ? ", " : "]\n");
  }
  Serial.print("  Preset RGBD:  [");
  for (int i = 0; i < 4; i++) {
    Serial.printf("%u%s", Dev_Data[idx].preset[i], (i < 3) ? ", " : "]\n");
  }
  Serial.println("------------------------------------------------");
}

void printDevices() { 
      if(diagNose == 0 ) return;     
      consoleOut(F(" ****** print all existing device files ******\n"));
      //if we have 4 devices devCount = 4 and last device has devNr3 (0 1 2 3)
      for (int x=0; x < devCount; x++) 
      {
      yield();
      //String bestand = "/Dev_Prop" + String(x) + ".str";
             consoleOut("struct: " + String(x));
              printDeviceProp(x);
              printDeviceData(x);
      }
}

void printLampState(int idx) {
  Serial.printf("\n----------------- lampState[%d] -----------------\n", idx);
  Serial.printf("  onoff:  %s\n", lampState[idx].onoff ? "Y" : "N");
  Serial.printf("  mode:   %d\n", lampState[idx].mode);
  Serial.printf("  temp:    %dX\n", lampState[idx].colorTemp);
  Serial.print("  Current RGBD:  [");
  for (int i = 0; i < 4; i++) {
    Serial.printf("%u%s", lampState[idx].present[i], (i < 3) ? ", " : "]\n");
  }
  //Serial.printf("  red:    %d\n", lampState[idx].present[R]); 
  //Serial.printf("  gre:    %d\n", lampState[idx].present[G]);
  //Serial.printf("  blu:    %d\n", lampState[idx].present[B]); 
  //Serial.printf("  dim:    %d\n", lampState[idx].present[D]);

}