// generic lib e.g. for Serial
#include <Arduino.h>
// OLED lib
#include <ACROBOTIC_SSD1306.h>
// for white temp and huminity sensor DHT22
#include "DHT.h"
// black temp sensor DS18B20
#include <DallasTemperature.h>

#define WITH_OLED true
#define WITH_DHT true
#define WITH_DS true

OneWire oneWire(26); // a 4.7K resistor is necessary between red & yellow
DallasTemperature dsSensors(&oneWire);
DHT dht(27, DHT22);  // the resistor is already on the board in our case

void setup() {
  Serial.begin(9600);
  Serial.println("Start");

  if(WITH_OLED) {
    Wire.begin();
    oled.init();
    oled.clearDisplay();
    // oled.setFont(font5x7); 
    oled.setFont(font5x7); 
  }

  if(WITH_DHT) {
    dht.begin();
  }

  if(WITH_DS) {
    dsSensors.begin();
  }
}

int counter = 0;

void loop() {
  counter++;

  if(WITH_OLED) {
    //oled.setTextXY(5, counter);
    //oled.putString("Hallo Jule");
    //delay(100);
    //oled.setTextXY(5, counter);
    //oled.putString("          ");
  }
  // Serial.println("Blink");
  // delay(500);

  if(WITH_DHT) {
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    delay(3000);

    float h = dht.readHumidity();
    float t = dht.readTemperature();
    if (isnan(h) || isnan(t)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }

    Serial.print(counter);
    Serial.print(", humidity: ");
    Serial.print(h);
    Serial.print(", temperature: ");
    Serial.println(t);

    oled.setTextXY(1, 0);
    oled.putNumber(counter);

    oled.setTextXY(3, 0);
    oled.putFloat(t);
    oled.putString(" 'C (DHT22)");

    oled.setTextXY(5, 0);
    oled.putFloat(h);
    oled.putString(" %");
  }

  if(WITH_DS) {
    delay(3000);
    // call sensors.requestTemperatures() to issue a global temperature request to all devices on the bus
    dsSensors.requestTemperatures();
    Serial.println("read temperature ...");
    // get the temperature from the first sensor only.
    float tempC = dsSensors.getTempCByIndex(0);
    if(tempC != DEVICE_DISCONNECTED_C) {
      Serial.print("Temperature for DS 0 is: ");
      Serial.println(tempC);

      oled.setTextXY(7, 0);
      oled.putFloat(tempC);
      oled.putString(" 'C (DS18)");
    } else {
      Serial.println("Error: Could not read temperature data");
    }
  }
}
