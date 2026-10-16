const char DEVCONFIG_START[] PROGMEM = R"=====(
<!DOCTYPE html><html><head><meta charset='utf-8'>
<title>ESP-Z2M</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="icon" type="image/x-icon" href="/favicon.ico" />
<link rel="stylesheet" type="text/css" href="/STYLESHEET">

<style>
.cap {
  font-weight:bold; 
  Background-color:lightgreen;
 }
div.overlay {
  display: block;
  width: 100%;
  height: 100%;
  background-color: rgba(0,0,0,0.7);
  z-index: 0;
  text-align: center;
  vertical-align: middle;
  line-height: 300px;
}
</style>
<script>
function showSubmit() {
document.getElementById("sub").style.display = "inline-block";
}

function submitFunction(a) {
document.getElementById('formulier').submit();
}

function cl() {
window.location.href='/MENU';
}
function delFunction(a) {
  if(confirm("are you sure to delete this device ?")) {  
  document.getElementById('formular').submit();   
  }
}

window.addEventListener('pageshow', function(event) {
    // Als event.persisted waar is, komt de pagina uit de browser-cache (back-button)
    if (event.persisted) {
        window.location.reload();
    }
});
</script>
</head>
<body onload='%LOADBAG%'>

<div id='msect'><div id='bo'><div>
<div id='menu' style='height:96px' >
<a href='/MENU' class='close'>&times;</span>
<a href='/DEV?welke=0' style='display:%none'0%>dev. 0</a>
<a href='/DEV?welke=1' style='display:%none'1%>dev. 1</a>
<a href='/DEV?welke=2' style='display:%none'2%>dev. 2</a>
<a href='/DEV?welke=3' style='display:%none'3%>dev. 3</a>
<a href='/DEV?welke=4' style='display:%none'4%>dev. 4</a>
<a href='/DEV?welke=5' style='display:%none'5%>dev. 5</a>
<a href='/DEV?welke=6' style='display:%none'6%>dev. 6</a>
<a href='/DEV?welke=7' style='display:%none'7%>dev. 7</a>
<a href='/DEV?welke=8' style='display:%none'8%>dev. 8</a>
<a href='/DEV?welke=99' style='color:#66ff33; display:%none'99%>add</a>
</div>
<center><div class='divstijl' style='height:70vh;'><center>
<form id='formulier' method='get' action='deviceconfig' oninput='showSubmit()' onsubmit="return confirm('sure to save this device?')">
    %<FORMPAGE>%
    <br>
  </div>
  </center>
  <form id='formular' method='get' action='/DEV_DEL'></form>
  <div id='menu'>
    <div id='pairknop' style='display:%none'p% >
    <a class='groen' href='/JOIN' onclick="return confirm('Are you sure you want to join this device?')">join</a>
    <a href='#' onclick='delFunction("/SW=BACK")'>delete</a>
    </div> 
    <a href='#' onclick='helpfunctie()'>help</a>
    <a href="#" id="sub" style='background:green; display: none' onclick='submitFunction("/sw=BACK")'>save</a><br>
</div>
<br>
</div>

</body></html>
 )=====";

 const char DEVICE_GENERAL[] PROGMEM = R"=====(
<div id='inverter0' style='display:block'>
    <table><tr><td style='width:160px;'><h4>DEVICE {nr}</h4></tr>
    </table>
        
    <br>
    <table style="background-color: lightgreen; padding:10px;">
    <tr><td class="cap" >NAME<td class="cap" ><input class='inp4' id='in' name='in' maxlength='12' value='{name}'></input>
    <tr><td class="cap">IDX<td class="cap" ><input class='inp2' name='mqidx' value='{idx}' size='4' length='4' readonly></input></td></tr>
    <tr><td class="cap" style="width:100px;">IEEE NR<td><input class='inp4' id='ie' name='ie' value='{0000000000000000}' readonly></input>
    <tr><td class="cap">SHORTADDR<td><input class='inp3' id='ia' name='ia' value='{0000}' readonly></input>
    <tr><td class="cap">ENDPOINT<td><input class='inp3' id='ep' name='ep' value='{EP}' readonly></input>
    <tr><td class="cap">TYPE<td><select name='devt' class='sb1' id='sel' onchange='myFunction()'>
    <option value='0' devtype_0>0 NOT DEFINED</option>
    <option value='1' devtype_1>1 TEMP/HUM-SENSOR</option>
    <option value='2' devtype_2>2 MOTION-SENSOR</option>
    <option value='3' devtype_3>3 SMART-BULB</option>
    <option value='4' devtype_4>4 SMART-SOCKET</option>
    </select>
    </tr>
    
    </tr></td>

</table></form></div>
)=====";


