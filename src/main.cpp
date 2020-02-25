// generic lib e.g. for Serial
#include <Arduino.h>
// OLED lib for chipset SSD1306
#include <ACROBOTIC_SSD1306.h>
// for white temp and huminity sensor DHT22
#include "DHT.h"
// black temp sensor DS18B20
#include <DallasTemperature.h>

// edit and copy the following lines into constants.h
// const char* logHost = "http://your-host:8000/";
// const char* ssid = "YOUR";
// const char* password = "YOUR";
#include "constants.h"

// send data to our server reachable over wifi via http client
// make sure your router and firewall allows device "espressif" reaching out to potentially remove "logHost" service
#include "WiFi.h"
#include <HTTPClient.h>

#define WITH_OLED false
#define WITH_DHT false
#define WITH_DS false
#define WITH_WIFI true
#define DELAY_IN_S 150

OneWire oneWire(26); // a 4.7K resistor is necessary between red & yellow
DallasTemperature dsSensors(&oneWire);
DHT dht(27, DHT22);  // the resistor is already on the board in our case

// GPIOs 34-39 are input-only *and* do not have internal pull-up or pull-down circuitry. This is mentioned in Appendix C.1 of the datasheet, Number 2 in the table.
// DS18B20 sensor uses onewire (data) and OLED uses I2C where there are two data wires: SCL for clock and SDA for data
// 1-Wire: Longer distance. Slower. Strictly master-slave. Theoretically, patent protected. Can easily be implemented in software. Requires 4.7k resistor
// I2C: Shorter distance (can be extended with separate dedicated chips).  Much faster.  Multi-master.  Practically requires dedicated hardware.

void setup() {
  // always setup serial connection and sensors
  Serial.begin(9600);
  Serial.println("Start ESP setup");
  dht.begin();
  dsSensors.begin();

  // configure to sleep X million ms
  esp_sleep_enable_timer_wakeup(DELAY_IN_S * 1e6);

  if(WITH_OLED) {
    // default pins SDA=21 and SCL=22 in pins_arduino.h
    Wire.begin();
    // Wire.begin(21, 22, frequency=0); // frequency 0 means do not change default of 100kHz
    oled.init();
    oled.clearDisplay();
    // oled.setFont(font5x7); 
    oled.setFont(font5x7); 
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
    // if an error happens e.g. on sensor read at least wait this time
    delay(3000);

    // read temperatures first, then do wifi
    dsSensors.requestTemperatures();
    float dsTemp = dsSensors.getTempCByIndex(0);
    if(dsTemp == DEVICE_DISCONNECTED_C) {
        Serial.println(F("Failed to read from DS temp sensor!"));
        return;
    }

    // read temp and humidity
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    if (isnan(h) || isnan(t)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }

    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
      delay(200);
      Serial.print(".");
    }
    Serial.println();
    Serial.print("start request to ");
    Serial.println(logHost);
    HTTPClient http;
    http.begin(logHost);
    http.addHeader("Content-Type", "text/plain");
    int httpResponseCode = http.POST(String(h) + "," + String(t) + "," + String(dsTemp));
    if(httpResponseCode < 0) {
      Serial.println("cannot send data");
      Serial.println(httpResponseCode);
    }

    http.end();
    WiFi.disconnect();

    // for battery usage this is very nice. But stable wifi usage with battery seems to be only possible with 3.6V Li-ion
    esp_deep_sleep_start();
  }
}
