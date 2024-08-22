// all functions

String rfid_read(){ // function to read card
  String rfid_id = "";

    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
        for (byte i = 0; i < rfid.uid.size; i++) {
            rfid_id += rfid.uid.uidByte[i] < 0x10 ? "0" : "";
            rfid_id += String(rfid.uid.uidByte[i], HEX);
        }
        rfid_id.toUpperCase(); // Convert to uppercase for consistency
        
            }
    return rfid_id;
}

void serve_file(const char * path){ // function to serve file
  File file = LittleFS.open(path,"r");
  if(!file){
    server.send(404,"text/plain","Fichier non trouvé");
    return;
  }
  if(server.streamFile(file,"text/html") != file.size()){
    Serial.println("echec d'envoie du fichier");
  }
  file.close();
}



void handle_login_form() {
    String username_arg = server.arg("username");
    String password_arg = server.arg("password");

    if (username_arg == username && password_arg == password) {
        server.sendHeader("Location", "/admin");
        server.send(303);
    } else {
        server.send(401, "text/html", "<h1>Access Denied</h1><p>Invalid password</p>");
    }
}


String wait_for_rfid() {
  static unsigned long time_0 = 0; // Initialisation du temps
    time_0 = millis();
    String rfid_string = "";
    while (rfid_string == "" && (millis() - time_0 <= 5000)) {
        rfid_string = rfid_read();
        delay(500); // Wait for half a second to prevent flooding the RFID read
    }
    return rfid_string;
}


void send_data(String first_name, String last_name, String department, String rfid_string) {
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;                 // Use HTTPClient
      WiFiClient client;               // Create a WiFiClient object
      http.begin(client, serverName);  // Use begin with WiFiClient and URL
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");

      //*********************************Construire la chaîne de requête avec les informations supplémentaires//
      String httpRequestData = "uid=" + rfid_string + "&firstName=" + first_name + "&lastName=" + last_name + "&department=" + department;

      int httpResponseCode = http.POST(httpRequestData);

      if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println("Response: " + response);
      } else {
        Serial.println("Error code: " + String(httpResponseCode));
      }

      http.end();
    } else {
      Serial.println("Error in WiFi connection");
    }
}

void save_card_info(String card_id, String first_name, String last_name, String department){
  bool test_unicity = unicity("/info.txt", card_id);
  if(!test_unicity){
    File file = LittleFS.open("/info.txt", "a");
      if (!file) {
          Serial.println("Failed to open file for writing");
          return;
      }

    String user_info = first_name + "," + last_name + "," + department;
    file.println(card_id + "," + user_info);
    file.close();
  //  lcd.print("User Registered");
    Serial.println("Utilisateur Ajouté");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.clear();
    lcd.print("Utilisateur");
    lcd.setCursor(0, 1);
    lcd.print("   Ajouté");
    delay(3000);
    lcd.clear();
    readFile("/info.txt");
    //readFile("/cards.txt");
  }
  else{
    Serial.println("Utilisateur existant");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.clear();
    lcd.print("Utilisateur");
    lcd.setCursor(0, 1);
    lcd.print("   Existant");
    delay(3000);
    lcd.clear();

  }
}

void readFile(String path) {
    File file = LittleFS.open(path, "r");
    if (!file) {
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.println("Employee Registered List:");
    while (file.available()) {
        String line = file.readStringUntil('\n');
        Serial.println(line);
    }

    file.close();
}

bool unicity(String path,String card_id){

  File file = LittleFS.open(path,"r");
  
  if(!file){
    Serial.println("Failed to open file for reading");
    return false;
  }
  else{  
    while(file.available()){
      String line = file.readStringUntil('\n');
      int commaIndex = line.indexOf(',');
      if(commaIndex != -1){
        String only_id = line.substring(0,commaIndex);
        if(card_id.equals(only_id)){
          file.close();
          return true;
        }
      }

    }
  file.close();
  return false;
  }
  
}


String name_after_verification(String path, String card){

  File file = LittleFS.open(path, "r");
  if (!file){
    Serial.println("Failed to open file");
    return "";
  }
  else{
    while (file.available()) {
      String line = file.readStringUntil('\n');
      int first_comma = line.indexOf(',');
      int second_comma = line.indexOf(',',first_comma + 1);
      String stored_id = line.substring(0,first_comma);
      if(card.equals(stored_id)){
        file.close();
        String first_name_display = line.substring(first_comma + 1, second_comma);
        return first_name_display;
      }
    }
      file.close();
  }
    return "";
  
}
int in_or_out(String in_out_id, String path) {
    // Check if it's the first time this ID is being processed
    bool first_time = unicity(path, in_out_id);

    if (!first_time) {
        Serial.println("First time ID detected, initializing entry.");

        // Initialize entry for this ID with a value of 0 (ARRIVE)
        number_int = 0;

        // Open the file to append the new ID entry
        File file = LittleFS.open(path, "a");
        if (!file) {
            Serial.println("Failed to open file for writing");
            return -1;
        }

        // Write the new entry to the file
        file.println(in_out_id + "," + String(number_int));
        file.close();

    } else {
        Serial.println("Existing ID detected, checking current state.");

        // If the ID is found, retrieve its current number_in_out value
        String number_string = name_after_verification(path, in_out_id);
        number_int = number_string.toInt();
        Serial.print("Current number_in_out value: ");
        Serial.println(number_int);

        // Increment the in/out counter
        number_int++;

        // Open the file to read and update its contents
        File file = LittleFS.open(path, "r");
        if (!file) {
            Serial.println("Failed to open file for reading");
            return -1;
        }

        String file_content = "";
        bool found = false;

        // Read the file line by line
        while (file.available()) {
            String line = file.readStringUntil('\n');

            // Check if this line contains the ID we're looking for
            if (line.startsWith(in_out_id + ",")) {
                found = true;
                // Split the line and update the specific field
                int commaIndex = line.indexOf(',');
                if (commaIndex != -1) {
                    String name_in_out = line.substring(0, commaIndex);
                    // Update the number associated with the ID
                    line = name_in_out + "," + String(number_int);
                }
            }

            // Append the line to the updated file content
            file_content += line + "\n";
        }
        file.close();

        if (found) {
            // Reopen the file for writing and overwrite it with the updated content
            file = LittleFS.open(path, "w");
            if (!file) {
                Serial.println("Failed to open file for writing");
                return -1;
            }
            file.print(file_content);
            file.close();
        } else {
            Serial.println("ID not found in file.");
        }
    }

    // Return the current in/out value (after incrementing)
    Serial.print("Returning number_in_out value: ");
    Serial.println(number_int);
    return number_int;
}

void display_some(String sentence_1, String sentence_2){ // display somethings
      
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("     "+sentence_1);
        lcd.setCursor(0, 1);
        lcd.print(sentence_2);
        delay(2000);
        lcd.clear();
}

