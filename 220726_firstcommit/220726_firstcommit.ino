#include <DHT.h>
#include <DHT_U.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define DHTPIN 7
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

int rainingPin = A1;

void setup() {
  Serial.begin(9600);
  dht.begin();
  lcd.init();
  lcd.backlight();
}

void loop() {
   float humi, temp;
  temp = dht.readTemperature();
  humi = dht.readHumidity();

  if(isnan(humi) || isnan(temp)){
    Serial.println("Failed to read from DHT sensor!!");
    return;
  }

  int value = analogRead(rainingPin);
  
  Serial.print("rain : ");
  Serial.println(value);

  delay(1);
 
  Serial.print("온도 : ");
  Serial.print(temp);
  Serial.println("℃");

  delay(200);
  
  Serial.print("습도 : ");
  Serial.print(humi);
  Serial.println("%");

  delay(200);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("raining : ");
    lcd.setCursor(10, 0);
    lcd.print(value);
    delay(1000);

    lcd.setCursor(0, 1);
    lcd.print("t:");
    lcd.setCursor(1, 1);
    lcd.print(temp);
9
    lcd.setCursor(8, 1);
    lcd.print("RH:");
    lcd.setCursor(9, 1);
    lcd.print(humi);
    delay(1000);
}
