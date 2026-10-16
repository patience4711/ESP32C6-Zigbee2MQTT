//.ls table {max-width: 98% }
//.l_s table { width: 100%; max-width: 500px;}
//.l_s table, .l_s th, .l_s td {border: 2px solid blue; font-size:14px; padding:6px; text-align:center; border-collapse:collapse;backgound-color:#dfff80;}

const char ESP_HOMEPAGE [] PROGMEM = R"=====(

<!DOCTYPE html><html><head><meta charset='utf-8' name='viewport' content='width=device-width, initial-scale=1'>
<title>ESP32-Z2M</title><link rel="icon" type="image/x-icon" href="/favicon.ico" />
<link rel='stylesheet' type='text/css' href='/STYLESHEET'>
<style>
body.l_s { background-color: #EEE;}
.l_s span {padding: 6px;}

.l_s table { width: 100%; max-width: 500px; border: 2px solid blue; font-size:14px; text-align:center; border-collapse:collapse; background-color:#dfff80;}
.l_s th, .l_s td { border: 2px solid blue; padding:6px; }

.l_s tr {background-color:#ccffcc;}
.l_s .btn { background-color: #199319; color: white; padding: 5px 22px; border-radius:6px;}
.l_s .btn:hover {background: #eeeF; color:black;} 

@media only screen and (max-width: 800px) {
  .l_s th, .l_s td { width:60px; font-size:11px;}
  .l_s tr {height:35px;} 
  .l_s .btn { padding: 5px 18px; font-size:10px;}}
</style>
<script type="text/javascript" src="SECURITY"></script>
<script type="text/javascript" src="JAVASCRIPT"></script>
</head>
<div class="l_s">
<body class="l_s" onload='loadScript()'>
<div id='msect'>
<div id='menu'>
<a class='fright' id='ml' href='/MENU'>menu</a>
</div>
<kop>ESP32-Z2M</kop><div class='divstijl' id='maindiv'><center><p>

<h4>DEVICES</h4>
<div id='devices' style='display:none;'><center><table><tr style='Background-color:lightblue; font-weight:bold; text-align:center; border:4px solid black;'>
<td style="width:130px">name<td style="width:50px">type<td style="width:220px; white-space: nowrap;">control / data</td></tr>
<tr id='r0' style='display:none;'><td id='n0'></td><td id='t0'></td><td id='c0'></td></tr>
<tr id='r1' style='display:none;'><td id='n1'></td><td id='t1'></td><td id='c1'></td></tr>
<tr id='r2' style='display:none;'><td id='n2'></td><td id='t2'></td><td id='c2'></td></tr>
<tr id='r3' style='display:none;'><td id='n3'></td><td id='t3'></td><td id='c3'></td></tr>
<tr id='r4' style='display:none;'><td id='n4'></td><td id='t4'></td><td id='c4'></td></tr>
<tr id='r5' style='display:none;'><td id='n5'></td><td id='t5'></td><td id='c5'></td></tr>
<tr id='r6' style='display:none;'><td id='n6'></td><td id='t6'></td><td id='c6'></td></tr>
<tr id='r7' style='display:none;'><td id='n7'></td><td id='t7'></td><td id='c7'></td></tr>
<tr id='r8' style='display:none;'><td id='n8'></td><td id='t8'></td><td id='c8'></td></tr>
</table></div><p>Powered by Hansiart</p>
<div id='busy'><span style='color:red;'><h3>checking / initialyzing zigbee network...</h3></span>
<br></div><div id='failed' style='display:none;'><span style='color:red;'><h3>the zigbee module is not working.</h3></span>
<br></div></center></div></div></div></body></html>
)=====";


// this is the original javascript. Can made compact via https://www.digitalocean.com/community/tools/minify
const char JAVA_SCRIPT[] PROGMEM = R"=====(

var term;
//var table_row;
var cnt = 0;
//var totalEn = 0;

function loadScript() {
  getGeneral();
  //console.log("getGeneral done, now getDevices()");
  setTimeout(getDevices, 300);
}

function getDevices() {
  //console.log("getAll count = " + cnt);
  for (let i = 0; i < cnt; i++) {
  term = "get.Data?devnr=" + i;
  //table_row = "dev" + i;
  getData(i);
  }

}

function getData(devnr) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var antwoord = this.responseText;
      //console.log("inverter = " + invnr);
      var obj = JSON.parse(antwoord);
      
      var regel = "r" + String(devnr);
      document.getElementById(regel).style.display="table-row";
      var cel = "c" + String(devnr);
      var celEl = document.getElementById(cel); // this is the last cel in de tablerow
      celEl.innerHTML = "n/a";
      
      if(obj.type == 1){  // read temperature
      celEl.innerHTML = "temp " + obj.read0.toFixed(1) + "&deg;C; hum = " + obj.read1.toFixed(1) + "%";
      }
      
      if(obj.type == 2) {  // motion seen?
        
        celEl.innerHTML = "motion " + (obj.read0 != 0 ? "YES" : "NO");
        
        if (obj.read0 != 0) {
            celEl.style.backgroundColor = "red";
            celEl.style.color = "white"; // Optional: witte text better readable
        } else {
            celEl.style.backgroundColor = "#ccffcc"; // standaard color
            celEl.style.color = "navy"; // standard
        }
      }
      
      if(obj.type == 3) {
        celEl.innerHTML = "<a href='/CONTROL?dev=" + String(devnr) + "'><button class='btn'>control</button</a>";
      } 
       if(obj.type == 4) {
        celEl.innerHTML = "<a href='/ONOFF?dev=" + String(devnr) + "'><button class='btn'>control</button</a>";
      }       

      //cel="i" + String(devnr);
      //document.getElementById(cel).innerHTML = obj.adr;
      cel="n" + String(devnr);
      document.getElementById(cel).innerHTML = obj.name;
      cel="t" + String(devnr);
      document.getElementById(cel).innerHTML = obj.type;
               
      document.getElementById("devices").style.display = "block";
    }
  }
  xhttp.open("GET", term, true);
  xhttp.send();
}

function getGeneral() {
//  console.log("term = " + term);
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var antwoord = this.responseText;
//      console.log("answer = " + antwoord);
      var obj = JSON.parse(antwoord);
      cnt = obj.cnt;
      console.log("getGeneral cnt = " + cnt);
      var st = obj.st;  //state
      var sl = obj.sl;  // sleep
      var rem = obj.rm;
      if(rem == 0) {document.getElementById("ml").style.display = "inline-block";} // show menu link          
      if(st == 11) {
        document.getElementById("busy").style.display = "block";
      } else {
        document.getElementById("busy").style.display = "none";
        document.getElementById("failed").style.display = "none";
      }
      if(st == 0 || st == 25) {
       document.getElementById("failed").style.display = "block";
       }

    }
  }
  xhttp.open("GET", "get.Data?General=1", true);
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
  if(e.data == "getDevices") {
  getAll();
  }
  if (e.data == "getdata") {
    getGeneral();
    setTimeout(getDevices, 300); 
  }
  if (e.data == "general") {
    getGeneral();
  }
 
 
 }, false);
}

)=====";

