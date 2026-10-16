void handle_Serial () 
{
      //DebugPrintln(F("we are in handle serial"));  
      int SerialInByteCounter = 0;
      //InputBuffer_Serial[0] = '\0'; // make it like empty
      byte SerialInByte;  
      memset(InputBuffer_Serial, 0, sizeof(InputBuffer_Serial)); 
      delay(200); // wait untill maybe more data available
      Serial.println("serial data available: " + String(Serial.available()) );
      while(Serial.available()) 
      {
            SerialInByte = Serial.read(); 
            
            // Strikte grens: maximaal 99 tekens in een 100-byte buffer (plek vrijhouden voor \0)
            if (SerialInByteCounter < 49) {
                InputBuffer_Serial[SerialInByteCounter++]=SerialInByte;
            }    
            
            // Zowel Newline (\n) als Carriage Return (\r) sluiten de invoer correct af
            if (SerialInByte == '\n' || SerialInByte == '\r') {                                              
                break; // Seriële data is compleet
            }
      }
     InputBuffer_Serial[SerialInByteCounter] = '\0';
     Serial.print("InputBuffer_Serial = ");
     Serial.println(InputBuffer_Serial);
     
     diagNose = 2; // direct the output to serial
     delay(100);
     Serial.println("\nType 10;HELP to list available commands");
     if (strlen(InputBuffer_Serial) > 6) 
     {                                // need to see minimal 8 characters on the serial port
        if (strncmp (InputBuffer_Serial,"10;",3) == 0) 
        {                 // Command from Master to RFLink
  
          if (strncasecmp(InputBuffer_Serial+3,"HELP",4) == 0) {
              scroll(4);
              Serial.println(F("*** AVAILABLE COMMANDS ***"));
              Serial.println(F("10;DIAG=x; (if x = 1 set diagNose for serial debug)"));
              Serial.println(F("10;DEVICES; (list devices in prefs")); 
              Serial.println(F("10;DEVICE=x; (list devices x in prefs"));
              Serial.println(F("10;STARTZB; (start zb coordinator)"));                           
             // Serial.println(F("10;ZBT=; (send zigbee message, e.g. 10;zbt=2101 (ping))"));
              Serial.println(F("10;DELETE=<file>; (delete a file)"));              
              Serial.println(F("10;HEALTH; (perform healthcheck zigbee)"));             
              Serial.println(F("10;SET_OPS; (set normalOps 0 1 5 7 9=unfiltered)"));
              Serial.println(F("10;SHOWVARS; (print some variables)"));
              return;
            } else 
              
 

          if (strncasecmp(InputBuffer_Serial+3,"DEVICES",7) == 0) {  
              printDevices();
              return;           
          } else

          if (strncasecmp(InputBuffer_Serial+3,"STARTZB",7) == 0) {  
              startCoordinator();
              return;           
          } else
           if (strncasecmp(InputBuffer_Serial+3,"HEALTH",6) == 0) {  
              Serial.println(F("\n\nchecking the ZB system"));
              healthCheck();
              return;             
          } else
  
          if (strncasecmp(InputBuffer_Serial+3,"SPIFFS",5) == 0) {  
              Serial.println("printing SPIFFS");
              File root = SPIFFS.open("/");
              File file = root.openNextFile();
              uint16_t counter = 0;
              while (file) 
              {
                Serial.print("FILE: ");
                Serial.print(file.name());
                Serial.print(" | SIZE: ");
                Serial.println(file.size());
                file.close();          // REQUIRED
                yield();
                counter++;
                if (counter > 10) {
                  Serial.println("BREAK");
                  break;
                }
                file = root.openNextFile();
              }
              root.close(); 
             return;  
                        
          } else 

            if (strncasecmp(InputBuffer_Serial + 3, "UNJOIN=", 7) == 0) {

              int deviceNr = atoi(InputBuffer_Serial + 10);
              if (deviceNr >= 0 && deviceNr < 9) {

                  Serial.println("unjoin device " + String(deviceNr));
                  unjoinDevice(deviceNr);
              } else {
                  Serial.println("invalid device nr!");
              }
              return;
            } else  
            
            if (strncasecmp(InputBuffer_Serial + 3, "RGB_ON=", 7) == 0) {
                int deviceNr = atoi(InputBuffer_Serial + 10);
                Serial.println("ON command for device" + String(deviceNr));
                if (deviceNr >= 0 && deviceNr < 9) {
                    devChoice = deviceNr;
                    bulbOn(deviceNr);
                }

                return;
            } else
                if (strncasecmp(InputBuffer_Serial + 3, "RGB_OFF=", 8) == 0) {
                int deviceNr = atoi(InputBuffer_Serial + 11);
                Serial.println("OFF command for device" + String(deviceNr));
                if (deviceNr >= 0 && deviceNr < 9) {
                    devChoice = deviceNr;
                    bulbOff(devChoice);
                }

                return;
            } else          
           if (strncasecmp(InputBuffer_Serial + 3, "DEV_ON=", 7) == 0) {
                int deviceNr = atoi(InputBuffer_Serial + 10);

                if (deviceNr >= 0 && deviceNr < 9) {
                    switchOn(deviceNr
                        //Dev_Prop[deviceNr].shortAddr,
                        //Dev_Prop[deviceNr].endpoint,
                        //true
                    );
                }

                return;
            } else
                if (strncasecmp(InputBuffer_Serial + 3, "DEV_OFF=", 8) == 0) {
                int deviceNr = atoi(InputBuffer_Serial + 11);

                if (deviceNr >= 0 && deviceNr < 9) {
                    switchOff(
                        deviceNr
                        //Dev_Prop[deviceNr].shortAddr,
                        //Dev_Prop[deviceNr].endpoint,
                        //false
                    );
                }

                return;
            } else


           if (strncasecmp(InputBuffer_Serial + 3,"DEVICE=",7) == 0) {
              int deviceNr = atoi(InputBuffer_Serial + 10);

              printDeviceProp(deviceNr);
              printDeviceData(deviceNr);
              printLampState(deviceNr);
              
              return;             
          } else
  
         
          if (strncasecmp(InputBuffer_Serial + 3, "DISCOVER=", 9) == 0) {

              int deviceNr = atoi(InputBuffer_Serial + 12);
              if (deviceNr >= 0 && deviceNr < 9) {

                  Serial.print("shortAddr = 0x");
                  Serial.println(Dev_Prop[deviceNr].shortAddr, HEX);
                  discoverDevice(Dev_Prop[deviceNr].shortAddr);

              } else {
                  Serial.println("invalid device nr!");
              }

              return;
            
          } else
           if (strncasecmp(InputBuffer_Serial+3,"DUMPVARS",8) == 0) {  
              Serial.println(F("\n\nprint some variables"));
              Serial.println("diagNose = " + String(diagNose));
              Serial.println("devChoice = " + String(devChoice));
              Serial.println("devCount = " + String(devCount));
              //Serial.println("interCept = " + String(interCept));
              Serial.println("joinWithbind = " + String(joinWithBind));
              return; 
           } //else
    
          //{
          // if we are here no command was recongized
          Serial.print( String(InputBuffer_Serial)); Serial.println(F(" INVALID COMMAND" ));     
          
        } // end if if (strncmp (InputBuffer_Serial,"10;",3) == 0)
    
      } //  end if strlen(InputBuffer_Serial) > 6
    Serial.print(String(InputBuffer_Serial)); Serial.println(F(" UNKNOWN COMMAND"));
    // the buffercontent is not making sense so we empty the buffer
    empty_serial();
} // 

  void scroll(int aantal) {
    for (int x=0;x<aantal; x++) { Serial.println(F("\n")); }
  }
 