#ifndef CLOCK_FUNCTION_H
#define CLOCK_FUNCTION_H

#include <Wire.h>
#include <ThreeWire.h>
#ifdef ESP32
#include <WiFi.h>
#elif defined (ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <RtcDS1302.h>
#include "RTClib.h"
#include "time.h"

class Clock_ {
public:
    void Begin();
    void init_WiFi(const char* ssid, const char* password);
    void sync_rtc();
    String show_date();

private:
    ThreeWire my_wire = ThreeWire(D0, D1, D4);  // Pins pour le DS1302 // esp8266 13 14 12 // esp32 13 15 12 // DAT: D2,CLK: D1,RST: D3
    RtcDS1302<ThreeWire> rtc = RtcDS1302<ThreeWire>(my_wire); // Initialisation RTC
    
};

#endif
