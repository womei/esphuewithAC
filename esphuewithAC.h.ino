#include <ESP8266WiFi.h>

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include "WiFiManager.h" 


const char* host = "192.168.XXX.XXX";
const char* hueUsername = "fill in username here ";

boolean isconnected = false;
boolean LightsOn = true;
int counter = 0;

boolean setLight(int lightNum,String command, boolean verbose = true)
{
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return false;
  }

  client.print("PUT /api/");
  client.print(hueUsername);
  client.print("/lights/");
  client.print(lightNum);  // hueLight zero based, add 1
  client.println("/state HTTP/1.1");
  
  client.print("Host: ");
  client.println(host);
  
//  client.print("Content-Length: ");
//  client.println(command.length());
//  
//  client.println("Content-Type: text/plain;charset=UTF-8");

  client.println();  // blank line before body
  
  client.println(command);  // Hue command
  
  if(verbose){
    delay(10);
   
   // Read all the lines of the reply from server and print them to Serial
   while(client.available()){
     String line = client.readStringUntil('\r');
     Serial.print(line);
   }
  }
  return true;  // command executed

}

boolean setAll(String command, boolean verbose = true)
{
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return false;
  }

  client.print("PUT /api/");
  client.print(hueUsername);
  client.print("/groups/0");
  client.println("/action HTTP/1.1");
  client.println("keep-alive");
  client.print("Host: ");
  client.println(host);
  client.print("Content-Length: ");
  client.println(command.length());
  client.println("Content-Type: text/plain;charset=UTF-8");
  client.println();  // blank line before body
  client.println(command);  // Hue command
  
  if(verbose){
    delay(10);
   
   // Read all the lines of the reply from server and print them to Serial
   while(client.available()){
     String line = client.readStringUntil('\r');
     Serial.print(line);
   }
  }
  else{
   //if we're not reading the reply, let's be kind enought to flush what's there
  client.flush();
  }
  //terminate the connection
  client.stop();
  return true;  // command executed

}

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
}

void setup() {
 Serial.begin(115200);
  delay(10);

  pinMode(D6, INPUT_PULLUP);
  // We start by connecting to a WiFi network 
  WiFiManager wifiManager;

  wifiManager.setAPCallback(configModeCallback);

  if(!wifiManager.autoConnect()) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  } 
  
  isconnected = true;
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {

//check the buttons
  if(!digitalRead(D6)){
    if(!LightsOn){
      Serial.println("lights on");
      setAll("{\"on\":true}");
      delay(100);
      LightsOn = true;
    }
    else{
      Serial.println("lights off");
      setAll("{\"on\":false}");
      delay(100);
      LightsOn = false;
    }
  }
  //not so pretty fix to reconnect to wifi if connection stopped working for some reason
  if(!isconnected){
  WiFiManager wifiManager;
  wifiManager.resetSettings();
  isconnected = true;
  }
}


