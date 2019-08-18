// OLED
#include <Wire.h>
// also OLED
#include <ACROBOTIC_SSD1306.h>
// generic lib
#include <Arduino.h>
// for white temp and huminity sensor DHT22
#include "DHT.h"
// for black&long temp sensor DS18B20
#include <OneWire.h>
// make using OneWire lib simpler:
#include <DallasTemperature.h>

// LED somehow does not work when explicitely controlled
#define LED_BUILTIN 2

#define WITH_LED false
#define WITH_OLED false
#define WITH_DHT false
#define WITH_DS true

DHT dht(27, DHT22);
OneWire oneWire(26);  // TODO a 4.7K resistor is necessary
DallasTemperature dsSensors(&oneWire);

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

  if(WITH_LED) {
    // initialize LED digital pin as an output.
    pinMode(LED_BUILTIN, OUTPUT);
  }

  if(WITH_DS) {
    dsSensors.begin();
  }
}

int counter = 0;

void loop() {
  if(WITH_LED) {
    // turn the LED on (HIGH is the voltage level and later low)
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  }

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
    oled.putString(" 'C ");

    oled.setTextXY(5, 0);
    oled.putFloat(h);
    oled.putString(" %");
  }

  if(WITH_DS) {
    // call sensors.requestTemperatures() to issue a global temperature request to all devices on the bus
    dsSensors.requestTemperatures();
    Serial.println("read temperature ...");
    // get the temperature from the first sensor only.
    float tempC = dsSensors.getTempCByIndex(0);
    if(tempC != DEVICE_DISCONNECTED_C) {
      Serial.print("Temperature for the device 1 (index 0) is: ");
      Serial.println(tempC);
    } else {
      Serial.println("Error: Could not read temperature data");
    }
  }
}
