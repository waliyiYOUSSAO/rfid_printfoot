#include <ESP8266WiFi.h>
#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
// #include "clock_function.h"
// #include <Wire.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include "HTTPSRedirect.h"
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <RTClib.h>

ESP8266WebServer server(80); // Web server port



//const char *serverName = "http://script.google.com/macros/s/AKfycby9VfafiIHq1j5y3Izfzs2cIS0Q9TZk-UQC5bERgSOMpL6fy28ix8GMt5gVi6a_EkRxPg/exec";

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

MFRC522 rfid(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);
RTC_DS3231 rtc;


const char *GScriptId = "AKfycbxJGJGBfDyXlDvqs6OPc2cost_l916VmRwPUW46wIEPR3gLqQ_LAtQYU--hs_yu8EEa";

// Enter command (insert_row or append_row) and your Google Sheets sheet name (default is Sheet1):
//String payload_base =  "{\"command\": \"insert_row\", \"sheet_name\":\"" + sheet_name +, \"values\": ";
String payload = "";

// Google Sheets setup (do not edit)
const char* host = "script.google.com";
const int httpsPort = 443;
const char* fingerprint = "";
String url = String("/macros/s/") + GScriptId + "/exec";
HTTPSRedirect* client = nullptr;

//AdresseIPFixe
IPAddress IP(192, 168, 1, 200);       //adresse fixe
IPAddress gateway(192, 168, 1, 1);  //passerelle par défaut // 192.168.100.1   //192.168.1.1
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

struct All_info {
  String id;
  String firstname;
  String lastname;
  String department;
};

int in_and_out = 0;
int number_int = 0;
bool is_connected = false;
int number_connect = 0;

// SETUP
void setup() {
    Serial.begin(115200);
    //buzzer initialization  
    // WiFi.config(IP, gateway, subnet, dns);

        
    
    while(!is_connected){
      switch (number_connect){
      case 0:
        wifi_connection("USER_0BD0F6","s2PAhtaL");
        break;
      case 1:
        wifi_connection("Youss","12345678");
        break;
      case 2:
        wifi_connection("youss_pc","youss220605");
        break;
      case 3:
        wifi_connection("HUAWEI-2.4G-c9bh","albako@2013");
        break;        
      default :
        Serial.println("Impossible de se connecter à l'un des WiFi disponible");
        break;
      }
    }

    // Initialize RFID
    rfid_init();

    // Initialize LCD

    lcd.begin(16,2);
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Initializing...");
    
    // Initialize RTC
    if (!rtc.begin()) {
        Serial.println("Couldn't find RTC");
        lcd.setCursor(0, 1);
        lcd.print("RTC Not Found!");
        while (1) yield();  // Avoid WDT reset
    }
    rtc.adjust(DateTime(2024, 8, 31, 23, 59, 0)); 
    
    // SETUP SERVER ROUTES
    server.on("/", HTTP_GET, handle_login);
    server.on("/login", HTTP_POST, handle_login_form);
    server.on("/admin", HTTP_GET, handle_admin);
    server.on("/add_user", HTTP_GET, handle_add_user);
    server.on("/add_user", HTTP_POST, handle_add_user_form);
    server.on("/handle_delete",HTTP_GET,handle_setting);
    server.on("/delete_user",HTTP_GET,handle_delete_user);
    server.on("/set_mode", HTTP_GET, handle_set_mode);
    server.on("/system", HTTP_GET,handle_system);
    
    // END SETUP SERVER ROUTES
    
    // Start the server
    server.begin();
    Serial.println("Server started");
    if (!LittleFS.begin()) {
        Serial.println("Failed to mount file system");
        return;
    }
    /////////////////////////////////////////////////////////
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


  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    lcd.setCursor(0, 1);
    lcd.print("RTC Not Found!");
    // while (1) yield();  // Avoid WDT reset
  }
    

    
}
String last_card = "";

