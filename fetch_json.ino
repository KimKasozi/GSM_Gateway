// Global vectors to hold unsent message data
std::vector<String> unsentMessageIDs;
std::vector<String> unsentContents;
std::vector<std::vector<String>> unsentRecipients;

int currentMessageIndex = 0;  // Index to track which message is being sent

bool gsm1TaskCompleted = false;
bool gsm2TaskCompleted = false;

std::vector<String> gsm1Recipients, gsm2Recipients;
String gsm1MessageContents, gsm2MessageContents;


void waitForBothGSMs() {
  // Block until both tasks complete
  while (!gsm1TaskCompleted || !gsm2TaskCompleted) {
    delay(100);  // Poll every 100ms
  }
  // Reset the task completion flags for the next message
  gsm1TaskCompleted = false;
  gsm2TaskCompleted = false;
}


// Task function for GSM1
void sendGSM1Task(void *pvParameters) {
  for (String recipient : gsm1Recipients) {
    sendMessage_gsm1(recipient, gsm1MessageContents);
    delay(2000);  // Simulate sending delay
  }

  gsm1TaskCompleted = true;
  vTaskDelete(NULL);  // Delete the task once done
}

// Task function for GSM2
void sendGSM2Task(void *pvParameters) {
  for (String recipient : gsm2Recipients) {
    sendMessage_gsm2(recipient, gsm2MessageContents);
    delay(2000);  // Simulate sending delay
  }

  gsm2TaskCompleted = true;
  vTaskDelete(NULL);  // Delete the task once done
}

std::pair<std::vector<String>, std::vector<String>> splitRecipientsForGSMs(const std::vector<String>& recipients) {
  std::vector<String> gsm1Recipients, gsm2Recipients;

  for (const String& recipient : recipients) {
    // Check if the recipient belongs to MTN or Airtel
    if (recipient.startsWith("77") || recipient.startsWith("78") || recipient.startsWith("76")) {
      // MTN numbers go to GSM1
      gsm1Recipients.push_back(recipient);
    } else if (recipient.startsWith("74") || recipient.startsWith("75") || recipient.startsWith("70")) {
      // Airtel numbers go to GSM2
      gsm2Recipients.push_back(recipient);
    } else {
      // For any other network, you can choose how to handle it, e.g., default to GSM1
      Serial.println("Unknown network for recipient: " + recipient + ", defaulting to GSM1.");
      gsm1Recipients.push_back(recipient);
    }
  }

  return {gsm1Recipients, gsm2Recipients};
}



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


//                                                 *********************
//                                           *************************************
//                                  **************************************************
// *********************************Sends A Message with its Spilt_Recipients For sending ******************************************
//                                  **************************************************
//                                         *************************************
//                                                *********************
void sendToBothGSMs(String messageID, String contents, std::vector<String> recipients) {
  auto [gsm1, gsm2] = splitRecipientsForGSMs(recipients);

  // Store the recipients and contents for each GSM
  gsm1Recipients = gsm1;
  gsm2Recipients = gsm2;
  gsm1MessageContents = contents;
  gsm2MessageContents = contents;

  tasksInProgress = true;

  // Create the tasks for sending to GSM1 and GSM2 concurrently
  xTaskCreatePinnedToCore(sendGSM1Task, "SendToGSM1", 4096, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(sendGSM2Task, "SendToGSM2", 4096, NULL, 2, NULL, 1);

  waitForBothGSMs();

  // Mark the message as sent
  markMessageAsSent(messageID);
}


std::vector<String> splitRecipients(String recipientsList) {
  std::vector<String> result;
  int lastIndex = 0;
  int commaIndex = recipientsList.indexOf(',');

  while (commaIndex >= 0) {
    String recipient = recipientsList.substring(lastIndex, commaIndex);
    recipient.trim();  // Trim spaces
    result.push_back(recipient);  // Add recipient to vector
    lastIndex = commaIndex + 1;
    commaIndex = recipientsList.indexOf(',', lastIndex);
  }

  // Handle the last recipient
  String recipient = recipientsList.substring(lastIndex);
  recipient.trim();  // Trim spaces
  result.push_back(recipient);

  return result;
}



// Task function to send all queued unsent messages iteratively
void sendQueuedMessagesTask(void *pvParameters) {
  while (currentMessageIndex < unsentMessageIDs.size()) {
    String messageID = unsentMessageIDs[currentMessageIndex];
    String contents = unsentContents[currentMessageIndex];
    std::vector<String> recipients = unsentRecipients[currentMessageIndex];

    Serial.println("Sending message ID: " + messageID);
    
    // Send the message to both GSMs
    sendToBothGSMs(messageID, contents, recipients);

    // Mark the message as sent
    markMessageAsSent(messageID);

    // Increment to send the next message
    currentMessageIndex++;

    // Wait 5 seconds before sending the next message (simulated delay)
    vTaskDelay(5000 / portTICK_PERIOD_MS);  // 5-second delay between sending messages
  }

  // Once all messages are sent, clean up and delete the task
  Serial.println("All queued messages sent.");
  currentMessageIndex = 0;  // Reset index for next batch
  unsentMessageIDs.clear();  // Clear message queue
  unsentContents.clear();
  unsentRecipients.clear();

  tasksInProgress = false;
  
  vTaskDelete(NULL);  // Delete the task once done
}

void fetchAndSendMessages() {
  HTTPClient http;
  String response;
  String url = JSON_URL;

  http.begin(url);
  int httpCode = http.GET();

  if (httpCode > 0) {
    response = http.getString();
    Serial.println(response);

    // Create a JSON document
    StaticJsonDocument<4096> doc;
    DeserializationError error = deserializeJson(doc, response);

    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    // Extract SMSmessages array
    JsonArray smsMessages = doc["SMSmessages"].as<JsonArray>();

    // Iterate through SMS messages
    for (JsonObject sms : smsMessages) {
      String messageID = sms["messageID"].as<String>();
      String contents = sms["contents"].as<String>();
      String recipientsList = sms["recipients"].as<String>();

      // Split the recipients list into a vector
      std::vector<String> recipients = splitRecipients(recipientsList);

      // Check if the message has already been sent
      if (!isMessageSent(messageID)) {
        // Queue the unsent messages for sending
        unsentMessageIDs.push_back(messageID);
        unsentContents.push_back(contents);
        unsentRecipients.push_back(recipients);
      } else {
        Serial.println("Message already sent: " + messageID);
      }
    }

    // If there are unsent messages, create a task to send them
    if (!unsentMessageIDs.empty()) {
      xTaskCreatePinnedToCore(sendQueuedMessagesTask, "SendQueuedMessages", 8192, NULL, 2, NULL, 1);
    }
  }

  http.end();
}
