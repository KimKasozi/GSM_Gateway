//void updateSerial(){
//  delay(200);
//  while(Serial.available()){
//    gsm1.write(Serial.read());
//  }
//
//  while(gsm1.available()){
//    Serial.write(gsm1.read());
//  }
//}

void sendMessage_gsm1(String contact, String message) {
  Serial.println("GSM1 Sending message to 0" + contact + ": " + message);
  gsm1.println("AT+CMGF=1");
   
  delay(200);
  gsm1.println("AT+CMGS=\"+256" + contact + "\"");
   
  delay(200);
  gsm1.print(message);
   
  delay(200);
  gsm1.write(26); // CTRL+Z to send the message
   
  
  Serial.println("Message sent successfully to " + contact);
  
  delay(4000); // Add a delay between messages
}


//                                    *********************
//                              *************************************
//                       **************************************************
// *********************************GSM 2 Sending Function ******************************************
//                       **************************************************
//                              *************************************
//                                    *********************

void sendMessage_gsm2(String contact, String message) {
  Serial.println("GSM2 Sending message to 0" + contact + ": " + message);
  gsm2.println("AT+CMGF=1");
   
  delay(200);
  gsm2.println("AT+CMGS=\"+256" + contact + "\"");
   
  delay(200);
  gsm2.print(message);
   
  delay(200);
  gsm2.write(26); // CTRL+Z to send the message
   
  
  Serial.println("Message sent successfully to " + contact);
  
  delay(4000); // Add a delay between messages
}
