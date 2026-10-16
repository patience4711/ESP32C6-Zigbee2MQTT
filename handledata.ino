void handleDataRequests(AsyncWebServerRequest *request) 

{
      //consoleOut("handleDataRequest");
      consoleOut("handleDataRequest the request is " + String(requestUrl));
      if( request->hasArg("devnr") )
        {
  
         int i = atoi(request->arg("devnr").c_str()) ;
         AsyncResponseStream *response = request->beginResponseStream("application/json");
         JsonDocument root;
         //we need some properties like device type, name
         String hexAddr = String(Dev_Prop[i].shortAddr, HEX);
         hexAddr.toUpperCase(); // Maakt er bijv. "123A" van i.p.v. "123a"
         root["adr"] = "0x" + hexAddr; // put the address
         root["name"] = String(Dev_Prop[i].devName); // put the name
         root["type"] = String(Dev_Prop[i].devType); // put the type
         root["control"] = 0;
         root["read0"] = Dev_Data[i].values[0];
         root["read1"] = Dev_Data[i].values[1];
         root["read2"] = Dev_Data[i].values[2];
         root["read3"] = Dev_Data[i].values[3];
         //root["control"] = Dev_Prop[i].devType;
         
         //if(Dev_Prop[i].devType == 3) root["control"] = 3;  
         //if(Dev_Prop[i].devType == 4) root["control"] = 4; 
          serializeJson(root, * response);
          request->send(response);
          return; 
        } else

      if( request->hasArg("General") )
    { 
      //consoleOut("found arg General");
      char temp[15]={0};
      uint8_t remote = 0;
      if(checkRemote( request->client()->remoteIP().toString()) ) remote = 1; // for the menu link
      
      AsyncResponseStream *response = request->beginResponseStream("application/json");
      JsonDocument root; //(160);
      //JsonObject root = doc.to<JsonObject>();
      root["cnt"] = devCount;    
      root["rm"] = remote;
      root["st"] = zigbeeUp;    
      serializeJson(root, * response);
      request->send(response);
      return;
    } 
  
    if( request->hasArg("Slider") )
    { 
      consoleOut("found arg Slider");
      consoleOut("devChoice = " + String(devChoice));
      char temp[15]={0};
      uint8_t remote = 0;
      if(checkRemote( request->client()->remoteIP().toString()) ) remote = 1; // for the menu link
      //int devNr = atoi(request->arg("Slider").c_str()) ;
      AsyncResponseStream *response = request->beginResponseStream("application/json");
      JsonDocument root; 
      root["devnr"] = devChoice;
      root["red"] = lampState[devChoice].present[R];  // to set the slider  
      root["gre"] = lampState[devChoice].present[G]; // to set the slider
      root["blu"] = lampState[devChoice].present[B]; // to set the slider
      root["dim"] = lampState[devChoice].present[D];  //to set the slider  
      root["pre1"] = Dev_Data[devChoice].preset[R];  // to set the slider  
      root["pre2"] = Dev_Data[devChoice].preset[G]; // to set the slider
      root["pre3"] = Dev_Data[devChoice].preset[B]; // to set the slider
      root["mode"] = lampState[devChoice].mode; //to set which button
      root["onoff"] = lampState[devChoice].onoff; //to set which button
      serializeJson(root, * response);
      request->send(response);
      return;
    } 
    if( request->hasArg("Switch") )
    { 
      consoleOut("found arg Switch");
      consoleOut("devChoice = " + String(devChoice));
      char temp[15]={0};
      uint8_t remote = 0;
      if(checkRemote( request->client()->remoteIP().toString()) ) remote = 1; // for the menu link
      //int devNr = atoi(request->arg("Slider").c_str()) ;
      AsyncResponseStream *response = request->beginResponseStream("application/json");
      JsonDocument root;
      root["devnr"] = devChoice; 
      root["onoff"] = lampState[devChoice].onoff; //1 or 0
      serializeJson(root, * response);
      request->send(response);
      return;
    } 

    // if we are here no maching request was found
       String term = "invalid request";
       request->send(200, "text/plain", term);
}    
