#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "*************************"
#define BLYNK_DEVICE_NAME "*************************"
#define BLYNK_AUTH_TOKEN "***********************"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "DHT.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>

#define led 26
#define fan 27
#define alarm 25
#define mq2 34

#define dht1 14
#define DHTTYPE DHT11
DHT dht(dht1, DHTTYPE);

#define OLED_SDA 21
#define OLED_SCL 22
Adafruit_SH1106 display(21, 22);

BlynkTimer timer;

const char* ssid = "*****************";
const char* pass = "********************";
const char* auth = "**********************";

int gas = 0;

void sendSensor(){
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  Blynk.virtualWrite(V2, h);
  Blynk.virtualWrite(V3, t);

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.print("Temp:");
  display.println(t);
  display.setCursor(0, 25);
  display.print("Humi:");
  display.println(h);
  display.display();
}

void sendGas(){
  gas = analogRead(mq2);
  Blynk.virtualWrite(V4, gas);
  Serial.println(gas);

  if (gas > 1200){
    Blynk.notify("Gas Detected!");
    digitalWrite(alarm, HIGH);
    display.clearDisplay();
    display.setTextSize(3);
    display.setTextColor(WHITE);
    display.setCursor(10, 10);
    display.print("Waring...");
    display.display();
  }
  else{
    digitalWrite(alarm, LOW);
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 40);
    display.print("Gas: ");
    display.println(gas);
    display.display();
  }
}

BLYNK_WRITE(V0){
  int button=param.asInt();
  digitalWrite(led, button);
}

BLYNK_WRITE(V1){
  int button=param.asInt();
  digitalWrite(fan, button);
}

BLYNK_WRITE(V5){
  int button=param.asInt();
  digitalWrite(alarm, button);
}

void setup() {
  Serial.begin(115200);
  
  
  Blynk.begin(auth, ssid, pass);
  
  dht.begin();
  timer.setInterval(1000L, sendSensor);
  
  //PinMode Define
  pinMode(led, OUTPUT);
  pinMode(fan, OUTPUT);
  pinMode(alarm, OUTPUT);
  pinMode(dht1, INPUT);
  pinMode(mq2, INPUT);

  //OLED Code Initiation
  display.begin(SH1106_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(35, 10);
  display.print("Smart");
  display.setCursor(45, 40);
  display.print("Home");
  display.display();
  delay(2000);
  display.clearDisplay();
  
}

void loop() {
  Blynk.run();              //Blynk Server Runing and connection 
  timer.run();
  sendSensor();
  sendGas();
  
}
