// generic lib e.g. for Serial
#include <Arduino.h>
// OLED lib for chipset SSD1306
#include <ACROBOTIC_SSD1306.h>
// for white temp and huminity sensor DHT22
#include "DHT.h"
// black temp sensor DS18B20
#include <DallasTemperature.h>

// edit and copy the following lines into constants.h
// #define LOG_HOST "http://YOUR-NODE-JS-SERVER:port"
// const char* ssid = "YOUR";
// const char* password = "YOUR";
#include "constants.h"

// send data to our server reachable over wifi via http client
#include "WiFi.h"
#include <HTTPClient.h>

#define WITH_OLED false
#define WITH_DHT false
#define WITH_DS false
#define WITH_WIFI true


OneWire oneWire(26); // a 4.7K resistor is necessary between red & yellow
DallasTemperature dsSensors(&oneWire);
DHT dht(27, DHT22);  // the resistor is already on the board in our case

// GPIOs 34-39 are input-only *and* do not have internal pull-up or pull-down circuitry. This is mentioned in Appendix C.1 of the datasheet, Number 2 in the table.
// DS18B20 sensor uses onewire (data) and OLED uses I2C where there are two data wires: SCL for clock and SDA for data
// 1-Wire: Longer distance. Slower. Strictly master-slave. Theoretically, patent protected. Can easily be implemented in software. Requires 4.7k resistor
// I2C: Shorter distance (can be extended with separate dedicated chips).  Much faster.  Multi-master.  Practically requires dedicated hardware.

void setup() {
  // always setup serial connection and dht sensor
  Serial.begin(9600);
  Serial.println("Start ESP setup");
  dht.begin();

  if(WITH_WIFI) {
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
      delay(200);
      Serial.print(".");
    }
    Serial.println();
  }

  if(WITH_OLED) {
    Wire.begin();
    oled.init();
    oled.clearDisplay();
    // oled.setFont(font5x7); 
    oled.setFont(font5x7); 
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

  if(WITH_WIFI) {
    if(WiFi.status() != WL_CONNECTED) {
      Serial.println("wifi connection not ready");
    } else {
      // read temp and humidity
      float h = dht.readHumidity();
      float t = dht.readTemperature();
      if (isnan(h) || isnan(t)) {
        Serial.println(F("Failed to read from DHT sensor!"));
        return;
      }
      Serial.println("start request");
      HTTPClient http;
      http.begin(LOG_HOST);
      http.addHeader("Content-Type", "text/plain");
      int httpResponseCode = http.POST(String(t) + "," + String(h));
      if(httpResponseCode < 0) {
        Serial.println("cannot send data");
        Serial.println(httpResponseCode);
      }

      http.end();

      // do not send too often
      delay(20000);
    }
  }
}
