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
//String rfid_read();

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
int in_and_out = 0;
int number_int = 0;
void setup() {
    Serial.begin(115200);

    //WiFi.config(IP, gateway, subnet, dns);

    // Initialize Clock
    clock_.Begin();
    clock_.init_WiFi("USER_0BD0F6", "s2PAhtaL"); // Your Wi-Fi credentials // USER_0BD0F6 s2PAhtaL   HUAWEI-2.4G-c9bh
    clock_.sync_rtc();

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
    lcd.print("   La carte");
    
    String rfid_string = wait_for_rfid();
    
    if(rfid_string != ""){
      
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.clear();
      lcd.print("  VOUS POUVEZ");
      lcd.setCursor(0, 1);
      lcd.print("RETIRE LA CARTE");
      delay(2000);
      lcd.clear();
      save_card_info(rfid_string,first_name,last_name,department);
    }
    else{
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.clear();
      lcd.print("   TEMPS");
      lcd.setCursor(0, 1);
      lcd.print("  ECOULE");
      delay(2000);
      lcd.clear();
    }
    

    // Display user information with RFID ID
    
    

    // Print to Serial
    Serial.println("Received user data:");
    Serial.println(first_name);
    Serial.println(last_name);
    Serial.println(department);
    Serial.println(rfid_string);
    // save informations in txt file

    
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
    String name_returned = name_after_verification("/info.txt",card);
    Serial.println("ACCES AUTORISE");
    int number_in_out = in_or_out(card, "/enter_exit.txt");
    String enter_or_exit = (number_in_out % 2 == 0 ) ? "   ARRIVE": "   DEPART";
    display_some(name_returned, enter_or_exit);
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
