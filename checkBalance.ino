int gsm1CheckBalance(){
  String Response = "";
  int smsCount = 0;
  gsm1.println("AT+CUSD=1, \"*131#\"");
  delay(6000);
  
  Response = checkGsmResponseTask1();  

  if (Response.length() > 0) {
    int smsIndex = Response.indexOf(" SMS;");
    if (smsIndex != -1) {
      // Locate the start of the number (space before it)
      int numberStart = Response.lastIndexOf(' ', smsIndex - 1) + 1;
      String smsPart = Response.substring(numberStart, smsIndex); // Include " SMS"
      smsCount = smsPart.toInt(); 
    }
    
    Response = "";
  }

  return smsCount;  
}

int gsm2CheckBalance(){
  String Response = "";
  int smsCount = 0;
  gsm2.println("AT+CUSD=1, \"*131#\"");
  delay(6000);
  
  Response = checkGsmResponseTask2();

  if (Response.length() > 0) {
    int smsIndex = Response.indexOf("SMS");
    if (smsIndex != -1) {
      // Use a regex or substring to extract digits before "SMS"
      int numberEnd = smsIndex; // The position where the number ends
      int numberStart = numberEnd - 1;

      // Find where the number starts (scan backwards until non-digit)
      while (numberStart >= 0 && isDigit(Response[numberStart])) {
        numberStart--;
      }
      numberStart++; // Move to the first digit

      String smsPart = Response.substring(numberStart, numberEnd); // Extract number
      smsCount = smsPart.toInt();
    }
    
    Response = "";
  }

  return smsCount;  
}
