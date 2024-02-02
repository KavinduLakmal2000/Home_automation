//this project in dean email
//#define BLYNK_TEMPLATE_ID "TMPLln-Vhydm"
//#define BLYNK_DEVICE_NAME "test"
//#define BLYNK_AUTH_TOKEN "b9tvGjv-42Mytf7RtFeo-GgPrCZZo6Dd"

#define BLYNK_TEMPLATE_ID "TMPLXlOIzBQ5"
#define BLYNK_DEVICE_NAME "Smart House 2022"
#define BLYNK_AUTH_TOKEN "ULdOAo97xNi6xYokXpE8hg22CV2HrMsv"


// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <DHT.h>

#define DHTTYPE DHT11   // DHT 11
#define dht_dpin 2 // D4
DHT dht(dht_dpin, DHTTYPE);

char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "SLT_Fiber_Optic";
char pass[] = "Life1Mal7i";

const long utcOffsetInSeconds = 19800;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

BlynkTimer timer;


BLYNK_WRITE(V0)
{
  int value = param.asInt();
  Blynk.virtualWrite(V1, value);
}

BLYNK_CONNECTED()
{

  Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
  Blynk.setProperty(V3, "onImageUrl",  "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
  Blynk.setProperty(V3, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");
}


void myTimerEvent()
{
  Blynk.virtualWrite(V2, millis() / 1000);
}

boolean Elight = true;
boolean starter;
boolean p_in = false;
boolean p_out = false;
boolean autolight = false;
int securtyCount = 0;
String data;
int Time;

int BAT = A0;
float RatioFactor = 5.714;

int value = LOW;
float Tvoltage = 0.0;
float Vvalue = 0.0, Rvalue = 0.0;

void pullDown() {
  digitalWrite(10, LOW);
  digitalWrite(9, LOW);
  digitalWrite(12, LOW);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);

  Serial.write("K");
  delay(20);
  Serial.write("r");
  delay(20);
  Serial.write("p");
  delay(20);
  Serial.write("J");

}

void setup()
{
  Serial.begin(9600);

  pinMode(16, OUTPUT); // loop status



  digitalWrite(16, LOW);

  Blynk.begin(auth, ssid, pass);

  timer.setInterval(1000L, myTimerEvent);
  timeClient.begin();
  dht.begin();

  starter = false;

  //pinMode(9,OUTPUT); // don't use 9 it's give u are damn error to kill ur whole day
  pinMode(16,OUTPUT); //MCU test LED
  pinMode(10, OUTPUT); // D
  pinMode(13, OUTPUT); // K 
  pinMode(12, OUTPUT); // S
  pinMode(4, OUTPUT); // kl room
  pinMode(5, OUTPUT); // L
  pinMode(15, INPUT); // main power
  pinMode(14, INPUT); // pir outside
  pinMode(0, INPUT); // pir inside

  delay(100);

}

void loop()
{
  digitalWrite(16, HIGH);

  boolean isconnected = Blynk.connected();
  while (isconnected == false) {
     digitalWrite(16, LOW);
     digitalWrite(10, LOW);
  
  }

  if (Serial.available() > 0) {
    data = Serial.read();
  }


  while (!starter) {
    Serial.write("r");
    delay(30);
    Serial.write("p");
    delay(30);
    Serial.write("V");

    starter = true;
  }

  if (Tvoltage > 14.8) {
    Serial.write("Y");
    delay(20);
    Blynk.logEvent("full_charged");
    //Blynk.virtualWrite(V21, LOW);
    Blynk.virtualWrite(V18, LOW);
    Blynk.virtualWrite(V3, LOW);
    Serial.write("w");

  }

  Blynk.run();
  timer.run();
  timeClient.update();
  proVolt();
  Status5V();
  pir_read();
  Status12V();
  voltMeter();
  send_DHT();
  mainPowerRead();
  //securtyMode(160);
  Time = timeClient.getHours();
  int MiN = timeClient.getMinutes();
  int sec = timeClient.getSeconds();

  Blynk.virtualWrite(V17, Time);
  Blynk.virtualWrite(V19, MiN);
  Blynk.virtualWrite(V20, sec);


  //18 > 17 && 22 > 23
   

  if (Time > 18 && Time < 23 && digitalRead(15)==LOW) { 
   
      digitalWrite(12,HIGH);
      //digitalWrite(5,HIGH);
      digitalWrite(10,HIGH);
      digitalWrite(13,HIGH);
   
   //digitalWrite(4, HIGH); 
    Elight = false;
  }

  else {
    Elight = true;
  }


  while (!Elight) { //////////////////////////////////////////////////// 2nd loop (after power cut)
    digitalWrite(16, HIGH);

    if (Serial.available() > 0) {
      data = Serial.read();
    }

    Blynk.run();
    send_DHT();
    pir_read();
    timer.run();
    timeClient.update();
    Status5V();
    Status12V();
    proVolt();
    voltMeter();
    mainPowerRead();
   // securtyMode(120);
    Time = timeClient.getHours();
    int MiN = timeClient.getMinutes();
    int sec = timeClient.getSeconds();

    Blynk.virtualWrite(V17, Time);
    Blynk.virtualWrite(V19, MiN);
    Blynk.virtualWrite(V20, sec);
    
   
    if (Time > 18 && Time < 23 && digitalRead(15)==LOW) {
      Elight = false;
    }
    else {      
        digitalWrite(12,LOW);
        digitalWrite(5,LOW);
        digitalWrite(4,LOW);
        digitalWrite(13,LOW);
        digitalWrite(10,LOW);
  
      Elight = true;
    }
  

    
    digitalWrite(16, LOW);
    delay(5);
  }
  digitalWrite(16, LOW);
  delay(5);
} ///////////////////////////////////////// loop end

void securtyMode(int Etime){ // after turn on inside pir this will start (automatic lights on and off)
  
  if (Time > 18 || Time < 8 && autolight) {

  if(securtyCount > 0){
      digitalWrite(12,HIGH);
      digitalWrite(5,HIGH);
      digitalWrite(10,HIGH);
      digitalWrite(13,HIGH);
      digitalWrite(4, HIGH); 
      Serial.write("S");
      
  }

if(securtyCount == Etime){
      securtyCount = 0;
      digitalWrite(12,LOW);
      digitalWrite(5,LOW);
      digitalWrite(10,LOW);
      digitalWrite(13,LOW);
      digitalWrite(4, LOW); 
      Serial.write("s");   
}
securtyCount++;    
 
}
}

void pir_read(){
  
  if (p_out && digitalRead(14)==HIGH){ // out pir
   Blynk.logEvent("motionOut");
   Serial.write("D");
   delay(100);
   Serial.write("S");
  }
  
  else{
   Serial.write("d");
   delay(100);
   Serial.write("s");
  }

  
  if (p_in && digitalRead(0)==HIGH){ // in pir
    //Blynk.logEvent("motionIn");
    Blynk.virtualWrite(V25, HIGH);
    Serial.write("D");
  }
  else{
    Blynk.virtualWrite(V25, LOW);
    Serial.write("d");
  }
}

void mainPowerRead() {

  if (digitalRead(15)==HIGH) {
    Blynk.virtualWrite(V24, HIGH);
  }
  else {
    Blynk.virtualWrite(V24, LOW);
  }
}

void Status5V() {
  if (data == "M") {
    Blynk.virtualWrite(V13, HIGH);  
  }
  if (data == "m") {
    Blynk.virtualWrite(V13, LOW);   
  }
}

void Status12V() {
  if (data == "X") {
    Blynk.virtualWrite(V14, HIGH);
  }
  if (data == "x") {
    Blynk.virtualWrite(V14, LOW);
  }
}

void proVolt() {
  if (Serial.available() > 0) {
    data = Serial.read();
    Blynk.virtualWrite(V11, data);
  }
}

void voltMeter() {

  for (unsigned int i = 0; i < 10; i++) {
    Vvalue = Vvalue + analogRead(BAT);
    delay(5);
  }
  Vvalue = (float)Vvalue / 10.0;
  Rvalue = (float)(Vvalue / 1024.0) * 5;
  Tvoltage = Rvalue * RatioFactor;

  Blynk.virtualWrite(V9, Tvoltage);
}


void send_DHT()
{
  int h = dht.readHumidity();
  int t = dht.readTemperature();

  Blynk.virtualWrite(V5, t);
  Blynk.virtualWrite(V6, h);
}

BLYNK_WRITE(V22) { // 16v battery on/off
  if (param.asInt()) {
    Serial.write("q");
  } else {
    Serial.write("w");
  }
}

BLYNK_WRITE(V7) { // main power cut
  if (param.asInt()) {
    Serial.write("E");
  } else {
    Serial.write("r");
  }
}

BLYNK_WRITE(V23) { // charger + 16v battery

  if (param.asInt()) {
    Blynk.virtualWrite(V18, HIGH); // charger LED
    Blynk.virtualWrite(V22, HIGH);  // 16v SW
    Serial.write("T"); // turn charger and 16v battery on

  }

  else {
    Blynk.virtualWrite(V18, LOW); // charger LED
    Blynk.virtualWrite(V21, LOW);  // 16v SW
    Serial.write("Y"); // turn off charger and 16v battery
  }
}

BLYNK_WRITE(V4) { // promini reset
  if (param.asInt()) {
    Serial.write("O");
  } else {
    Serial.write("p");
  }
}

BLYNK_WRITE(V8) { // kl light
  if (param.asInt()) {
    Serial.write("K");
  } else {
    Serial.write("L");
  }
}

BLYNK_WRITE(V10) { // inside_pir
  if (param.asInt()) {
    Serial.write("f");
    p_in = true; 
  } else {
    Serial.write("g");
    p_in = false;
  }
}

BLYNK_WRITE(V15) { // Outside_pir
  if (param.asInt()) {
    Serial.write("F");
    p_out = true;
  } else {
    Serial.write("G");
    p_out = false;
  }
}

BLYNK_WRITE(V12) { // led light mode
  if (param.asInt()) {
    Serial.write("H");
  } else {
    Serial.write("J");
  }
}

/*
BLYNK_WRITE(V21) { // volt meter off/on
  if (param.asInt()) {
    Serial.write("C");
  } else {
    Serial.write("V");
  }
}
*/

BLYNK_WRITE(V26) { // autolights
  if (param.asInt()) {
    autolight = true;
  } else {
    autolight = false;
  }
}
