void rfid_init(){
  SPI.begin();
   rfid.PCD_Init();
  Serial.println("RFID Initialization Successful");
}

void lcd_init(){
  Wire.begin(SDA_PIN, SCL_PIN);
  lcd.begin(16, 2);
  lcd.backlight();
}

void clock_init(){
  clock_.Begin();
    clock_.init_WiFi("USER_0BD0F6", "s2PAhtaL"); // Your Wi-Fi credentials // USER_0BD0F6 s2PAhtaL   HUAWEI-2.4G-c9bh
    clock_.sync_rtc();
}