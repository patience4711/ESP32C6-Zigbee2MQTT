//<link rel="icon" type="image/x-icon" href="/favicon.ico" />

const char CONSOLE_HTML[] PROGMEM = R"=====(
<!DOCTYPE html><html><head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
<title>ESP32-Z2M</title>
<meta name="viewport" content="width=device-width, initial-scale=1">

<link rel="stylesheet" type="text/css" href="/STYLESHEET">
<script>
function helpfunctie() {
document.getElementById("hulp").style.display = "block";
}
function sl() {  
document.getElementById("hulp").style.display = "none";
}

</script>

<style>
 tr {height:16px !important;
 font-size:15px !important;
 } 
 li a:hover {
   background-color: #333 !important;
}
#hulp {
  background-color: #ffffff; 
  border: solid 2px; 
  display:none; 
  padding:4px;
  width:94vw;
}
.divstl { width: 60vw; height:84vh; background: #dbd89c; border:1px solid; padding-left:10px;
}
@media only screen and (max-width: 800px) {
  .divstl { width:98%; }
  }


</style>
</head>
<body>
  <div id='hulp'>
  <span class='close' onclick='sl();'>&times;</span><h3>CONSOLE COMMANDS</h3>
  <b>10;DELETE=filename: </b> delete a file.<br><br>
  <b>10;JOIN-DEVICE=x: </b> join a new device<br><br>
  <b>10;INTERVIEW=x: </b> interview a device<br><br>
  <b>10;HEALTH: </b> healthcheck zigbee hw/system<br><br>
  <b>10;INIT_N: </b> start the zigbee coordinator<br><br>
  <b>10;DIAG: </b> change debug, 0=disable, 1=console, 2=serial<br><br>
  <b>10;CLEAR: </b> clear console window<br><br> 
  <b>10;SET_OPS=: </b> set normalOps value 9=all<br><br> 
  </div>

<div id='msect'>
<div id='menu'>
<a href='/MENU' onclick='confirmExit()' class='close'>&times;</span></a>
<a href='#' onclick='helpfunctie()'>help</a>
<a><input type="text" placeholder="type here" id="tiep"></a>
</div>  
<br>  
  <div class='divstl'>
  <table id='tekstveld'></table>
  </div>
 </div>

<script>
  var field = document.getElementById('tekstveld');
  var gateway = `${(window.location.protocol == "https:"?"wss":"ws")}://${window.location.hostname}/ws`;
  var websocket;
  var inputField = document.getElementById('tiep');

  window.onbeforeunload = confirmExit;
  function confirmExit()
  {
      alert("close the console?");
      ws.close();  
  }  
  
  window.addEventListener('load', onLoad);
  function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage; // <-- add this line
  }
  function onOpen(event) {
    console.log('Connection opened');
    field.insertAdjacentHTML('beforeend', "<tr><td>* * connection opened * *");
    inputField.focus();
    }
  function onClose(event) {
    console.log('Connection closed');
    field.insertAdjacentHTML('beforeend', "<tr><td>* * connection closed * *");
    //setTimeout(initWebSocket, 2000);
  }
  function onMessage(event) {
    //var message = event.data;
    field.insertAdjacentHTML('beforeend', "<tr><td>" + event.data );
    if (field.rows.length > 20) {
    var rtm = field.rows.length - 20;
    for (let x=0; x<rtm; x++) { field.deleteRow(0); }
  }
    if (event.data == "clearWindow") { 
    for (let i = 0; i < 22; i++) {
        field.deleteRow(0); }
    }
   }
 
  function onLoad(event) {
    initWebSocket();
    sendEvent();
  }

  function sendEvent() {
    inputField.addEventListener('keyup', function(happen) {
    if (happen.keyCode === 13) {
       happen.preventDefault();
       sendData();
       }   
    });
  }  
  function sendData(){
  var data = inputField.value; 
  websocket.send(data, 1);
  inputField.value = "";
  }

function disConnect() {
  alert("close the console");
  ws.close(); 
}
</script>
</body>
</html>
)=====";


void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) 
{
  AwsFrameInfo *info = (AwsFrameInfo*)arg;

      for(int i=0; i<len; i++ ) 
      {
      txBuffer[i] = data[i];
      }
      txBuffer[len]='\0'; // terminate the array
      consoleOut("console incom " + String(txBuffer));
      if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) 
      {
            //diagNose = 2; // direct the output to ws
          data[len] = 0;

           
          if (strncasecmp(txBuffer+3,"HEALTH",6) == 0) {  
              ws.textAll("check zb system");
              actionFlag=44; // perform the healthcheck
              return;             
          } else          
           if (strncasecmp(txBuffer+3,"SET_OPS=",8) == 0) {  
              normalOps = atoi(txBuffer + 11);
              ws.textAll("write normalOps value: " + String(normalOps));
               return; 
           } else
           if (strncasecmp(txBuffer+3,"CLEAR",5) == 0) {  
              ws.textAll("clearWindow");
              return;             
          } else

          if (strncasecmp(txBuffer+3,"DELETE=",7) == 0) 
          {  
              //input can be 10;DELETE=filename
              String bestand="";
              for(int i=10;  i<len+1; i++) { bestand += String(txBuffer[i]); }
               ws.textAll("bestand = " + bestand); 
              if (SPIFFS.exists(bestand)) 
              {
                  ws.textAll("going to delete file " + bestand); 
                      SPIFFS.remove(bestand);
                      ws.textAll("file " + bestand + " removed!"); 
                      basisConfigsave();  // save inverterCount
              }
              else 
              { 
                 ws.textAll("no such file");
              }
              return;                      
          } else

          if (strncasecmp(txBuffer+3, "JOINBIND", 8) == 0) // normal operation
          {
             if(joinWithBind) {
               joinWithBind = false;
             } else {
              joinWithBind = true;
             }
             
        
              ws.textAll("set joinWithBind to " + String(joinWithBind) );
              //write_eeprom();
              return;   
              // ****************************************************************
          } else  


          if (strncasecmp(txBuffer+3, "DIAG",4) == 0) // normal operation
          {
             switch(diagNose) {
              case 0: 
                  diagNose = 1; 
                  break;
              case 1:
                  diagNose = 2; 
                  break;            
              default:
                  diagNose = 0; 
                  break; 
              }
              ws.textAll("set diagnose to " + String(diagNose) );
              //write_eeprom();
              return;   
              // ****************************************************************
          } else      
      
         
          if (strncasecmp(txBuffer+3, "INTERVIEW=",10) == 0) //
          {
                int kz = String(txBuffer[13]).toInt();
                actionFlag = 70+kz;
                Serial.println("actionflag set to " + String(actionFlag)); 
                diagNose=1;
                return;
          } else {

            ws.textAll("unknown command"); 
          }
  
      }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
    //Serial.println("onEvent triggered");
    switch (type) {
      case WS_EVT_CONNECT:
        //Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        break;
      case WS_EVT_DISCONNECT:
        //Serial.printf("WebSocket client #%u disconnected\n", client->id());
        break;
      case WS_EVT_DATA:
        //Serial.println("WebSocket received data");
        handleWebSocketMessage(arg, data, len);
        break;
      case WS_EVT_PONG:
      case WS_EVT_ERROR:
        break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}
