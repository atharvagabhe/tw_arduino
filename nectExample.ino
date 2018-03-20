/*******************   Libraries *************************/
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
//#include <OneWire.h>
//#include <DallasTemperature.h>
float offset = 0;
/*******************   Temperature Sensor (DS18B20) Setup   ************/
// Data wire microcontroller pin
//#define ONE_WIRE_BUS 2

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
//OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
//DallasTemperature sensors(&oneWire);

/******************   WiFi Setup   *********************************/
const char* ssid = "Exorcist.net";
const char* password = "31331715012219208338";

const char* host = "192.168.178.108"; // ThingWorx Server IP Address
const int httpsPort = 8080;

WiFiClient client;


void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());;

  // Start up the DS18B20 sensor
  //sensors.begin();


}

void loop() {
  // Get the Temp from the sensor(s)
  //sensors.requestTemperatures(); // Send the command to get temperatures

  // There is only one sensor connected, so get it from sensor index 0
  //float tempF = sensors.getTempFByIndex(0);
  float tempF = 19.5 + offset;
  offset>10?offset=0:offset++;
  String strTempF = String(tempF);
  Serial.print("Temp: ");
  Serial.println(strTempF);


  // Use WiFiClientSecure class to create TLS connection
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }else{
    Serial.println("Connected to ThingWorx.");
  }

  String url = "/Thingworx/Things/nectThing/Properties/Temperature";
  Serial.print("requesting URL: ");
  Serial.println(url);

  String strPUTReqVal = "{\"Temperature\":\"" + strTempF + "\"}";
  Serial.print("PUT Value: ");
  Serial.println(strPUTReqVal);

  client.print(String("PUT ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "appKey:c56e4f1b-d1d4-4f9d-ab20-0bf3c08dc363" + "\r\n" +
               "x-thingworx-session: false" + "\r\n" +
               "Accept: application/json" + "\r\n" +
               "Connection: close" + "\r\n" +
               "Content-Type: application/json" + "\r\n" +
               "Content-Length: " + String(strPUTReqVal.length()) + "\r\n\r\n" +
               strPUTReqVal+ "\r\n\r\n");
    
  while (client.connected()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  client.stop();

  // Run this every 5 seconds
  delay(5000);
}
