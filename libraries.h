#include <WiFi.h>       //library for WiFi connectivity
#include <SPIFFS.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <vector>
#include <WiFiManager.h>
#include <HardwareSerial.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

//#define GSM1_TX_PIN 16
//#define GSM1_RX_PIN 17
//
//#define GSM2_TX_PIN 19
//#define GSM2_RX_PIN 21

bool tasksInProgress = false;

HardwareSerial gsm2(2);
HardwareSerial gsm1(1);


WiFiManager wm;

#define JSON_URL "https://kimkasozi.github.io/firmware_update/update.json"

HTTPClient http;

String message;

bool isSent = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  if (!SPIFFS.begin(true)) {
    Serial.println("An error occurred while mounting SPIFFS");
    return;
  }

  bool res;

  res = wm.autoConnect("GSM-GateWay", "12345689");

  if(!res){
    Serial.println("Failed to Create an Access Point");
  }else{
    Serial.println("Access Point Created Successfully!!");
  }

   
  gsm2.begin(38400, SERIAL_8N1, 23, 22);      // For Airtel numbers
  gsm1.begin(38400, SERIAL_8N1, 21, 19);      // For MTN numbers
  
}
