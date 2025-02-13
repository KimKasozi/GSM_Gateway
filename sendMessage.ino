void sendMessage_gsm1(String contact, String message, int smsCount, String messageID) {
String Status;
//int smsBalanceB4 = 0;
//smsBalanceB4 = gsm1CheckBalance();
//Serial.print("GSM1 Balance b4 Send: ");
//Serial.println(smsBalanceB4);
  Serial.println("GSM1 Sending message to +" + contact + ": " + message);
  gsm1.println("AT+CMGF=1");
   
  delay(200);
  gsm1.println("AT+CMGS=\"+" + contact + "\"");
   
  delay(200);
  gsm1.print(message);
   
  delay(200);
  gsm1.write(26); // CTRL+Z to send the message
   
  String response = "";
  unsigned long startTime = millis();
  while (millis() - startTime < 15000) {  // Timeout after 15 seconds
    while (gsm1.available()) {
      char c = gsm1.read();
      response += c;
    }
  }

  Serial.println();
  Serial.println();
  Serial.println("The response from the GSM is:");
  Serial.println(response);
  Serial.println();
  Serial.println();

  // Check if the message was sent successfully
  if (response.indexOf("+CMGS:") != -1 && response.indexOf("OK") != -1) {
    markMessageAsSent(messageID);
    Status = "Success";
    Serial.println("Message send to +" + contact + " Successful!");
  } else if (response.indexOf("ERROR") != -1) {
    Status = "Cancelled";
    Serial.println("Message send to +" + contact + " Cancelled!");
  } else {
    Status = "Cancelled";
    Serial.println("Message send to +" + contact + " Cancelled!");
  }
  
  
  
//delay(10000); // 10-SECONDS delay between previous and next AT commands
//
//int smsBalanceAfta = 0;
//smsBalanceAfta = gsm1CheckBalance();
//Serial.print("GSM1 Balance After Send: ");
//Serial.println(smsBalanceAfta);
//
//if(smsBalanceB4 - smsBalanceAfta == smsCount){
//  smsBalanceB4 = 0;
//  smsBalanceAfta = 0;
//  Status = "Success";
//}else{
//  Status = "Failed";
//}

  String jsonPayload;    
  jsonPayload = "{\"message_id\":\"" + messageID + "\",\"status\":\"" + Status + "\"}";

  // Retry mechanism for server upload
  bool uploadSuccess = false;
  while (!uploadSuccess) {
    http.begin("http://192.168.1.200:8080/Aliesms-API/api/message/delivery/report"); 

    // Set headers
    http.addHeader("Content-Type", "application/json");

    int statusCode = http.POST(jsonPayload);

    Serial.println("The payload sent is: " + jsonPayload);
    Serial.print("The status code is: ");
    Serial.println(statusCode);
    Serial.println("\n");

    if (statusCode == 200) {
      uploadSuccess = true;  // Exit loop when upload is successful
      Serial.println("Payload uploaded successfully!");
    } else {
      Serial.println("Failed to upload payload. Retrying...");
      delay(3000);  
    }

  }     
}


//                                    *********************
//                              *************************************
//                       **************************************************
// *********************************GSM 2 Sending Function ******************************************
//                       **************************************************
//                              *************************************
//                                    *********************

void sendMessage_gsm2(String contact, String message, int smsCount, String messageID) {
String Status;
//int smsBalanceB4 = 0;
//smsBalanceB4 = gsm2CheckBalance();
//Serial.print("GSM2 Balance b4 Send: ");
//Serial.println(smsBalanceB4);
  Serial.println("GSM2 Sending message to +" + contact + ": " + message);
  gsm2.println("AT+CMGF=1");
   
  delay(200);
  gsm2.println("AT+CMGS=\"+" + contact + "\"");
   
  delay(200);
  gsm2.print(message);
   
  delay(200);
  gsm2.write(26); // CTRL+Z to send the message
   
  String response = "";
  unsigned long startTime = millis();
  while (millis() - startTime < 15000) {  // Timeout after 15 seconds
    while (gsm2.available()) {
      char c = gsm2.read();
      response += c;
    }
  }

  Serial.println();
  Serial.println();
  Serial.println("The response from the GSM is:");
  Serial.println(response);
  Serial.println();
  Serial.println();

  // Check if the message was sent successfully
  if (response.indexOf("+CMGS:") != -1 && response.indexOf("OK") != -1) {
    markMessageAsSent(messageID);
    Status = "Success";
    Serial.println("Message send to +" + contact + " Successful!");
  } else if (response.indexOf("ERROR") != -1) {
    Status = "Cancelled";
    Serial.println("Message send to +" + contact + " Cancelled!");
  } else {
    Status = "Cancelled";
    Serial.println("Message send to +" + contact + " Cancelled");
  }
  
  
  
//delay(10000); // 10-SECONDS delay between previous and next AT commands
//
//int smsBalanceAfta = 0;
//smsBalanceAfta = gsm2CheckBalance();
//Serial.print("GSM2 Balance After Send: ");
//Serial.println(smsBalanceAfta);
//
//if(smsBalanceB4 - smsBalanceAfta == smsCount){
//  smsBalanceB4 = 0;
//  smsBalanceAfta = 0;
//  Status = "Success";
//  // Mark the message as sent
//  markMessageAsSent(messageID);
//}else{
//  Status = "Failed";
//}

  String jsonPayload;    
  jsonPayload = "{\"message_id\":\"" + messageID + "\",\"status\":\"" + Status + "\"}";
     
  // Retry mechanism for server upload
  bool uploadSuccess = false;
  while (!uploadSuccess) {
    http.begin("http://192.168.1.200:8080/Aliesms-API/api/message/delivery/report"); 

    // Set headers
    http.addHeader("Content-Type", "application/json");

    int statusCode = http.POST(jsonPayload);

    Serial.println("The payload sent is: " + jsonPayload);
    Serial.print("The status code is: ");
    Serial.println(statusCode);
    Serial.println("\n");

    if (statusCode == 200) {
      uploadSuccess = true;  // Exit loop when upload is successful
      Serial.println("Payload uploaded successfully!");
      Serial.println("The payload sent is: " + jsonPayload);
    } else {
      Serial.println("Failed to upload payload. Retrying...");
      delay(3000);  
    }
  }
  
}
