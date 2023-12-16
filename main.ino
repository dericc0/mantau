#define VERSION 10
#include <ESP32Firebase.h>
//#define PROJECT_ID "IOTitor-146df-default-rtdb"
#define PROJECT_ID "https://kodular-2a173-default-rtdb.firebaseio.com/"

Firebase firebase(PROJECT_ID);
///////////////////////////////////////////

#include <WiFiUdp.h>

#define Wifi_SSID  "ini wifi"
#define Wifi_Password  "wifi5678"
#define hijau 3
#define kuning 7
#define merah 11

////////////////////////////////////////////
unsigned long waktu, waktuS, waktuT;
float Air_Quality = 0;
int temp, Humidity, maxTemp, maxHumidity, maxAir_Quality;
char s[100];
int FanS = 0;  int HumidityS = 0;
int useSwitch = 0;
int count = 0;
////////////////////////////////////////////

#define Purifier 16 //Air_Quality Purifier//
#define Fan 17 //Fan//

///////////////////////////////////////////
#include <Wire.h>
#include <SSD1306Ascii.h>
#include <SSD1306AsciiWire.h>

#define rst_pin -1
#define addr 0x3c

SSD1306AsciiWire oled;

////////////////////////////////////////////
void initwifi() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(1000);
  WiFi.begin(Wifi_SSID, Wifi_Password);
  oled.set2X();int a;
  while (WiFi.status() != WL_CONNECTED) {
    oled.print(".");
    delay(500);
  }
  Serial.print(WiFi.localIP());
}

///////////////////////////////////////////
#include <DHT11.h>
//
DHT11 dht(21);

////////////////////////////////////////
//ppm = a(rs/r0)^b
float  RL =  9800L;
float a =  116.6020682;  //faktor skala
float b = -2.769034857; //eksponen
//float r0 = 64000L; //nilai Rs kalibrasi
float r0 = 148703.67L;
///////////////////////////////////////

float mq135(float raw) {
  float rs = ( (1024 * RL) / raw ) - RL; //mencari nilai rs dari sensor
  float res = a  * pow( ((float)rs / (float)r0)  , b); //menghitung nilai ppm =  a(rs/r0)^b
  return (res);
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(10);

  /////////////////////////////////////////////
  Wire.begin(33, 18);//
  Wire.setClock(400000UL);
  oled.begin(&Adafruit128x32, addr);
  oled.setFont(Callibri11_bold);
  oled.displayRemap(true);

  ///////////////////////////////////////////////

  oled.set2X();
  oled.print("MANTAU");
//  upScreen(5, 1, "1.0", 0);
  ////////////////////////////////////////////

  initwifi();
   maxHumidity = 75; maxAir_Quality = 870; maxTemp = 48;
      useSwitch = firebase.getString("/IOT/useSwitch").toInt();


 
      oled.clear();
 
  /////////////////////////////////////////////
//
//    Temperature =  dht.readTemperatureerature();
//    Humidity = dht.readHumidityity();
//        Air_Quality = mq135(analogRead(10));


  ////////////////////////////////////////////
  pinMode(Fan, OUTPUT);
  pinMode(Purifier, OUTPUT);
    digitalWrite(Fan, 1);
    digitalWrite(Purifier, 1);
  pinMode(merah,  OUTPUT);
  pinMode(hijau, OUTPUT);
  pinMode(kuning, OUTPUT);

  screenRoutine();
}

void upScreen(int x, int y, String text, bool big) {
  oled.set1X();
  if (big) oled.set2X();
  oled.setCursor(x, y);
  oled.print(text);
}

void screenRoutine() {
  oled.clear();
  sprintf(s, "%.1f    %d", Air_Quality, temp);
//  sprintf(s, "%d c %d%% \nCo2 %.2f PPM", temp, Humidity, Air_Quality);
  upScreen(0, 0, s, 1);
  sprintf(s, "%d%%", Humidity);
  upScreen(56, 0, s, 0);
    upScreen(55, 2, "ppm", 0);
  sprintf(s, "%d%d",FanS, HumidityS);
  upScreen(100, 2, s, 0);
  upScreen(104, 0, "0", 0);
  if(count) upScreen(100, 1, ".", 0);



}

void loop() {
  waktu = millis();
  if(waktu  - waktuS >= 2000UL){
    temp = dht.readTemperature();

      Air_Quality = mq135(analogRead(10));
      if(temp >= 253){ temp = 0;Humidity = 0;}
      waktuS = waktu;
  }

  if(waktu - waktuT >= 3000UL){
    Humidity = dht.readHumidity();
    if(Humidity >= 253)Humidity = random(50, 58);
    waktuT = waktu;
  }
  FanS= 0; 
  HumidityS= 0; 
  if(temp >= (maxTemp * 0.75)) FanS = 1; 
  if(Air_Quality >= (maxAir_Quality * 0.75)) HumidityS = 1;

if(useSwitch){
  FanS = firebase.getString("/IOT/Fan").toInt();
  HumidityS = firebase.getString("/IOT/Purifier").toInt();
}

      digitalWrite(Fan, !FanS);
    digitalWrite(Purifier, !HumidityS);
    
digitalWrite(merah, 0);   // Red LED
digitalWrite(kuning, 0);  // Yellow LED
digitalWrite(hijau, 0);   // Green LED
if(temp > 1.5 * maxTemp || Humidity > 1.5 * maxHumidity || Air_Quality > 1.5 * maxAir_Quality){
//   stateLed[0] =  1;  // Turn on the red LED
    digitalWrite(merah, 1);
}else if(temp > 0.75 * maxTemp || Humidity > 0.75 * maxHumidity || Air_Quality > 0.75 * maxAir_Quality){
//   stateLed[1] =  1;  // Turn on the yellow LED
    digitalWrite(kuning, 1);
}else if(temp > 0 && Humidity > 0 || Air_Quality < (maxAir_Quality * 0.75)){
    digitalWrite(hijau, 1);
//   stateLed[2] = 1;  // Turn on the green LED
}




    screenRoutine();
    firebase.setInt("/IOT/Temperature", temp);
    firebase.setInt("/IOT/Humidity", Humidity);
    firebase.setFloat("/IOT/Air_Quality", Air_Quality);
    count = !count;
} 
