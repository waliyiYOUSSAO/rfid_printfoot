#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include "clock_function.h"
#include <Wire.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <EEPROM.h>
//#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>

ESP8266WebServer server(80); // Web server port

//const char* host = "script.google.com";
//const int httpsPort = 443;
//WiFiClientSecure client; // Create a WiFiClientSecure object

//String GAS_ID = "1pELYc44bfWVblF8f6LtszbhUTUAy3o0vW4oStyrRVNXPljfa7mlbZyxK"; // Spreadsheet script ID

const char *serverName = "http://script.google.com/macros/s/AKfycby9VfafiIHq1j5y3Izfzs2cIS0Q9TZk-UQC5bERgSOMpL6fy28ix8GMt5gVi6a_EkRxPg/exec";


// RFID pins
const uint8_t SS_PIN = 15;   // D8 = 15
const uint8_t RST_PIN = -1;  // Not used

// LCD pins
const uint8_t SDA_PIN = D2;
const uint8_t SCL_PIN = D3;

// Prototypes
String rfid_read();

// Creation of instances
Clock_ clock_;
MFRC522 rfid(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);

//AdresseIPFixe
// IPAddress IP(192, 168, 1, 20);       //adresse fixe
// IPAddress gateway(192, 168, 1, 14);  //passerelle par défaut
// IPAddress subnet(255, 255, 255, 0);  //masque de sous-réseau
// IPAddress dns(8, 8, 8, 8);           //DNS

// Admin information
const String username = "youssao";
const String password = "1234567890";

// Instance of client
String first_name = "";
String last_name = "";
String department = "";

void setup() {
    Serial.begin(115200);

    //WiFi.config(IP, gateway, subnet, dns);

    // Initialize Clock
    clock_init();

    // Initialize RFID
    rfid_init();

    // Initialize LCD
    lcd_init();

    // Initialize LittleFS
    if (!LittleFS.begin()) {
        Serial.println("Failed to mount file system");
        return;
    }

    // Setup server routes
    server.on("/", HTTP_GET, handle_login);
    server.on("/login", HTTP_POST, handle_login_form);
    server.on("/admin", HTTP_GET, handle_admin);
    server.on("/add_user", HTTP_GET, handle_add_user);
    server.on("/add_user", HTTP_POST, handle_add_user_form);
    
    // Start the server
    server.begin();
    Serial.println("Server started");
}
String last_card = "";
void loop() {
    server.handleClient();
    lcd.setCursor(0, 0);
    lcd.print(clock_.show_date());
    entered_exit_rfid();
    

    
}

void handle_login() {
    serve_file("/login.html");
}

void handle_admin() {
    serve_file("/admin.html");
}

 void handle_add_user() {
    serve_file("/add_user.html");
}


void handle_add_user_form() {
    first_name = server.arg("first-name");
    last_name = server.arg("last-name");
    department = server.arg("department");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Rapprochez");
    lcd.setCursor(0,1);
    lcd.print("La carte");
    
    String rfid_string = wait_for_rfid();

    // Display user information with RFID ID
    if(rfid_string != ""){
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.clear();
      lcd.print("Vous pouvez");
      lcd.setCursor(0, 1);
      lcd.print("retiré la carte");
      delay(2000);
      lcd.clear();
    }
    

    // Print to Serial
    Serial.println("Received user data:");
    Serial.println(first_name);
    Serial.println(last_name);
    Serial.println(department);
    Serial.println(rfid_string);
    // save informations in txt file
    save_card_info(rfid_string,first_name,last_name,department);
    send_data(first_name, last_name, department, rfid_string);
    server.sendHeader("Location", "/admin",true);
    server.send(302,"text/plain","");
    // Reset temporary variablesp
    first_name = "";
    last_name = "";
    department = "";
  
  
}
    

void entered_exit_rfid(){
  String card = rfid_read();
  String first_name_display = "";
  if(last_card != card){
    bool verification = unicity("/info.txt",card);
  if(verification){
    File file = LittleFS.open("/info.txt","r");
    if (!file){
      Serial.println("Failed to open file");
      return;
    }
    else{
      while (file.available()) {
        String line = file.readStringUntil('\n');
        int first_comma = line.indexOf(',');
        int second_comma = line.indexOf(',',first_comma + 1);
        String stored_id = line.substring(0,first_comma);
        if(card.equals(stored_id)){
          file.close();
          first_name_display = line.substring(first_comma + 1, second_comma);
          Serial.println("ACCES AUTORISE");
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(first_name_display);
          lcd.setCursor(0, 1);
          lcd.print(" ACCES AUTORISE");
          delay(2000);
          lcd.clear();
        }
      }
        file.close();
    }
    
  }
  else{
    Serial.println("ACCES REFUSE");
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print(" ACCES REFUSE");
    delay(2000);
    lcd.clear();
  }

  }
}
// int in_ou_out(){
  

// }
