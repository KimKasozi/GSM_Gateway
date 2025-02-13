#include <WiFi.h>       //library for WiFi connectivity
#include <SPIFFS.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <vector>
#include <WiFiManager.h>
#include <HardwareSerial.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <map>

#define GSM1_TX_PIN 16
#define GSM1_RX_PIN 17
//
#define GSM2_TX_PIN 22
#define GSM2_RX_PIN 23

bool tasksInProgress = false;

HardwareSerial gsm1(1);
HardwareSerial gsm2(2);
//HardwareSerial gsm3(1);
//HardwareSerial gsm4(2);


WiFiManager wm;

#define JSON_URL "http://192.168.1.200:8080/Aliesms-API/api/message/request"

HTTPClient http;

String message;

bool isSent = false;

int smsToBeSent = 0;


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

  
  gsm1.begin(38400, SERIAL_8N1, GSM1_TX_PIN, GSM1_RX_PIN);      // For MTN numbers
  gsm2.begin(38400, SERIAL_8N1, GSM2_TX_PIN, GSM2_RX_PIN);      // For Airtel numbers
//gsm3.begin(38400, SERIAL_8N1, 19, 21);      // For MTN numbers 
//gsm4.begin(38400, SERIAL_8N1, 33, 32);        // For Airtel Numbers 
}

String checkGsmResponseTask1() {
  String gsmResponse = "";
  while (gsm1.available()){
    char receivedChar = gsm1.read(); // Read each character
    gsmResponse += receivedChar; // Append to the response string
  }

  return gsmResponse;  
}

String checkGsmResponseTask2() {
  String gsmResponse = "";
  while (gsm2.available()){
    char receivedChar = gsm2.read(); // Read each character
    gsmResponse += receivedChar; // Append to the response string
  }

  return gsmResponse;
}
