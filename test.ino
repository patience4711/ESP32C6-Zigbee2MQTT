


// void rawMessage()
//       char sendCmd[100]={0};
//       char reCeived[254]={0};
//       int len;
//       len = strlen( txBuffer );

//       //put all the bytes of txBuffer) in sendCmd, starting at pos 7
//       for(int i=0; i<len; i++) 
//       {
//          sendCmd[i] = txBuffer[i]; 
//       }
//       consoleOut("the message = " + String(sendCmd)); 
//        //now we send this command
//        sendRaw(sendCmd); // this adds the
//        // find the answer
       
//       decodeGeneralAnswer();
      
//       delayMicroseconds(250);
       
//        // cleanup
//        memset(&sendCmd, 0, sizeof(sendCmd)); //zero out 
//        delayMicroseconds(250);
// }

// void sendRaw( char printString[] )
// {
//     char bufferSend[254]={0};
//     char byteSend[3]; // never more than 2 bytes 

//     //strcpy(bufferSend, printString); // 

//     empty_serial2();
//     //if (Serial2.availableForWrite() > (uint8_t)strlen(bufferSend))
//     if (Serial2.availableForWrite() > (uint8_t)strlen(printString))
//     {
//         //Serial2.write(0xFE); //we have to send "FE" at start of each command
//         for (uint8_t i = 0; i <= strlen(printString) / 2 - 1; i++)
//         {
//          // we use 2 characters to make a byte
//             strncpy(byteSend, printString + i * 2, 2); 
//             delayMicroseconds(250);                     //

//             Serial2.write(StrToHex(byteSend));        //turn the two chars to a byte and send this
//         }
            
//             Serial2.flush(); //wait till the full command was sent
            
//     }
    
//     consoleOut("\n sendRaw ZB sent " + String(printString));
//     //else if (diagNose == 2) ws.textAll("sendZB FE" + String(bufferSend));
// }

//bool readZBFF(char *inMess) 
// {
//     unsigned long timeoutMs = 200;
//     unsigned long start = millis();
//     int index = 0;
//     int frameLen = -1;   // will store LEN byte
//     bool feFound = false;

//     inMess[0] = '\0';    // clear buffer

//     while (millis() - start < timeoutMs) 
//     {
//         if (Serial2.available()) 
//         {
//             uint8_t byte = Serial2.read();

//             if (!feFound) 
//             {
//                 if (byte == 0xFE) 
//                 {
//                     feFound = true;
//                     index = 0;
//                     strcat(inMess, "FE");
//                 }
//                 continue;
//             }

//             // Convert byte to hex and append
//             char buf[3];
//             sprintf(buf, "%02X", byte);
//             strcat(inMess, buf);
//             index++;

//             // LEN byte is second byte after FE
//             if (index == 1) 
//             {
//                 frameLen = byte;  // payload length
//                 if (frameLen + 2 > CC2530_MAX_SERIAL_BUFFER_SIZE/2) 
//                 {
//                     // sanity check
//                     consoleOut("Frame too long, discarding");
//                     return false;
//                 }
//             }

//             // Check if full frame received: LEN + CMD0/CMD1 + FCS
//             if (frameLen >= 0 && index >= frameLen + 2) 
//             {
//                 return true; // full frame captured
//             }
//         }
//     }

//     // timeout
//     return false;
// }



// we can send a zigbee message via the API
void testMessage() 
{
      
      consoleOut("running testMessage");
      char sendCmd[100]={0};
     
    //  if(console) diagNose = 1; else diagNose = 2;
      int len;
      len = strlen( txBuffer );  //else len = strlen( InputBuffer_Serial );

      consoleOut("the zigbee message = " + String(txBuffer)); 
       //now we send this command
       //sendZB(txBuffer);
       // find the answer
       
      //processAll();

      delayMicroseconds(250);
       
       // cleanup
       memset(&txBuffer, 0, sizeof(txBuffer)); //zero out 
       delayMicroseconds(250);
}



// ******************************************************************



void testCoordinator()
{
char testCmd[30];
char s_d[254]={0}; // provide a buffer for the call to readZB
    strcpy(testCmd, "26058300631A");
    consoleOut("the cmd = " + String(testCmd));
    //ZBhardReset();
    delay(500);
    //sendZB( testCmd );
    my_wifiConnect.ledBlink(0,0,150,1,50);

    //check if anything was received
    //readZB(s_d); // we read but flush the answer
    //readZB(s_d); // we read but flush the answer
}