String show_mode = "";
void loop() { // START OF LOOP FUNCTION ////////////////////////////////////////////////////////////////////////////////////////
    static unsigned long lastUpdate = 0;
    unsigned long currentMillis = millis();

    if (currentMillis - lastUpdate >= 1000) {
        lastUpdate = currentMillis;

        DateTime now = rtc.now();
        lcd.setCursor(0, 0);
        lcd.print(now.year(), DEC);
        lcd.print("/");
        printTwoDigits(now.month());
        lcd.print("/");
        printTwoDigits(now.day());
        lcd.print(",");
        printTwoDigits(now.hour());
        lcd.print(":");
        printTwoDigits(now.minute());
        lcd.print(":");
        printTwoDigits(now.second());

        Serial.print("Current Date: ");
        Serial.print(now.year(), DEC);
        Serial.print("/");
        printTwoDigits(now.month());
        Serial.print("/");
        printTwoDigits(now.day());
        Serial.print("  Current Time: ");
        printTwoDigits(now.hour());
        Serial.print(":");
        printTwoDigits(now.minute());
        Serial.print(":");
        printTwoDigits(now.second());
        Serial.println();
    }

    server.handleClient();
    lcd.setCursor(0,1);
    
    lcd.print(show_mode);
    entered_exit_rfid();
}
 // END OF LOOP FUNCTION////////////////////////////////////////////////////////////////////////////////////



void wifi_connection(String ssid, String password){

      static unsigned long time_connect = 0;
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid, password);
      Serial.print("Connexion au WiFi...");
      time_connect = millis();
      while (WiFi.status() != WL_CONNECTED && (millis() - time_connect) <= 10000) {
        Serial.print('.');
        delay(500);
      }
      if(WiFi.status() == WL_CONNECTED){
        is_connected = true;
        Serial.println(WiFi.localIP());
      }
      else{
        Serial.println("Impssible de se connecter à: " + ssid);
        number_connect ++;
      }
      return ;
}
void handle_login() {
    serve_file("/login.html");
}

void handle_admin() {
    //serve_file("/admin.html");
  String admin_content = read_html("/admin.html");
  int number_employe = count_employe("/info.txt");
  admin_content.replace("{{NBR}}", String (number_employe));
  server.send(200,"text/html",admin_content);
  
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
      lcd.print("TRAITEMENT...");
      lcd.setCursor(0, 1);
      delay(2000);
      lcd.clear();
      save_card_info(rfid_string, first_name, last_name, department);

      // send information in registered user sheet
    }
    else{
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.clear();
      lcd.print("   TEMPS");
      lcd.setCursor(0, 1);
      lcd.print("   ECOULE");
      delay(2000);
      lcd.clear();
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
    // save_card_info(rfid_string, first_name, last_name, department);
    // Reset temporary variablesp
    first_name = "";
    last_name = "";
    department = "";
  
  
}
    
void handle_setting() {
    String setting_content = read_html("/delete_user.html");
    String all_user_list = "";

    if (LittleFS.exists("/info.txt")) {
        File info_file = LittleFS.open("/info.txt", "r");

        bool fileHasContent = false;

        while (info_file.available()) {
            String line = info_file.readStringUntil('\n');
            line.trim();
            if (line.length() > 0) {
                fileHasContent = true;
                int first_comma = line.indexOf(',');
                int second_comma = line.indexOf(',', first_comma + 1);
                int last_comma = line.lastIndexOf(',');

                String id = line.substring(0, first_comma);
                String firstname = line.substring(first_comma + 1, second_comma);
                String lastname = line.substring(second_comma + 1, last_comma);
                String department = line.substring(last_comma + 1);

                all_user_list += "<tr>";
                all_user_list += "<td>" + firstname + "</td>";
                all_user_list += "<td>" + lastname + "</td>";
                all_user_list += "<td>" + department + "</td>";
                all_user_list += "<td>";
                all_user_list += "<button class='edit-btn'>Edit</button> ";
                all_user_list += "<button class='delete-btn' onclick='deleteUser(\"" + id + "\")'>Delete</button>";
                all_user_list += "</td>";
                all_user_list += "</tr>";
            }
        }

        // If the file is empty or contains only blank lines, add a "None" row
        if (!fileHasContent) {
            all_user_list += "<tr>";
            all_user_list += "<td>None</td>";
            all_user_list += "<td>None</td>";
            all_user_list += "<td>None</td>";
            all_user_list += "<td>None</td>";
            all_user_list += "</tr>";
        }

        info_file.close();
    } else {
        // If the file does not exist, add a "None" row
        all_user_list += "<tr>";
        all_user_list += "<td>None</td>";
        all_user_list += "<td>None</td>";
        all_user_list += "<td>None</td>";
        all_user_list += "<td>None</td>";
        all_user_list += "</tr>";
    }

    setting_content.replace("{{USER_LIST}}", all_user_list);
    server.send(200, "text/html", setting_content);
}


