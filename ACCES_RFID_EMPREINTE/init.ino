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




