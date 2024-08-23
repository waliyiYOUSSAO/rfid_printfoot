#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include "clock_function.h"
#include <Wire.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <EEPROM.h>
#include "HTTPSRedirect.h"
#include <ESP8266HTTPClient.h>

ESP8266WebServer server(80); // Web server port



const char *serverName = "http://script.google.com/macros/s/AKfycby9VfafiIHq1j5y3Izfzs2cIS0Q9TZk-UQC5bERgSOMpL6fy28ix8GMt5gVi6a_EkRxPg/exec";

// buzzer


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

const char *GScriptId = "AKfycbw8kOAGiioS42-s64_pAc6hXwijAJp4vd3PXs0ygwW-hVluM7lqmuWnhzJ1QEq4pjlF";

// Enter command (insert_row or append_row) and your Google Sheets sheet name (default is Sheet1):
String payload_base =  "{\"command\": \"insert_row\", \"sheet_name\": \"data\", \"values\": ";
String payload = "";

// Google Sheets setup (do not edit)
const char* host = "script.google.com";
const int httpsPort = 443;
const char* fingerprint = "";
String url = String("/macros/s/") + GScriptId + "/exec";
HTTPSRedirect* client = nullptr;

//AdresseIPFixe
IPAddress IP(192, 168, 1, 21);       //adresse fixe
IPAddress gateway(192, 168, 1, 1);  //passerelle par défaut
IPAddress subnet(255, 255, 255, 0);  //masque de sous-réseau
IPAddress dns(8, 8, 8, 8);           //DNS

// Admin information
const String username = "youssao";
const String password = "1234567890";

// Instance of client
String first_name = "";
String last_name = "";
String department = "";
struct Last_name_department_sheet{
  String last_name_sheet;
String department_sheet;
};


int in_and_out = 0;
int number_int = 0;
void setup() {
    Serial.begin(115200);
    //buzzer initialization

  
    
    WiFi.config(IP, gateway, subnet, dns);

    // Initialize Clock
    clock_.Begin();
    clock_.init_WiFi("USER_0BD0F6", "s2PAhtaL"); // Your Wi-Fi credentials // USER_0BD0F6 s2PAhtaL   HUAWEI-2.4G-c9bh
    clock_.sync_rtc();

    // Initialize RFID
    rfid_init();

    // Initialize LCD
    lcd_init();

    client = new HTTPSRedirect(httpsPort);
    client->setInsecure();
    client->setPrintResponseBody(true);
    client->setContentTypeHeader("application/json");
    Serial.print("Connecting to ");
    Serial.println(host);
    bool flag = false;
  for (int i=0; i<5; i++){ 
    int retval = client->connect(host, httpsPort);
    if (retval == 1){
       flag = true;
       Serial.println("Connected");
       break;
    }
    else
      Serial.println("Connection failed. Retrying...");
  }
  if (!flag){
    Serial.print("Could not connect to server: ");
    Serial.println(host);
    return;
  }
  delete client;    // delete HTTPSRedirect object
  client = nullptr; // delete HTTPSRedirect object

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
    //server.on("/delete_user",HTPP_GET,delete_user);
    
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
      //lcd.clear();
    }
    server.sendHeader("Location", "/admin",true);
    server.send(302,"text/plain","");

    // Display user information with RFID ID
    
    // Print to Serial
    Serial.println("Received user data:");
    Serial.println(first_name);
    Serial.println(last_name);
    Serial.println(department);
    Serial.println(rfid_string);
    
    // Reset temporary variablesp
    first_name = "";
    last_name = "";
    department = "";
  
  
}
    
// void handle_admin() {
//   String html = "<html><body>";
//   html += "<h2>User List</h2>";
//   html += "<table border='1'>";
//   html += "<tr><th>First Name</th><th>Last Name</th><th>Department</th><th>Action</th></tr>";

//   File file = LittleFS.open("/info.txt", "r");
//   if (file) {
//     while (file.available()) {
//       String line = file.readStringUntil('\n');
//       int first_comma = line.indexOf(',');
//       int second_comma = line.indexOf(',', first_comma + 1);
//       int last_comma = line.lastIndexOf(',');

//       String id = line.substring(0, first_comma);
//       String first_name = line.substring(first_comma + 1, second_comma);
//       String last_name = line.substring(second_comma + 1, last_comma);
//       String department = line.substring(last_comma + 1);

//       html += "<tr>";
//       html += "<td>" + first_name + "</td>";
//       html += "<td>" + last_name + "</td>";
//       html += "<td>" + department + "</td>";
//       html += "<td><form action='/delete_user' method='POST'>";
//       html += "<input type='hidden' name='user_id' value='" + id + "'>";
//       html += "<button type='submit'>Delete</button>";
//       html += "</form></td>";
//       html += "</tr>";
//     }
//     file.close();
//   }

//   html += "</table></body></html>";
//   server.send(200, "text/html", html);
// }







