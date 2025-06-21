#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT11

LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHTPIN, DHTTYPE);

// Pins for each component
const int moisturePin = A0;
const int ledPin = 7;
const int buttonPin = 3;
const int relayPin = 8;
const int dhtPin = 2;

volatile bool interrupt = false;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  dht.begin();

  pinMode(ledPin, OUTPUT);
  pinMode(relayPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  //Interrupt code with button
  attachInterrupt(digitalPinToInterrupt(buttonPin), changetext, FALLING); 
}

void loop() {
  // Read DHT11 temperature and humidity
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Read soil moisture
  int sensorValue = analogRead(moisturePin);
  int moisturePercent = map(sensorValue, 1023, 0, 0, 100);

  // LED lights up when soil is dry which below 50%.LED turns off when soil is moist which is above 50%
  if (moisturePercent < 50) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }

  // Click button for interrupt. The interrupt will change display content on LCD screen to moisture percentage and status of relay
  if (interrupt) {
    Serial.println("Interrupt");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Moisture:");
    lcd.print(moisturePercent);
    lcd.print("%");
    lcd.setCursor(0,1);
    lcd.print("Relay On");

    //Relay turns on for 5 seconds to simulate watering 
    digitalWrite(relayPin, HIGH);
    delay(5000);
    digitalWrite(relayPin, LOW);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Relay off");

    delay(2000);

    interrupt = false;
  } else {
    // Code if DHT11 can't work
    lcd.clear();
    if (isnan(humidity) || isnan(temperature)) {
      lcd.setCursor(0, 0);
      lcd.print("DHT11 Error");
      Serial.println("Failed to read DHT");
    } else {
      // Displays temperature and humidity only
      lcd.setCursor(0, 0);
      lcd.print("Temp: ");
      lcd.print(temperature, 1);
      lcd.print("C");

      lcd.setCursor(0, 1);
      lcd.print("Humid: ");
      lcd.print(humidity, 1);
      lcd.print("%");

      Serial.print("Temperature: ");
      Serial.print(temperature);
      Serial.print(" C, Humidity: ");
      Serial.print(humidity);
      Serial.println("%");
    }
  }

  delay(2000);
}

void changetext() {
  delay(2000); 
  interrupt = true;
}
