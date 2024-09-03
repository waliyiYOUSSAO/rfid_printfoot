


void printTwoDigits(int number) {
  if (number < 10) {
    lcd.print("0");
  }
  Serial.print(number);
  lcd.print(number);
  
}

