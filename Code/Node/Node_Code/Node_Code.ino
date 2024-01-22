#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <LiquidCrystal_I2C.h>
#include "ThingSpeak.h"

//D6 = Rx & D5 = Tx
SoftwareSerial nodemcu(D6, D5);

 unsigned long ch_no = 1814513;
    const char * write_api = "CLTWJCINMR1NNPOF";
    const char * read_api = "DXRR1I8TGKEA692Q";
    String apiKey = "CLTWJCINMR1NNPOF"; 
       char auth[] = "mwa0000027030829";
       
       char ssid[] = "capstone21";
       char pass[] = "123456789";
       
const char* server = "api.thingspeak.com";

    unsigned long startMillis;
    unsigned long currentMillis;
    const unsigned long period = 10000;
    
String myStatus = "";

    WiFiClient client;
    
    int P;
    
void setup() {
  // Initialize Serial port
  Serial.begin(9600);
  nodemcu.begin(9600);
  
  while (!Serial) continue;
  WiFi.begin(ssid, pass);
          while (WiFi.status() != WL_CONNECTED)
          {
            delay(500);
            Serial.print(".");
//            lcd.clear();
//            lcd.print("............");
            delay(2000);
          }
              Serial.println("WiFi connected");
              Serial.println(WiFi.localIP());
//              lcd.clear();
//              lcd.setCursor(0,0);
//              lcd.print("WiFi connected");
//              lcd.setCursor(2,1);
//              lcd.print(WiFi.localIP());
              delay(2000);
              ThingSpeak.begin(client);
              startMillis = millis();
}


void loop() {
  
  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& data = jsonBuffer.parseObject(nodemcu);

  if (data == JsonObject::invalid()) {
    Serial.println("Invalid Json Object");
    jsonBuffer.clear();
    return;
  }

  Serial.println("JSON Object Recieved");
  
  Serial.print("Recieved GridsupplyVoltage:  ");
  String GridsupplyVoltage = data["GridsupplyVoltage_"];
  Serial.println(GridsupplyVoltage);
  
  Serial.print("Recieved KWH:  ");
  float KWH = data["KWH_"];
  Serial.println(KWH);
  
  Serial.print("Recieved Amps_RMS:  ");
  float Amps_RMS = data["Amps_RMS_"];
  Serial.println(Amps_RMS);
  
  Serial.print("Recieved SolarsupplyVoltage:  ");
  float SolarsupplyVoltage = data["SolarsupplyVoltage_"];
  Serial.println(SolarsupplyVoltage);
  
  Serial.print("Recieved KWH1:  ");
  float KWH1 = data["KWH1_"];
  Serial.println(KWH1);

  Serial.print("Recieved Amps_RMS1:  ");
  float Amps_RMS1 = data["Amps_RMS1_"];
  Serial.println(Amps_RMS1);

  Serial.print("Recieved KWH2:  ");
  float KWH2 = data["KWH2_"];
  Serial.println(KWH2);

  Serial.print("Recieved alarm:  ");
  float alarm = data["alarm_"];
  Serial.println(alarm);
  
  Serial.println("-----------------------------------------");


ThingSpeak.writeField(ch_no,4,KWH2,write_api);
  ThingSpeak.writeField(ch_no,5,GridsupplyVoltage,write_api);
//  
ThingSpeak.writeField(ch_no,8,alarm,write_api);
//
ThingSpeak.writeField(ch_no,1,SolarsupplyVoltage,write_api);
////delay(16000);
ThingSpeak.writeField(ch_no,2,Amps_RMS,write_api);
//delay(16000);
ThingSpeak.writeField(ch_no,3,KWH1,write_api);
//delay(6000);
ThingSpeak.writeField(ch_no,4,KWH2,write_api);
//delay(16000);
ThingSpeak.writeField(ch_no,5,GridsupplyVoltage,write_api);

ThingSpeak.writeField(ch_no,6,Amps_RMS1,write_api);
delay(16000);
ThingSpeak.writeField(ch_no,7,KWH,write_api);
//delay(16000);
delay(1000);

}
