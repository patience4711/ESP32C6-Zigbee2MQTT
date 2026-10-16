// find out if the request comes from inside the network
bool checkRemote(String url) {
//check if the first 9 characters of the router 192.168.0
    if(securityLevel == 0) return false; // never remote
    if ( url.indexOf(WiFi.gatewayIP().toString().substring(0, securityLevel)) == -1 ) return true; else return false;
}

// we come here when an unknown request is done
void handleNotFound(AsyncWebServerRequest *request) 
{
  
   bool intern = false;
   if(!checkRemote( request->client()->remoteIP().toString()) ) intern = true;

  // **************************************************************************
  //             R E S T R I C T E D   A R E A
  // **************************************************************************
  // access only from inside the network
  if ( intern ) {    //DebugPrintln("the request comes from inside the network");
      String serverUrl = request->url().c_str();
      Serial.println("serverUrl = " + serverUrl);             
     
    
     if ( serverUrl.indexOf("SENDTEST") > -1) 
        { 
          strcpy( txBuffer, request->arg("mes").c_str()); 
          diagNose = 10; // consoleOut logs the output 
          debugLog = "debug send test command\n" + String(txBuffer); 
          actionFlag = 35; 
          String term = "sending zigbee message"; 
          if(diagNose == 10) term += "<br><br><a href=\"/readDebug\">view log</a>"; 
          request->send ( 200, "text/html", term ); 
          return; 
        } else
    
 
      // /SHOWFILE?file=/wificonfig.json
       if ( serverUrl.indexOf("SHOWFILES") > -1) 
      { 
      loginBoth(request, "admin"); // we need to be logged in
          diagNose = 3; // consoleOut logs the output 
          String term = "printing all files in SPIFFS\n "; 
          SPIFFS_read();
          //printInverters(); // show all the inverter files
          term += "<br><br><a href=\"/readDebug\">view log</a>"; 
          request->send ( 200, "text/html", term ); 
          return; 
      } else
 
        if ( serverUrl.indexOf("DEVICES") > -1) 
      { 
      loginBoth(request, "admin"); // we need to be logged in
          diagNose = 10; // consoleOut logs the output 
          String term = "printing all device files\n "; 
          printDevices();
          //printInverters(); // show all the inverter files
          if(diagNose == 10) term += "<br><br><a href=\"/readDebug\">view log</a>";
          request->send ( 200, "text/html", term ); 
          if(diagNose == 10) diagNose = 1;
          return; 
      } else
      
    //   if ( serverUrl.indexOf("SETHUE") > -1) 
    //   { 
    //       // command looks like SETHUE?dev=2&hue=10&trans=1
    //       int dev = request->arg("dev").toInt();
    //       int hue = request->arg("hue").toInt();
    //       int trans = request->arg("trans").toInt();
    //       // make sure the bulb is on
    //       bulbOnOff(dev, true, true);
    //       bulbSetHue(dev, hue);
    //       String term="bulb " + String(dev) + " set to hue " + String(hue); 
    //       request->send ( 200, "text/html", term );
    //       return; 
    //   } else    
      
    //    if ( serverUrl.indexOf("SETSAT") > -1) 
    //   { 
    //       // /SETHUE?dev=2&hue=10&trans=1
    //       int dev = request->arg("dev").toInt();
    //       int sat = request->arg("sat").toInt();
    //       int trans = request->arg("trans").toInt();
    //                 // make sure the bulb is on
    //       bulbOnOff(dev, true, true);
    //       bulbSetSat(dev, sat, trans);
    //       String term="bulb " + String(dev) + " set to saturation " + String(sat); 
    //       request->send ( 200, "text/html", term );
    //       return; 
    //   } else     
      
    //      if ( serverUrl.indexOf("BULB") > -1) 
    //   { 
    //       // BULB?dev=2&on=1
    //       int dev = request->arg("dev").toInt();
    //       int state = request->arg("state").toInt();
    //       //int trans = request->arg("trans").toInt();
    //       if (state == 1) bulbOnOff(dev, true, true); else bulbOnOff(dev, false, true);
    //       //bulbSetSat(dev, hue, trans);
    //       String term="bulb " + String(dev) + "on = " + String(state); 
    //       request->send ( 200, "text/html", term );
    //       return; 
    //   } else    
      
      if ( serverUrl.indexOf("TESTMQTT") > -1) 
      {
          if (request->hasParam("debug")) diagNose = 10;
          
          if (diagNose == 10) debugLog = "debug test mosquitto\n<br>";
          actionFlag = 49;
          
          String term = "<center>test message mosquitto\n<br>";
          if (diagNose == 10) term += "<br><br><a href=\"/readDebug\">view debug log</a>";
          request->send ( 200, "text/html", term );
          return;
      } else

      if ( serverUrl.indexOf("readDebug") > -1) {
          debugLog += "\nlength log: " + String(debugLog.length());
          request->send(200, "text/html", "<pre>" + debugLog + "</pre>");
          debugLog="";
          return;
      } //else

     // if we are here, no valid api was found    
     request->send ( 200, "text/plain", "ERROR this link is invalid, go back <--" ); //send not found message
     }             
    else 
     { // not intern
      //DebugPrint("\t\t\t\t unauthorized, not from inside the network : ");
      request->send ( 200, "text/plain", "ERROR you are not authorised, go back <--" );//send not found message
     }
}
