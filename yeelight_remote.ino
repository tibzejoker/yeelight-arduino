#include <WiFi.h>
#include <ArduinoJson.h>
#include "yeelight.h"
#include <string>


/////////////////////////////////////REMOVE///////////////////////////
#include "D:\arduino\boxInfos.cpp"
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////ADD//////////////////////////////
//String ssid = "MY_BOX_SSID";
//String pass = "MY_BOX_PASSWORD";
//////////////////////////////////////////////////////////////////////

//#define BUTTON_1_PIN "15"
//#define BUTTON_2_PIN "4"
#define BUTTON_1_PIN "13"
#define BUTTON_2_PIN "12"

#define BUTTON_3_PIN "13"
#define BUTTON_4_PIN "12"

const int numberOfButtons = 2;

String buttons[numberOfButtons][4]={
  //salon
  //button, state,  ip address, changeValue
  {BUTTON_1_PIN,  "1","192.168.1.53","n"},
  
  //cuisine
  //button, state,  ip address
  {BUTTON_2_PIN,  "1","192.168.1.54","n"},
  
  //chambre
  //button, state,  ip address, changeValue
  //{BUTTON_3_PIN,  "1","192.168.1.55","n"},
  
  //salle de bain
  //button, state,  ip address
  //{BUTTON_4_PIN,  "1","192.168.1.56","n"}
  
  
  };


  
int maxTics = 1200;
int tics = 0;

bool vverbose = false;

StaticJsonBuffer<200> jsonBuffer;
Yeelight* yeelight;

bool refreshButtons(){
  //GPIO config
  int i = 0;
  int actualState;
  bool lookup = false;
  while(i < numberOfButtons){
    actualState = digitalRead(buttons[i][0].toInt());
    if (actualState != buttons[i][1].toInt()){
        buttons[i][1] = String(actualState);
        buttons[i][3] = "y";
        digitalWrite(2, HIGH);
        lookup = true;
      }
    i++;
  }
  return lookup;
}

void setup() {
  Serial.begin(115200);

  //GPIO config
  int i = 0;
  while(i < numberOfButtons){
    pinMode(buttons[i][0].toInt(), INPUT_PULLUP);
    i++;
  }

  pinMode(2, OUTPUT);
  
  Serial.println("Starting...");
  connectToWiFi(ssid, pass);
  yeelight = new Yeelight();
  //refreshButtons();
  //delay (1000);
  //yeelight->lookup();
}

void toogleDevice(){
    if (yeelight->isPowered()) {
      Serial.println(yeelight->sendCommand("set_power", "[\"off\", \"smooth\", 500]"));
    }else{
      Serial.println(yeelight->sendCommand("set_power", "[\"on\", \"smooth\", 500]"));
    }
}

int findButton(String addresseIP){
  int i = 0;
  while(i < numberOfButtons && i < 100){
    if (addresseIP == buttons[i][2]){
      return i;
    }
    i++;
  }

  return -1;
}

void loop() {
  if (yeelight->feedback()) {
    String addresseIP = yeelight->getIp();
    
    if (vverbose){
      JsonObject& root = jsonBuffer.parseObject(yeelight->sendCommand("get_prop", "[\"power\", \"name\"]"));
      Serial.println("command done");
      //Serial.println(yeelight->sendCommand("set_name", "[\"salon\"]"));
      const char* state = root["result"][0];
      const char* name = root["result"][1];
      Serial.print("device: ");
      Serial.println(yeelight->getLocation());
      Serial.print("- power is: ");
      Serial.println(state);
      Serial.print("- IP is: ");
      Serial.println(yeelight->getIp());
      Serial.print("- name is: ");
      Serial.println(name);  
    }

    
    int i = findButton(addresseIP);

    if (i > -1){
      
      if (buttons[i][3] == "y"){
        Serial.print("Le bouotn pressé est : ");
        Serial.println(i);
        toogleDevice();
        buttons[i][3] = "n";
        digitalWrite(2, LOW);
      }
    }else{
      if (vverbose){
        Serial.println("ERROR IN FINDING BUTTON");
      }
    }
  }

  if (refreshButtons()){
    tics -= maxTics/2;
    yeelight->lookup();  
  }

  tics++;
  delay (1);
  if (tics == maxTics){
    //Serial.println("TIC");
    yeelight->lookup();  
    tics = 0;
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
