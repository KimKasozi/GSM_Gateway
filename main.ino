#include "libraries.h"

void loop() {
  // put your main code here, to run repeatedly:
  if (!tasksInProgress) {
    fetchAndSendMessages();
    Serial.println("Waiting 30 seconds for the next fetch...");
    delay(30000);
    Serial.println("Time's up. Preparing the next fetch...");
  }else{
    delay(1000);
  }
}
