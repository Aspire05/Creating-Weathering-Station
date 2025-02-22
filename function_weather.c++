#define BLYNK_TEMPLATE_ID "TMPL6tOfl6G6t"
#define BLYNK_TEMPLATE_NAME "Weather Station with Mobile Notifications"
#define BLYNK_AUTH_TOKEN "MLlcNSLs5FPyOq1adyVT8998V0999dCW"  // Your Blynk Auth Token

#include <Wire.h>
#include <Adafruit_BME280.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// Blynk credentials
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Mi 11 Lite";  // Replace with your Wi-Fi SSID
char pass[] = "12341235";    // Replace with your Wi-Fi password

// DHT22 setup
#define DHTPIN D4  // GPIO pin for DHT22
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// BME280 setup (I2C interface)
Adafruit_BME280 bme;

// LDR setup
#define LDRPIN D6  // Digital pin for LDR sensor

// Rain sensor setup
#define RAIN_SENSOR_PIN A0 // Analog pin for Rain Sensor

// Blynk Timer
BlynkTimer timer;

// Function to send sensor data to Blynk
void sendSensorData() {
    // DHT22 readings
    float dhtTemperature = dht.readTemperature();
    float dhtHumidity = dht.readHumidity();

    // BME280 readings (Barometric pressure)
    float bmePressure = bme.readPressure() / 100.0F; 

    // LDR readings
    int ldrValue = digitalRead(LDRPIN);  
    int ledValue = (ldrValue == HIGH) ? 255 : 0; // 255 for light detected, 0 for dark

    // Rain sensor readings
    int rainValue = analogRead(RAIN_SENSOR_PIN); // Analog value from Rain Sensor
    String rainStatus = (rainValue > 500) ? "No Rain" : "Rain Detected"; // Adjust threshold as needed

    // DHT22 Sensor Alerts
    if (isnan(dhtTemperature) || isnan(dhtHumidity)) {
        Serial.println("Failed to read from DHT22 sensor!");
    } else {
        Blynk.virtualWrite(V1, dhtTemperature); 
        Blynk.virtualWrite(V2, dhtHumidity);
        Blynk.logEvent("temperature_alert", String("Temperature: ") + dhtTemperature + " °C");
        Blynk.logEvent("humidity_alert", String("Humidity: ") + dhtHumidity + " %");
    }

    // BME280 Sensor Alerts
    if (isnan(bmePressure)) {
        Serial.println("Failed to read from BME280 sensor!");
    } else {
        Blynk.virtualWrite(V3, bmePressure);
        Blynk.logEvent("pressure_alert", String("Pressure: ") + bmePressure + " hPa");
    }

    // LDR Light Status Alerts
    Blynk.virtualWrite(V4, ledValue);  // Send 255 for light, 0 for dark to Virtual Pin 4
    String lightStatus = (ledValue == 255) ? "Light Detected!" : "Dark Detected!";
    Blynk.logEvent("light_status_change", lightStatus);

    // Rain Sensor Alerts
    Blynk.virtualWrite(V5, rainStatus);
    Blynk.logEvent("rain_status_change", rainStatus);

    // Serial Monitor Output for debugging
    Serial.println("--------Weather Station All Data--------");
    Serial.print("DHT22 Temperature: "); Serial.print(dhtTemperature); Serial.println(" °C");
    Serial.print("DHT22 Humidity: "); Serial.print(dhtHumidity); Serial.println(" %");
    Serial.print("BME280 Barometric Pressure: "); Serial.print(bmePressure); Serial.println(" hPa");
    Serial.print("LDR LED Value: "); Serial.println(ledValue == 255 ? "Light Detected!" : "Dark Detected!");
    Serial.print("Rain Sensor Value: "); Serial.print(rainValue); 
    Serial.print(" -> "); Serial.println(rainStatus);
    Serial.println("----------------------------------------");
}

void setup() {
    Serial.begin(115200);
    Blynk.begin(auth, ssid, pass);

    dht.begin();
    if (!bme.begin(0x76)) { 
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        while (1);  
    }

    pinMode(LDRPIN, INPUT); // Set LDR pin as INPUT
    timer.setInterval(2000L, sendSensorData);  // Send sensor data every 2 seconds
}

void loop() {
    Blynk.run();  // Run Blynk
    timer.run();  // Run Blynk Timer
}
