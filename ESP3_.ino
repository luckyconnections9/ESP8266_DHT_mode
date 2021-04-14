//#include <IoTtweet.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiServer.h>
#include <WiFiServerSecure.h>
#include <WiFiServerSecureAxTLS.h>
#include <WiFiServerSecureBearSSL.h>
//#include <WiFiUdp.h>

// #include <Adafruit_ADXL343.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>

#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>


// Import required libraries

// Replace with your network credentials
const char* ssid = "VinCity";
const char* password = "Kartar101";

#define DHTPIN 04     // Digital pin connected to the DHT sensor

// Uncomment the type of sensor in use:
#define DHTTYPE    DHT11     // DHT 11
//#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);

// current temperature & humidity, updated in loop()
float t = 0.0;
float h = 0.0;

// Create AsyncWebServer object on port 6500
AsyncWebServer server(6500);

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;    // will store last time DHT was updated

// Updates DHT readings every 10 seconds
const long interval = 10000;  

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2>Transponder Dr</h2>
  <p><img src='https://martianis.com/wp-content/uploads/2020/07/martianis-logo.png' style='max-width:150px;'></p>
  <p>
    <i class="fas fa-thermometer-half" style="color:#f60029;"></i>
    <span class="dht-labels">Temperature</span>
    <span id="temperature">%TEMPERATURE%</span>
    <sup class="units">&deg;C</sup>
  </p>
  <p>
    <i class="fas fa-tint" style="color:#00add6;"></i>
    <span class="dht-labels">Humidity</span>
    <span id="humidity">%HUMIDITY%</span>
    <sup class="units">%</sup>
  </p>
  <p>
    <input type="button" onClick="SubmitData();" id="start_stop" value="Start" />
    <input type="hidden" value="" id="iapd" />
    <input type="text" readonly value="" id="start_stop_value" />
  </p>
</body>
<script>
function SubmitData() {
  var start_stop = document.getElementById("start_stop").value; alert(start_stop);
  if(start_stop == "Start")
  {
    document.getElementById("start_stop").value="Stop";
    document.getElementById("start_stop_value").value="Processing";

    const headers = new Headers();
    const apiKey = "8bb7a5cb92d42fe"
    const apiSecret = "9fe9136e6406750"
    const url = "http://hcs.martianis.com/api/method/iap.iapd.insertIapd?board=45:80:DE:RS:58&start_time=2020-06-11 16:25:50"
    const token = "token " + apiKey + ":" + apiSecret
   
    headers.append('Authorization', token);
    headers.append('cache-control', 'no-cache');
   
    const init = {
      method: 'GET',
      headers
    }
   
    fetch(url, init)
    .then((response) => {
      return response.text(); // or .json() or .blob() ...
    })
    .then((text) => {
      document.getElementById("iapd").value=JSON.parse(text).message;
      console.log(text) // text is the response body
    })
    .catch((e) => {
      console.log(e) // error in e.message
    });
   
  }
  if(start_stop == "Stop")
  {
    document.getElementById("start_stop").value="Start";
    const headers = new Headers();
    const apiKey = "8bb7a5cb92d42fe"
    const apiSecret = "9fe9136e6406750"
    const url = "http://hcs.martianis.com/api/method/iap.iapd.insertIapdendtime?iapd="+document.getElementById("iapd").value+"&end_time=2020-05-13%2017:25:50"
    const token = "token " + apiKey + ":" + apiSecret
   
    headers.append('Authorization', token);
    headers.append('cache-control', 'no-cache');
   
    const init = {
      method: 'GET',
      headers
    }
   
    fetch(url, init)
    .then((response) => {
      return response.text(); // or .json() or .blob() ...
    })
    .then((text) => {
      document.getElementById("start_stop_value").value="";
      document.getElementById("iapd").value="";
      console.log(text) // text is the response body
    })
    .catch((e) => {
      console.log(e) // error in e.message
    });
  }
}

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature").innerHTML = this.responseText;

      if(document.getElementById("start_stop_value").value == "Processing")
      {
        var iapd = document.getElementById("iapd").value;
        const headers = new Headers();
        const apiKey = "8bb7a5cb92d42fe"
        const apiSecret = "9fe9136e6406750"
        const url = "http://hcs.martianis.com/api/method/iap.iapd.insertIapdata?iapd="+iapd+"&device=TEMPERATURE&device_data="+this.responseText
        const token = "token " + apiKey + ":" + apiSecret
       
        headers.append('Authorization', token);
        headers.append('cache-control', 'no-cache');
       
        const init = {
          method: 'GET',
          headers
        }
       
        fetch(url, init)
        .then((response) => {
          return response.text(); // or .json() or .blob() ...
        })
        .then((text) => {
         console.log("Temperature Updated") // text is the response body
        })
        .catch((e) => {
          console.log(e) // error in e.message
        });
      }
   
    }
  };
  xhttp.open("GET", "/temperature", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humidity").innerHTML = this.responseText;

      if(document.getElementById("start_stop_value").value == "Processing")
      {
        var iapd = document.getElementById("iapd").value;
        const headers = new Headers();
        const apiKey = "8bb7a5cb92d42fe"
        const apiSecret = "9fe9136e6406750"
        const url = "http://hcs.martianis.com/api/method/iap.iapd.insertIapdata?iapd="+iapd+"&device=HUMIDITY&device_data="+this.responseText
        const token = "token " + apiKey + ":" + apiSecret
       
       /* headers.append('Authorization', token);
        headers.append('cache-control', 'no-cache');
       
        const init = {
          method: 'GET',
          headers
        }
       
        fetch(url, init)
        .then((response) => {
          return response.text(); // or .json() or .blob() ...
        })
        .then((text) => {
         console.log("Humidity Updated") // text is the response body
        })
        .catch((e) => {
          console.log(e) // error in e.message
        }); */
      }

     
    }
  };
  xhttp.open("GET", "/humidity", true);
  xhttp.send();
}, 12000 ) ;
</script>
</html>)rawliteral";

const char vindex_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
Transponder Dr is Working~
<a href="/vindex">Click here<a/> to Check Temperature~
</body>
</html>)rawliteral";


// Replaces placeholder with DHT values
String processor(const String& var){
  //Serial.println(var);
  if(var == "TEMPERATURE"){
    return String(t);
  }
  else if(var == "HUMIDITY"){
    return String(h);
  }
  return String();
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  dht.begin();
 
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(".");
  }
 
  //Setting up martianis AP
 
  //
 
  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", vindex_html, processor);
  });
  server.on("/vindex", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(t).c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(h).c_str());
  });

 
 
 
  // Start server
  server.begin();
}
 
void loop(){  
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you updated the DHT values
    previousMillis = currentMillis;
    // Read temperature as Celsius (the default)
    float newT = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    //float newT = dht.readTemperature(true);
    // if temperature read failed, don't change t value
    if (isnan(newT)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
      t = newT;
      Serial.println(t);
    }
    // Read Humidity
    float newH = dht.readHumidity();
    // if humidity read failed, don't change h value
    if (isnan(newH)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
      h = newH;
      Serial.println(h);
    }
  }
}