// **********************************************************************************
//                         script
// **********************************************************************************

// const char DEV_SCRIPT[] PROGMEM = R"=====(
// //var myLink = 
// function delFunction(a) {
//   if(confirm("are you sure to delete this device ?")) {  
//   document.getElementById('formular').submit();   
//   }
// }

// )====="; 




//*******************************************************************************************
//             prepare for saving the data
// *****************************************************************************************
void handleDeviceconfig(AsyncWebServerRequest *request) 
{ 
      // form action = handleInverterconfig
      // we only collect the data for this specific inverter
      // collect the serverarguments
      //strcpy(Dev_Prop[devChoice].devAdr, request->arg("ia").c_str());
      
      strcpy(Dev_Prop[devChoice].devName, request->arg("in").c_str());
      //strcpy(Dev_Prop[devChoice].devIeee, request->arg("ie").c_str());
      
      Dev_Prop[devChoice].devType = request->arg("devt").toInt(); //values are  0 1 2 3 4 
      Dev_Prop[devChoice].devIdx = request->arg("mqidx").toInt(); //values are 0 1  
      Dev_Prop[devChoice].isActive = true; //values are 0 1 
      
      actionFlag = 30 + devChoice; // this means save
      if(devChoice == devCount) 
      {
        devCount += 1;
        consoleOut("we appended a device, devCount now : " + String(devCount));
        // if we append a device, all properties should be set to NULL
        memset(Dev_Prop[devChoice].devIeee, 0, sizeof(esp_zb_ieee_addr_t));
        Dev_Prop[devChoice].shortAddr = 0;
        Dev_Prop[devChoice].endpoint = 0; 
      }
      
      String toReturn = "/DEV?welke=" + String(devChoice);
      strcpy(requestUrl, toReturn.c_str() ); 
      consoleOut("requestUrl = " + String(requestUrl));
      consoleOut("\ndevCount after edit (saved) = " + String(devCount));  
      //consoleOut("list of the files we have after edit");
      //printDevices();
      confirm();
      request->send(200, "text/html", toSend);
}

//*******************************************************************************************
//             delete a device
// *****************************************************************************************
void handleDevicedel(AsyncWebServerRequest *request, uint8_t welke) 
{ 
  // form action = handleInverterconfig
  // we only collect the data for this specific inverter
  // read the serverargs and copy the values into the variables
    consoleOut("whichDevice = " + String(welke));
    consoleOut("deviceDel devChoice = " + String(devChoice));
    actionFlag = 20 + devChoice; // this means devicedelete   
    procesId = 3;
    String toReturn = "/DEV?welke=0";
    strcpy(requestUrl, toReturn.c_str() ); 
    confirm();
    request->send(200, "text/html", toSend);
}



// ********************************************************************
//                     processor
// *********************************************************************
String processor(const String& var)
{
//
  // if(var == "LOADBAG") 
  // {
  //   consoleOut(F("found LOADBAG"));
  //   if(Dev_Prop[iKeuze].devType == 1) 
  //     {
  //     return F("showFunction()"); 
  //     } else {
  //     return F("hideFunction()");  
  //     }
  // }
// make the menu items visible --> works
  for(int x=0; x<9; x++) { // for every button we have to set the visibility
     String placeholder = "none'" + String(x);
     //Serial.println("placeholder = " + placeholder);
       if(var == "none'" + String(x) ) { 
        if (x < devCount) { return F("inline-block'"); } else { return F("none'"); }
       }
  }
//   
   if(devCount < 9) {
    // show the add button 
    if(var == "none'99") return F("inline-block'");   
   } else {
    if(var == "none'99") return F("none'");
   }
  
  if(var == "<FORMPAGE>"){
    consoleOut(F("found FORMPAGE"));
    return(toSend);  
  }

  if(var == "none'p") {
    //String bestand = "/Dev_Prop" + String(devChoice) + ".str";
    if(structExists(devChoice)) return "inline-block'"; else return "none'";
  }

return String(); //return empty when no match
}