void handle_delete_user(){
  if(server.hasArg("id")){
    String user_id = server.arg("id");
    String deleted_firstname = name_after_verification("/info.txt",user_id);
    send_data("Deleted Users",user_id,deleted_firstname,"None","00:00");
    File info_file = LittleFS.open("/info.txt","r");
    String file_content = "";
    // send information in deleted user sheet
    while(info_file.available()){
      String line = info_file.readStringUntil('\n');
      if(line.indexOf(user_id) == -1){
        file_content += line + "\n";
      }
    }
    info_file.close();

    info_file = LittleFS.open("/info.txt","w");
    info_file.print(file_content);
    info_file.close();
    //server.send(200, "text/html", );
    server.sendHeader("Location", "/admin",true);
    server.send(302,"text/html","");
    
  } else{
    server.send(400,"text/plain", "User ID not provided");
    
  }


}


int count_employe(String path){
  int employe_number = 0;
  File number_info = LittleFS.open(path,"r");
  
  if(!number_info){
    Serial.println("Impossible to open this file");
    return 0;
  }
  while(number_info.available()){
    String line = number_info.readStringUntil('\n');
    if(line.length() > 0){
      employe_number ++;
    }
  }
  number_info.close();
  return employe_number;
}

String read_html(String path){
  if (!LittleFS.exists(path)) {
        server.send(404, "text/plain", "File not found");
        return "";
    }

    File html_file = LittleFS.open(path, "r");
    String html_content = html_file.readString();
    html_file.close();

    return html_content;
}

void handle_system(){
  handle_html("/setting.html");
}

void handle_html(String path){
  File file = LittleFS.open(path, "r");
        if (file) {
            server.streamFile(file, "text/html");
            file.close();
        } else {
            server.send(404, "text/plain", "File not found");
        }
}

void handle_set_mode() {
  String mode = server.arg("mode");
  Serial.println(show_mode);
  switch (mode.toInt()){
    case 1:
      // lcd.clear();
      // lcd.setCursor(0, 1);
      show_mode = "MODE BADGE      ";
      break;
    case 2:
      // lcd.clear();
      // lcd.setCursor(0, 1);
      show_mode = "MODE EMPREINTE  ";
      break;
    case 3:
      // lcd.clear();
      // lcd.setCursor(0, 1);
      show_mode = "MODE DOUBLE     ";
      break;
    default:
      show_mode = "";
  }
  
}




void reset_action(){
  //
}
// All_info all_information(String path){
//   All_info all_info = {"","","",""};
//   File info_file = LittleFS.open(path, 'r');

//   if(!info_file){
//     Serial.println("Failed to open this file");
//     return all_info;
//   }

//   while(info_file.available()){
//     String line = info_file.readStringUntil('\n');
//     line.trim();
//     int first_comma = line.indexOf(',');
//     int second_comma = line.indexOf(',',first_comma + 1);
//     int third_comma = line.indexOf(',',second_comma + 1);
//     int last_comma = line.indexOf(',');

//     if(line.length() > 0){


//     }


    
//   }


// }








  