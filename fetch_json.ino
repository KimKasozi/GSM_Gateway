// Global vectors to hold unsent message data
std::vector<String> unsentMessageIDs;
std::vector<String> unsentContents;
std::vector<String> unsentRecipients;
std::vector<String> unsentProvidersList;
std::vector<int> unsentSMSCount;

int currentMessageIndex = 0;  // Index to track which message is being sent

bool gsm1TaskCompleted = false;
bool gsm2TaskCompleted = false;

String gsm1Recipients, gsm2Recipients;
String gsm1MessageContents, gsm2MessageContents, gsmMessageContents;




bool isMessageSent(String messageID) {
  if (!SPIFFS.exists("/sentMessages.txt")) {
    return false;  // No messages have been sent yet
  }

  File file = SPIFFS.open("/sentMessages.txt", "r");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return false;
  }

  // Read file line by line
  while (file.available()) {
    String line = file.readStringUntil('\n');
    line.trim();  // Remove newline characters
    if (line.equals(messageID)) {
      file.close();
      return true;  // Message found, already sent
    }
  }

  file.close();
  return false;  // Message not found
}

void markMessageAsSent(String messageID) {
  File file = SPIFFS.open("/sentMessages.txt", "a");  // Append to the file
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }

  file.println(messageID);  // Write messageID to the file
  file.close();
}


// Task function to send all queued unsent messages iteratively
void sendQueuedMessagesTask(void *pvParameters) {
  while (currentMessageIndex < unsentMessageIDs.size()) {
    String messageID = unsentMessageIDs[currentMessageIndex];
    String contents = unsentContents[currentMessageIndex];
    String recipient = unsentRecipients[currentMessageIndex];
    String provider = unsentProvidersList[currentMessageIndex];
    int smsCount = unsentSMSCount[currentMessageIndex];

    Serial.println("Sending message ID: " + messageID);
    
    // Send the message to appropriate GSM
    if(provider.equalsIgnoreCase("MTN")) {
      tasksInProgress = true;
      sendMessage_gsm1(recipient, contents, smsCount, messageID);
    }else if(provider.equalsIgnoreCase("AIRTEL")) {
      tasksInProgress = true;
      sendMessage_gsm2(recipient, contents, smsCount, messageID);
    } 

    
    // Increment to send the next message
    currentMessageIndex++;
    
    // Mark the message as sent
//    markMessageAsSent(messageID);

    // Wait 5 seconds before sending the next message 
    vTaskDelay(1000 / portTICK_PERIOD_MS);  // 1-second delay between sending messages
  }

  // Once all messages are sent, clean up and delete the task
  Serial.println("All queued messages sent.");
  currentMessageIndex = 0;  // Reset index for next batch
  unsentMessageIDs.clear();  // Clear message queue
  unsentContents.clear();
  unsentRecipients.clear();
  unsentProvidersList.clear();
  unsentSMSCount.clear();
  smsToBeSent = 0;

  tasksInProgress = false;
  
  vTaskDelete(NULL);  // Delete the task once done
}

void fetchAndSendMessages() {
  smsToBeSent = 0;
  HTTPClient http;
  String response;
  String url = JSON_URL;

  // Start HTTP request
  http.begin(url);
  int httpCode = http.GET();

  if (httpCode > 0) {
    response = http.getString();
    Serial.println("Response: " + response);

    // Create a JSON document
    StaticJsonDocument<4096> doc;
    DeserializationError error = deserializeJson(doc, response);

    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    // Check if "details" key exists and is an array
    if (!doc["details"].isNull() && doc["details"].is<JsonArray>()) {
      JsonArray details = doc["details"].as<JsonArray>();

      // Iterate through SMS messages
      for (JsonObject sms : details) {
        String messageID = sms["messageID"].as<String>();
        String contents = sms["messageText"].as<String>();
        String recipients = sms["phoneNumber"].as<String>();
        String provider = sms["provider"].as<String>();
        int smsCount = sms["smsCount"].as<int>();

        smsToBeSent += smsCount;

        Serial.println("Processing message: " + messageID);

        // Check if the message has already been sent
        if (!isMessageSent(messageID)) {
          // Queue the unsent messages for sending
          unsentMessageIDs.push_back(messageID);
          unsentContents.push_back(contents);
          unsentRecipients.push_back(recipients);
          unsentProvidersList.push_back(provider);
          unsentSMSCount.push_back(smsCount);
        } else {
          Serial.println("Message already sent: " + messageID);
        }
      }

      Serial.print("Messages to be sent: ");
      Serial.println(smsToBeSent);

      // If there are unsent messages, create a task to send them
      if (!unsentMessageIDs.empty()) {
        xTaskCreatePinnedToCore(sendQueuedMessagesTask, "SendQueuedMTNMessages", 8192, NULL, 4, NULL, 1);
      }
    } else {
      Serial.println(F("\"details\" key missing or not an array in the JSON response."));
    }
  } else {
    Serial.print(F("HTTP GET failed, code: "));
    Serial.println(httpCode);
  }

  http.end();
}
