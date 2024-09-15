#include <ESP8266WiFi.h>
// #include <ESP8266Ping.h>
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
HTTPClient http;
WiFiClient wifiClient;  


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

// google sheet ID
const char *GScriptId = "AKfycbxuC20FX9Vs9_qDJfiBYI5CWc-vaAS8vUlg-1BdJdi8Rle-Kk6seZqbGyKmDvj0hAPY7g";


String payload = "";

// Google Sheets setup (do not edit)
const char* host = "script.google.com";
const int httpsPort = 443;
const char* fingerprint = "";
String url = String("/macros/s/") + GScriptId + "/exec";
HTTPSRedirect* client = nullptr;

//AdresseIPFixe
IPAddress IP(192, 168, 1, 200);       //adresse fixe
IPAddress gateway(192, 168, 100, 1);  //passerelle par défaut // 192.168.100.1   //192.168.1.1
IPAddress subnet(255, 255, 255, 0);  //masque de sous-réseau
IPAddress dns(8, 8, 8, 8);           //DNS

// Admin information
const String username = "youssao";
const String password = "1234567890";

// Instance of client
String first_name = "";
String last_name = "";
String department = "";

String send_time = "";
String send_date = "";
int time_minute;
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
int ye = 0;
int mo = 0;
int da = 0;
int ho = 0;
int mi = 0;
// SETUP
void setup() {
    Serial.begin(115200);
    Serial.println("System On");
    delay(1000);
    Serial.println("Loading........");
    delay(2000);
    // buzzer initialization  
    WiFi.config(IP, gateway, subnet, dns);
    
    while(!is_connected){
      switch (number_connect){
      case 0:
        wifi_connection("USER_0BD0F6","s2PAhtaL");
        break;
      
      default :
        Serial.println("Impossible de se connecter à l'un des WiFi disponible");
        break;
      }
    }

    // Initialize RFID
    rfid_init();

    // Initialize LCD
    Wire.begin();
    lcd.begin(16,2);
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Initializing...");
    delay(1000);
    
    // Initialize RTC
    if (!rtc.begin()) {
        Serial.println("Couldn't find RTC");
        lcd.setCursor(0, 1);
        lcd.print("RTC Not Found!");
        while (1) yield();  // Avoid WDT reset
    }
    lcd.clear();
    rtc.adjust(DateTime(__DATE__, __TIME__));  // Set RTC to compile time
    // rtc.adjust(DateTime(ye, mo, da, ho, mi, 0)); 

    
    
    
    // SETUP SERVER ROUTES
    server.on("/", HTTP_GET, handle_login);
    server.on("/login", HTTP_POST, handle_login_form);
    server.on("/admin", HTTP_GET, handle_admin);
    server.on("/add_user", HTTP_GET, handle_add_user);
    server.on("/add_user", HTTP_POST, handle_add_user_form);
    server.on("/handle_delete",HTTP_GET,handle_setting);
    server.on("/delete_user",HTTP_GET,handle_delete_user);
    server.on("/set_mode", HTTP_GET, handle_set_mode);
    server.on("/set_date_time", HTTP_GET, handle_set_date_time);
    server.on("/system", HTTP_GET,handle_system);
    // server.on("history", HTTPGET, handle_redirect);

    
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
   
}
String last_card = "";

String show_mode = "";
void loop() { // START OF LOOP FUNCTION ////////////////////////////////////////////////////////////////////////////////////////
    static unsigned long lastUpdate = 0;
    unsigned long currentMillis = millis();
    DateTime now = rtc.now();
    if (currentMillis - lastUpdate >= 1000) {
        lastUpdate = currentMillis;
        lcd.setCursor(0, 0);
        lcd.print(now.year(), DEC);
        lcd.print("/");
        printTwoDigits(now.month());
        lcd.print("/");
        printTwoDigits(now.day());
        lcd.print(",");
        send_date = (now.day() < 10 ? "0" : "") + String(now.day()) + "/" + (now.month() < 10 ? "0" : "") + String(now.month(),DEC) + "/" + String(now.year(),DEC);
        
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
        send_time = (now.hour() < 10 ? "0" : "") + String(now.hour()) + ":" + (now.minute() < 10 ? "0" : "") + String(now.minute());
        time_minute = now.hour() * 60 + now.minute();
        Serial.println(time_minute);
        printTwoDigits(now.second());
        Serial.println();
    }
    server.handleClient();
    lcd.setCursor(0,1);
    
    lcd.print(show_mode);
    // http.begin(wifiClient, "http://www.google.com"); 
    // int httpCode = http.GET();
    // if(send_date == "20/06/2000"){
      
    //   // reset_action("/store.txt");
    // }
    // httpCode > 0
    if ( WiFi.status() == WL_CONNECTED && (time_minute < 510 || time_minute >= 570)){
      readAndDeleteFirstLine("/store.txt");
    }
    entered_exit_rfid();
}
 // END OF LOOP FUNCTION////////////////////////////////////////////////////////////////////////////////////

void rfid_init(){
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("RFID Initialization Successful");
}

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
      lcd.print("Traitement...");
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
      lcd.print("   Temps");
      lcd.setCursor(0, 1);
      lcd.print("   ecoule");
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
    
    send_data("Deleted Users",user_id,deleted_firstname,"None",send_time, send_date);
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

void handle_set_date_time(){
  String date = server.arg("date");
  String year_str = date.substring(0,4);
  ye = year_str.toInt();
  String month_str = date.substring(5,7);
  int mo = month_str.toInt();
  String day_str = date.substring(8,10);
  int da = day_str.toInt();
  Serial.println(da);
  String time = server.arg("time");
  String hour_str = time.substring(0,2);
  int ho = hour_str.toInt();
  String minute_str = time.substring(3,5);
  int mi = minute_str.toInt();
  if(ye == 2000 && mo == 6 && da == 20){
    reset_action("/info.txt");
    reset_action("/cards.txt");
    reset_action("/enter_exit.txt");
  }else{
    rtc.adjust(DateTime(ye, mo, da, ho, mi, 25));
  }
  
  
  // Serial.println(date + " " + time);
}


void reset_action(String path){
  if (LittleFS.exists(path)) {
    // Open the file in write mode, which will overwrite the existing file
    File file = LittleFS.open(path, "w");
    if (file) {
      Serial.println("File opened successfully.");
      file.close();  // Closing it immediately after opening will erase the contents
      Serial.println("File contents deleted.");
    } else {
      Serial.println("Failed to open file.");
    }
  } else {
    Serial.println("File does not exist.");
  }
  
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








  