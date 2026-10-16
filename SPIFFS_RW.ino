// ******************   spiffs lezen  *************************

// als er geen spiffs bestand is dan moet hij eigenlijk altijd een ap openenen
void SPIFFS_read() {
  //DebugPrintln("mounting FS...");
 if (SPIFFS.begin(true)) {
    Serial.println("mounted file system");
       if( file_open_for_read("/basisconfig.json") ) {     
             Serial.println("read basisconfig\n");
          } else {
          Serial.println("basisconfig.json not opened\n");
        } 
       if( file_open_for_read("/wificonfig.json") ) {
                Serial.println("read wificonfig\n");
          } else {
             Serial.println("wificonfig.json not opened\n");
          }
       if( file_open_for_read("/mqttconfig.json") ) {     
             Serial.println("mqttconfig read");
          } else {
          Serial.println("mqttconfig.json not opened");
        }         
  } else {
        Serial.println("failed to mount FS");
   }
 // einde spiffs lezen 5 bestanden

}  

// void writeStruct( String whichfile, int nummer) {
//       File configFile = SPIFFS.open(whichfile, "w");
//       //input=/Inv_Prop2.str
//      //int nummer=whichfile.substring(9,10).toInt();
//      //Serial.println("nummer = "+ String(nummer));  
//         if (!configFile)
//            {
//              Serial.print(F("Failed open for write : ")); Serial.println(whichfile);            
//            } 
//               Serial.print(F("Opened for write....")); Serial.println(whichfile);
//               configFile.write( (unsigned char *)&Dev_Prop[nummer], sizeof(Dev_Prop[nummer]) );
//               configFile.close();        
// }

// bool readStruct(String whichfile) {
//       consoleOut("readStruct whichfile = " + whichfile);
//       if (!SPIFFS.exists(whichfile)) {
//          consoleOut("Failed to open for read" + whichfile);
//          return false;           
//       } 
//       // the file exists so we can open it for read
//       File configFile = SPIFFS.open(whichfile, "r");
//       int dev = whichfile.substring(9,10).toInt();

//       consoleOut("readStruct ivn = " + String(dev) );  
//       consoleOut("reading " + whichfile);
//       configFile.read( (unsigned char *)&Dev_Prop[dev], sizeof(Dev_Prop[dev]) );
//       configFile.close();
//       return true;
//  }

// void printStruct( String bestand ) {
// //input String bestand = "/Inv_Prop" + String(x) + ".str";
//       //String bestand = bestand + String(i) + ".str"
//       //leesStruct(bestand); is done at boottime
//       int dev = bestand.substring(9,10).toInt();
//       consoleOut("Dev_Prop[" + String(dev) + "].devName = " + String(Dev_Prop[dev].devName));     
//       consoleOut("Dev_Prop[" + String(dev) + "].devIeee = " + ieeeToHexString(Dev_Prop[dev].devIeee));
//       consoleOut("Dev_Prop[" + String(dev) + "].shortAddr = " + String(Dev_Prop[dev].shortAddr)); 
//       consoleOut("Dev_Prop[" + String(dev) + "].devType = " + String(Dev_Prop[dev].devType));
//       consoleOut("Dev_Prop[" + String(dev) + "].endpoint = " + String(Dev_Prop[dev].endpoint));
//       consoleOut("");
//       consoleOut("****************************************");
// }

// **************************************************************************** 
//                      save the data in SPIFFS                         *  
// ****************************************************************************
void wifiConfigsave() {
   Serial.println("saving config");

    JsonDocument doc;
    JsonObject json = doc.to<JsonObject>();   
    //json["ip"] = static_ip;
    json["pswd"] = pswd;
    json["longi"] = longi;
    json["lati"] = lati;
    
    json["gmtOffset"] = gmtOffset;
    json["zomerTijd"] = zomerTijd;
    json["securityLevel"] = securityLevel;
    File configFile = SPIFFS.open("/wificonfig.json", "w");
    if (!configFile) {
      Serial.println("open file for writing failed!");
    }
    //DebugPrintln("wificonfig.json written");
    //json.printTo(Serial);
    if(diagNose != 0)
    {
    serializeJson(json, Serial);
    Serial.println(F("")); 
    }
    serializeJson(json, configFile);
    configFile.close();
}


