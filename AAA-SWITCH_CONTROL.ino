const char SWITCH_PAGE[] PROGMEM = R"=====(
<!DOCTYPE html><html><head><meta charset='utf-8'>
<title>ESP32-Zib=gbee2Mqtt</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link href="data:image/x-icon;base64,AAABAAEAEBAQAAEABAAoAQAAFgAAACgAAAAQAAAAIAAAAAEABAAAAAAAgAAAAAAAAAAAAAAAEAAAAAAAAAAu29YA0Q8YALTd3gBm4eMA0+blAIHh4wCa2tsAdd/gALUUnQC24+IAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAARERERERERERJkZmRGZkZlEkhIhIhIhKUSSFmFmFmEpRJIVUVUVUSlEkhV3EXcRGUSSZQAAB1YpRJJlczMHVilEiGWIOAdWKESIZYgIh1aIRIhliHiIiIhEiIiIVYhYiUSIZohmaCgpRIgiiCIoiClEiJmImZmJmUREREREREREQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" rel="icon" type="image/x-icon"/>
<link rel="stylesheet" type="text/css" href="/STYLESHEET">
<style>
#demoLPM:hover {
  cursor: pointer;
  background-color: yellow;
  font-size: 20px;
}
.btlabel {text-align:center; color: white;font-size:25px;}
       
.bt2 {
  font-size:16px;
  padding: 10px;
  margin:6px;
  text-align:center;
  border-radius:8px;
  color:white;
  background: #8742f5;
  box-shadow: 0 6px #302442;
  display: block;
  }

@media only screen and (max-width: 600px) {

}
</style>
</head>
<body onload='loadScript()'>
<div id='msect'>
<div id="menu">
<a  href="/" class='close' >&times;</a>
<a href=# id="goBack">devices</a>
</div>
</div>
<div id='msect'>
<br>
<kop>ESP32-Z2M SWITCH <span id="NAME"></span></kop>
<br></div>
<div id='msect'>
  <div class='divstijl' id = 'maindiv' style='height:74vh; background: grey;'>
  <center> <br><span id='pwdby'style='font-size:11px; margin:auto; display:table; color:white;'>powered by Hansiart</span>
<br>
<button id="bt0" class="bt" onclick="buttonFunction(11)"> </button>
<br>
</div><br>
</div></body>

<script>
var myLink=document.getElementById("goBack");
document.addEventListener("visibilitychange", function() {
    if (!document.hidden){
        console.log("Browser tab is visible")
        getData(); // update the page 
    } 
});


function loadScript() {
// the first time we load the settings at once
getData();
// request data at onLoad and onEvent
}

function buttonFunction(button) {
var xhttp = new XMLHttpRequest();
var buttonPressed="?BUTTON=" + button;
var what = "/BUTTON" + buttonPressed;
xhttp.open("GET", what, true);
xhttp.send();

}

function getData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) 
    {
      var antwoord = this.responseText;
      var obj = JSON.parse(antwoord);
      var onoff = obj.onoff;
      var devnr = obj.devnr; 
      myLink.href="/DEV?welke="+devnr;
      document.getElementById("NAME").textContent = devnr;
      if (!obj.onoff) {
      document.getElementById("bt0").style.background="#b9b9c1";
      document.getElementById("maindiv").style.background="grey";
      document.getElementById("bt0").textContent = "OFF";
      document.getElementById("pwdby").style.color="white";
      } else { 
      document.getElementById("bt0").style.background ="green";
      document.getElementById("bt0").textContent = "ON";
      document.getElementById("maindiv").style.background="linear-gradient(#e8edc8, #c8eaed)";
      document.getElementById("pwdby").style.color="green";
      }
    }
  };
  xhttp.open("GET", "get.Data?Switch=1", true);
  xhttp.send();
}

if (!!window.EventSource) {
 var source = new EventSource('/events');

 source.addEventListener('open', function(e) {
  console.log("Events Connected");
 }, false);
 source.addEventListener('error', function(e) {
  if (e.target.readyState != EventSource.OPEN) {
    console.log("Events Disconnected");
  }
 }, false);

 source.addEventListener('message', function(e) {
  console.log("message", e.data);
  if(e.data == "getdata") {
     getData();
  }

 }, false);

}

</script>
</html>
)=====";


