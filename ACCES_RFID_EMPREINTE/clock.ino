// #include <Wire.h>
#include <RTClib.h>

// RTC_DS3231 rtc;

void initializeSerial() {
  Serial.begin(115200);  // Initialize serial communication
}

void initializeWire() {
  Wire.begin(D4, D1);  // Set SDA to D2 and SCL to D1
  
}

void initializeRTC() {
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    lcd.setCursor(0, 1);
    lcd.print("RTC Not Found!");
    while (1) yield();  // Avoid WDT reset
    
  }
  rtc.adjust(DateTime(2024, 8, 31, 23, 59, 0));  // Example: August 31, 2024, 11:59:00 PM
}


    // Manually set the date and time
    // Year, Month, Day, Hour, Minute, Second
    


// void printDateTime() {
//   DateTime now = rtc.now();

//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print("Date: ");
//   lcd.print(now.year(), DEC);
//   lcd.print("/");
//   printTwoDigits(now.month());
//   lcd.print("/");
//   printTwoDigits(now.day());

//   lcd.setCursor(0, 1);
//   lcd.print("Time: ");
//   printTwoDigits(now.hour());
//   lcd.print(":");
//   printTwoDigits(now.minute());
//   lcd.print(":");
//   printTwoDigits(now.second());

//   Serial.print("Current Date: ");
//   Serial.print(now.year(), DEC);
//   Serial.print("/");
//   printTwoDigits(now.month());
//   Serial.print("/");
//   printTwoDigits(now.day());
//   Serial.print("  Current Time: ");
//   printTwoDigits(now.hour());
//   Serial.print(":");
//   printTwoDigits(now.minute());
//   Serial.print(":");
//   printTwoDigits(now.second());
//   Serial.println();

//   delay(1000);  // Update every second
// }

void printTwoDigits(int number) {
  if (number < 10) {
    lcd.print("0");
  }
  Serial.print(number);
  lcd.print(number);
}