// construct the form and write in a file toSend
void deviceForm() 
{
    int verklikker = 0;
    if (devCount >= 88 ) // if we add this = 99
    { 
        verklikker = 88;
        devCount -= verklikker; // restore the original inverterCount
    }
    devCount += verklikker; // add 88 again
    // now we have 3 situations
    // devCount == 0, show the page currently no devices
    // dKeuze < devcount, we have an existing device
    // dKeuze == devcount, we are adding a new device
    // if we clicked the add button then devcount is at least 88
    if( devCount != 0 ) 
    {
   
        // **********************************************************************
        //        construct the devicepage with actual data
        // **********************************************************************
        if (devCount >= 88 ) devCount -= 88; // restore inverterCount
        toSend = FPSTR(DEVICE_GENERAL);  
        // is there a file iKeuze then
        //String bestand = "/Dev_Prop" + String(devChoice) + ".str";
        if(structExists(devChoice)) 
        {
            consoleOut("deviceForm struct exists ");
            //the file exists so we can display the values 
            toSend.replace("{nr}" , String(devChoice)); // vervang inverter nummer not available
            toSend.replace("{0000000000000000}", ieeeToHexString(Dev_Prop[devChoice].devIeee));
            toSend.replace("{name}", String(Dev_Prop[devChoice].devName));
            toSend.replace("{idx}", String(Dev_Prop[devChoice].devIdx));
            String hexAddr = String(Dev_Prop[devChoice].shortAddr, HEX);
            hexAddr.toUpperCase(); // Maakt er bijv. "123A" van i.p.v. "123a"
            toSend.replace("{0000}", "0x" + hexAddr);
            toSend.replace("{EP}", String(Dev_Prop[devChoice].endpoint));
            

            // put back the select
            switch (Dev_Prop[devChoice].devType)
            {
               case 1:
                  toSend.replace("devtype_1", "selected");
                  break;
               case 2:
                  toSend.replace("devtype_2", "selected");
                  break;
               case 3:
                  toSend.replace("devtype_3", "selected");
                  break;
               case 4:
                  toSend.replace("devtype_4", "selected");
                  break;
            }

        } else {
        // the file does not exist so we show an empty page
        consoleOut("File does not exist");
        toSend.replace("devtype_0", "selected");
        toSend.replace("{0000000000000000}", "0000000000000000");
        toSend.replace("{name}", "");
        toSend.replace("{idx}", "0");
        toSend.replace("{0000}", "0000");
        }

    } else { // so if devCount == 0 we present this page
     toSend = "<br><br><br><h3>currently no devices</h3>"; 
    }
// now we have toSend ready to include in the inverterpage
}


void deviceDelete(uint8_t devNr)
{
   //String bestand = "/Dev_Prop" + String(devNr) + ".str"; // /Inv_Prop0.str
   Serial.println("remove struct " + String(devNr)); 
 
   yield();
 
   // if we have 5 devices, devCount = 5 and the last device = nr 4
   // when we remove the last device we dont need to remove_gaps
   // we ondly decreese the devCount
   // say we removed dev 2 out of 4 devices, file remaining 0 1 3
   // deviceCount was 4 and lowered to 3
   // so when the devicnr was 2 (not deviceCount) then we need to remove the gaps
   // when the devicenr was 3 
   //
   // mark the device for deletion
   
   Dev_Prop[devNr].isActive= false;
   //when devCount = 4 then the last struct = 3
   if(devNr != devCount-1) {
       Serial.println("device is not the last, need to remove gaps");
       remove_gaps();
   } else {
       // the deleted device is the last one nr devCount-1
       devCount -= 1; 
   }

    Serial.println("devCount after removal = " + String(devCount));
    yield();
    Serial.println("list of the files we have after removed one");
    printDevices();
} 

// remove the gaps
void remove_gaps() {
  //String bestand_1;
  //String bestand_2;
  //bool found = false;  

  // say we have Inv_Prop0, Inv_Prop1, Inv_Prop3, Inv_Prop4, Inv_Prop5
  // this are 5 files 
  // there can only a gap of 1 inverter(can only remove 1 at a time) 
  // if we know the devCount we can search for a gap and put the last file in it
  // so if we are missing Inv_Prop1, we know that we have Inv_prop5 
  // say we have 3 devs and the dev[1] is marked for delete 
  // structExists = false then
  consoleOut("remove gaps, devCount = " + String(devCount));
  for(int i=0; i < devCount; i++ ) { // 0 1 2 3 4
  //
  // if this file not exixts we know that there must be a file "?inv_Prop inverterCount.str
  if( !structExists(i) ) {
   
      consoleOut("found a gap at nr " + String(i) );  
      yield();
      //bestand_2 = "/Dev_Prop" + String(devCount) + ".str"; // the last file
      //if( !structExists(devCount)) consoleOut("error not exists struct " + String(devCount));  
      // if we rename the last file to the gap, it keeps the old content
      // so we just copy the struct and write that to spiffs
      consoleOut("copy the last struct to struct " + String(i));
      //if we had 3 structs, the last one is struct[2]
      Dev_Prop[i] = Dev_Prop[devCount-1]; // copie the entyre struct
      Dev_Data[i] = Dev_Data[devCount-1];
      Dev_Prop[devCount-1].isActive = false;
      savePrefs(i); // write the copied struct
      devCount -= 1;
      return;  
      }
  }
  consoleOut("no gaps found");
} 