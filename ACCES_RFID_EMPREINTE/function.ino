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


void send_data(String rfid_string, String first_name, String last_name, String department, String statut){
  static bool flag = false;
  if (!flag){
    client = new HTTPSRedirect(httpsPort);
    client->setInsecure();
    flag = true;
    client->setPrintResponseBody(true);
    client->setContentTypeHeader("application/json");
  }
  if (client != nullptr){
    if (!client->connected()){
      client->connect(host, httpsPort);
    }
  }
  else{
    Serial.println("Error creating client object!");
  }
  
  // Create json object string to send to Google Sheets
  payload = payload_base + "\"" + rfid_string + "," + first_name + "," + last_name_sheet + "," + department_sheet + "," + statut + "\"}";
  
  // Publish data to Google Sheets
  Serial.println("Publishing data...");
  Serial.println(payload);
  if(client->POST(url, host, payload)){ 
    last_name_sheet = "";
    department_sheet = "";
  }
  else{
    // do stuff here if publish was not successful
    Serial.println("Error while connecting");
  }

  // a delay of several seconds is required before publishing again    
  delay(1000);
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
// Global or class-level variables


void send_data(String rfid_string, String first_name, String statut) {
  static bool flag = false;
  if (!flag) {
    client = new HTTPSRedirect(httpsPort);
    client->setInsecure();
    flag = true;
    client->setPrintResponseBody(true);
    client->setContentTypeHeader("application/json");
  }
  
  if (client != nullptr) {
    if (!client->connected()) {
      client->connect(host, httpsPort);
    }
  } else {
    Serial.println("Error creating client object!");
  }
  
  Last_name_department_sheet lastname_department = extract_last_name_and_department("/info.txt",rfid_string);
  // Create json object string to send to Google Sheets
  String payload = payload_base + "\"" + rfid_string + "," + first_name + "," + lastname_department.last_name_sheet + "," + lastname_department.department_sheet + "," + statut + "\"}";
  
  // Publish data to Google Sheets
  Serial.println("Publishing data...");
  Serial.println(payload);
  
  if (client->POST(url, host, payload)) { 
    
  } else {
    // Do stuff here if publish was not successful
    Serial.println("Error while connecting");
  }

  // A delay of several seconds is required before publishing again    
  //delay(1000);
}

String name_after_verification(String path, String card) {
  File file = LittleFS.open(path, "r");
  if (!file) {
    Serial.println("Failed to open file");
    return "";
  } else {
    while (file.available()) {
      String line = file.readStringUntil('\n');
      int first_comma = line.indexOf(',');
      int second_comma = line.indexOf(',', first_comma + 1);
      int last_comma = line.lastIndexOf(',');
      String stored_id = line.substring(0, first_comma);
      Serial.println(stored_id);
      
      // Assign to global variables
      
      if (card.equals(stored_id)) {
        
        String first_name_display = line.substring(first_comma + 1, second_comma);
        extract_last_name_and_department("/info.txt",card);
        file.close();
        return first_name_display;
      }
    }
    file.close();
  }
  return "";
}

Last_name_department_sheet extract_last_name_and_department(String path, String card) {
  Last_name_department_sheet last_name_department = {"",""};

  File file = LittleFS.open(path, "r");
  if (!file) {
    Serial.println("Failed to open file");
    return last_name_department;
  } else {
    while (file.available()) {
      String line = file.readStringUntil('\n');
      int first_comma = line.indexOf(',');
      String stored_id = line.substring(0, first_comma);
      
      if (card.equals(stored_id)) {
        int second_comma = line.indexOf(',', first_comma +1);
        int third_comma = line.indexOf(',', second_comma + 1);  // Find the position of the third comma
        //third_comma = line.indexOf(',', third_comma + 1); // Move to the third comma
        int last_comma = line.lastIndexOf(',');

        // Extract last name and department
        String last_name = line.substring(second_comma + 1, third_comma);
        String department = line.substring(last_comma + 1);
        
        //        // Sanitize the extracted strings to avoid JSON errors
        last_name.replace("\"", "\\\"");
        department.replace("\"", "\\\"");
        department.replace("\n", ""); // Remove newlines
        department.replace("\r", ""); // Remove carriage returns
        // Print the last name and department
        Serial.print("Last Name: ");
        Serial.println(last_name);
        Serial.print("Department: ");
        Serial.println(department);
        last_name_department.last_name_sheet = last_name;
        last_name_department.department_sheet = department;
        file.close();
        return last_name_department;  // Return a string containing both if needed
      }
    }
    file.close();
  }
  return last_name_department;
}

void deleteFile(String path) {
    if (LittleFS.exists(path)) {  // Check if the file exists
        if (LittleFS.remove(path)) {  // Attempt to delete the file
            Serial.println("File deleted successfully");
        } else {
            Serial.println("Failed to delete the file");
        }
    } else {
        Serial.println("File does not exist");
    }
}


void entered_exit_rfid(){
  String card = rfid_read();
  String first_name_display = "";
  if(last_card != card){
    bool verification = unicity("/info.txt",card);
  if(verification){
    String name_returned = name_after_verification("/info.txt",card);

    Serial.println("ACCES AUTORISE");
    int number_in_out = in_or_out(card, "/enter_exit.txt");
    String enter_or_exit = (number_in_out % 2 == 0 ) ? "   ARRIVE": "   DEPART";
    display_some(name_returned, enter_or_exit);
    send_data(card, name_returned, enter_or_exit);
    
    
  }
  else{
    Serial.println("ACCES REFUSE");
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print(" ACCES REFUSE");
    delay(2000);
    // digitalWrite(buzzer_pin, LOW);
    // delay(500);
    // Serial.println("piiiiiiii");
    // digitalWrite(buzzer_pin,LOW);
    // delay(500);
    lcd.clear();
  }

  }
}
