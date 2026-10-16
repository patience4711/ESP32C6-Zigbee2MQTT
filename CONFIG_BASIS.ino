const char BASISCONFIG[] PROGMEM = R"=====(
<body>
<div id='msect'>
<div id='menu'>
    <a href="/MENU" class='close'>&times;</a>
    <a href="#" id="sub" style='background:green; display: none' onclick='submitFunction()'>save</a><br>
</div>

<kop>GENERAL SETTINGS</kop>

<div class='divstijl' style='height:56vh;'>
  <form id='formulier' method='get' action='submitform' oninput='showSubmit()'>
  <center><table>
  <tr><td style='width:140px;'>pan id<td><input class='inp6' name='panid' value='{id}' minlength='12' maxlength='12' required></input><td></tr>
  <tr><td>user passwd<td><input  class='inp5' name='pw1' length='11' placeholder='max. 10 char' value='{pw1}' pattern='.{4,10}' title='between 4 en 10 characters'></input> 
  <tr><td>serial debug:<td><input type='checkbox' style='width:30px; height:30px;' name='deBug' #checkA></tr>
  <tr><td>led signals:<td><input type='checkbox' style='width:30px; height:30px;' name='ledSig' #checkB></tr>
  <tr><td>zigbee debug:<td><input type='checkbox' style='width:30px; height:30px;' name='zbD' #checkC></tr>
  </td></tr></table></form>

  </div><br>
</div>
</body></html>
)=====";

void zendPageBasis(AsyncWebServerRequest *request) {
  String(webPage)="";
    //DebugPrintln("we zijn nu op zendPageBasis");
    webPage = FPSTR(HTML_HEAD);
    webPage += FPSTR(BASISCONFIG);
    
    // replace data
    webPage.replace("'{id}'" , "'" + String(PAN_ID) + "'") ;
    webPage.replace( "'{pw1}'" , "'" + String(userPwd) + "'") ;
     Serial.println("diagNose = " + String(diagNose));
     if (diagNose != 0) { 
      webPage.replace("#checkA", "checked");
    } 
    
    if(ledSignal) webPage.replace("#checkB", "checked");
    if(zbDebug) webPage.replace("#checkC", "checked");
    request->send(200, "text/html", webPage);
    webPage=""; // free up
}


// void handleBasisconfig(AsyncWebServerRequest *request) { // form action = handleConfigsave
// // verzamelen van de serverargumenten   
//    strcpy(PAN_ID, request->arg("panid").c_str());
//    strcpy(userPwd, request->arg("pw1").c_str());
// //   pollRes = request->arg("pr").toInt();
// //   hc_IDX = request->arg("hcidx").toInt();
  
// // this value gets currupted when it is negative, we get 256 -/- the number
// // so -2 becomes 254
// //   if (po > 200) { pollOffset = po - 256; } else { pollOffset = po; } 
   
   
// //   calliBration = request->arg("cali").toFloat();
// //BEWARE CHECKBOX
// //if(request->hasParam("pL")) Polling = true;  else  Polling = false;
//   //toSend = FPSTR(CONFIRM);
//   basisConfigsave();  // alles opslaan
//  // request->send_P(200, "text/html", CONFIRM); //send the html code to the client
 
//   //DebugPrintln("basisconfig saved");
//   actionFlag=25; // recalculates the time with these new values 
// }
