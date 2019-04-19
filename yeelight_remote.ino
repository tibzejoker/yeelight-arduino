#include <WiFi.h>
#include <ArduinoJson.h>
#include "yeelight.h"

/////////////////////////////////////REMOVE///////////////////////////
#include "D:\arduino\boxInfos.cpp"
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////ADD//////////////////////////////
//char* ssid = "MY_BOX_SSID";
//char* pass = "MY_BOX_PASSWORD";
//////////////////////////////////////////////////////////////////////

#define BUTTON_1 15
#define BUTTON_2 4

int maxTics = 200;

int tics = 0;
int ticSalon = 0;
int ticCuisine = 0;
bool salonPressed = false;
bool salonToogled = false;
const char* ipSalon = "192.168.1.53";

bool cuisinePressed = false;
const char* ipCuisine = "192.168.1.54";
bool cuisineToogled = false;
char* pressed = "";

bool vverbose = false;

StaticJsonBuffer<200> jsonBuffer;
Yeelight* yeelight;

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_1, INPUT_PULLUP);
  pinMode(BUTTON_2, INPUT_PULLUP);
  
  Serial.println("Starting...");

  connectToWiFi(ssid, pass);

  yeelight = new Yeelight();
  yeelight->lookup();
}

void toogleDevice(){
    if (yeelight->isPowered()) {
      Serial.println(yeelight->sendCommand("set_power", "[\"off\", \"smooth\", 50]"));
    }else{
      Serial.println(yeelight->sendCommand("set_power", "[\"on\", \"smooth\", 50]"));
    }
}

void loop() {
  if (yeelight->feedback()) {
    


    JsonObject& root = jsonBuffer.parseObject(yeelight->sendCommand("get_prop", "[\"power\", \"name\"]"));
    Serial.println("command done");
    //Serial.println(yeelight->sendCommand("set_name", "[\"salon\"]"));
    const char* state = root["result"][0];
    const char* name = root["result"][1];
    if (vverbose){
      Serial.print("device: ");
      Serial.println(yeelight->getLocation());
      Serial.print("- power is: ");
      Serial.println(state);
      Serial.print("- IP is: ");
      Serial.println(yeelight->getIp());
      Serial.print("- name is: ");
      Serial.println(name);  
    }
    
    if (yeelight->getIp() == ipSalon && salonPressed && !salonToogled){
      salonToogled = true;
      
      toogleDevice();
    }
    
    if (yeelight->getIp() == ipCuisine && cuisinePressed && !cuisineToogled){
      cuisineToogled = true;
      toogleDevice();
    }
        
    pressed = "";
  }

  if (digitalRead(BUTTON_1) == 0 && !salonPressed){
    yeelight->lookup();  
    pressed = "salon";
    salonPressed = true;
    ticSalon = tics;
    Serial.println(pressed);
  }
  
  if (digitalRead(BUTTON_2) == 0 && !cuisinePressed){
    yeelight->lookup();  
    pressed = "cuisine";
    ticCuisine = tics;
    cuisinePressed = true;
    Serial.println(pressed);
  }
  tics++;
  if (tics == maxTics){
    //Serial.println("TIC");
    tics = 0;
  }
  delay(1);
  
  if (cuisinePressed){
    if (ticCuisine == tics){
      cuisinePressed = false;
      cuisineToogled = false;
    }
    
  }
  
  
  if (salonPressed){
    if (ticSalon == tics){
      salonPressed = false;
      salonToogled = false;
    }
    
  }
  
  
  
}

void connectToWiFi(const char * ssid, const char * pwd)
{
  WiFi.begin(ssid, pwd);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }

  Serial.println("");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}
