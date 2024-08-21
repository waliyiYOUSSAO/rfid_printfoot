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
    String rfid_string = "";
    while (rfid_string == "") {
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