void basisConfigsave() {
    Serial.println("saving basis config");
    JsonDocument doc;
    JsonObject json = doc.to<JsonObject>();
    json["PAN_ID"] = PAN_ID;
    json["userPwd"] = userPwd;
    json["diagNose"] = diagNose;
    json["ledSignal"] = ledSignal;
    json["zbDebug"] = zbDebug;
    //json["pollOffset"] = pollOffset;
        
    File configFile = SPIFFS.open("/basisconfig.json", "w");
    if (!configFile) {
      //DebugPrintln("open file for writing failed");
    }
    Serial.println("basisconfig.json written");
    if (diagNose != 0)
    { 
    serializeJson(json, Serial);
    Serial.println(F(""));     
    }
    serializeJson(json, configFile);
    configFile.close();
}

void mqttConfigsave() {
   //DebugPrintln("saving mqtt config");
    JsonDocument doc;
    JsonObject json = doc.to<JsonObject>();
// 
//    json["Mqtt_Enabled"] = Mqtt_Enabled;
    json["Mqtt_Broker"] = Mqtt_Broker;
    json["Mqtt_Port"] = Mqtt_Port;    
    //json["Mqtt_stateIDX"]       = Mqtt_stateIDX;  
    json["Mqtt_inTopic"] = Mqtt_inTopic;
    json["Mqtt_outTopic"] = Mqtt_outTopic;
    json["Mqtt_Username"] = Mqtt_Username;
    json["Mqtt_Password"] = Mqtt_Password;
//    json["Mqtt_Idx"] = Mqtt_Idx;
    json["Mqtt_Format"] = Mqtt_Format;    
    File configFile = SPIFFS.open("/mqttconfig.json", "w");
    if (!configFile) {
      //DebugPrintln("open file for writing failed");
    }
    Serial.println("mqttconfig.json written");
    if(diagNose != 0)
    {
    serializeJson(json, Serial);
    Serial.println(F("")); 
    }
    serializeJson(json, configFile);
    configFile.close();
}


bool file_open_for_read(const char* bestand) 
{
      String Output = "";
      consoleOut("we are in file_open_for_read, file = " + String(bestand)); //DebugPrintln(bestand); 
        JsonDocument doc;
        File configFile = SPIFFS.open(bestand, "r");
        if (configFile) {
        DeserializationError error = deserializeJson(doc, configFile);
        configFile.close();
        if (error) {
            Serial.print(F("Failed to parse config file: "));
            Serial.println(error.c_str());
            // Continue with fallback values
        } else {
        // no error so we can print the file
            //serializeJson(doc, Serial);  // always print
            serializeJson(doc, Output);  // always print
            Serial.println("here we should see the file content");
            Serial.println(Output);
        }
    } else {
        Serial.print(F("Cannot open config file: "));
        Serial.println(bestand);
        // Continue with empty doc -> all fallbacks will be used
    }
    // we read the file even if it doesn't exist, so that variables are initialized
    // we read every variable with a fall back value to prevent crashes

    //serializeJson(doc, jsonStr);
    if (strcmp(bestand, "/wificonfig.json") == 0) {
              //strcpy(static_ip, doc["ip"] | "000.000.000.000");
              strcpy(pswd, doc["pswd"] | "0000");
              longi = doc["longi"] | 5.432;
              lati = doc["lati"] | 51.743;                      
              strcpy(gmtOffset, doc["gmtOffset"] | "+120");
              zomerTijd = doc["zomerTijd"].as<bool>() | true;
              securityLevel = doc["securityLevel"].as<int>() | 6;
    }

    if ( strcmp(bestand, "/basisconfig.json") == 0) {
            strcpy (PAN_ID, doc["PAN_ID"] | "D8A3");
            strcpy (userPwd, doc["userPwd"] | "1111" );
            diagNose = doc["diagNose"].as<int>() | 2; // default is serial
            ledSignal = doc["ledSignal"] | false;
            zbDebug = doc["zbDebug"] | false;
            //Polling = doc["Polling"].as<bool>() | false;
      }            

    if ( strcmp(bestand, "/mqttconfig.json") == 0) {
              strcpy(Mqtt_Broker,   doc["Mqtt_Broker"]   | "192.168.0.100");
              strcpy(Mqtt_Port,     doc["Mqtt_Port"]     | "1883");  
              strcpy(Mqtt_outTopic, doc["Mqtt_outTopic"] | "domoticz/in"); 
              strcpy(Mqtt_inTopic,  doc["Mqtt_inTopic"]  | "domoticz/out");        
              strcpy(Mqtt_Username, doc["Mqtt_Username"] | "n/a");
              strcpy(Mqtt_Password, doc["Mqtt_Password"] | "n/a");
              Mqtt_Format = doc["Mqtt_Format"].as<int>() | 0;
              //Mqtt_stateIDX = doc["Mqtt_stateIDX"].as<int>() | 123;      
    }
      return true;
} 



