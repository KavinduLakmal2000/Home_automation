#define BLYNK_TEMPLATE_ID "TMPLXlOIzBQ5"
#define BLYNK_DEVICE_NAME "Smart House 2022"
#define BLYNK_AUTH_TOKEN "awxkaHqxaW15rK-q0VRrTfL99Ekl2m8o"
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <WiFiUdp.h>
#include <DHT.h>
#include <NTPClient.h>


#define DHTTYPE DHT11   // DHT 11
#define dht_dpin 2 // D4
DHT dht(dht_dpin, DHTTYPE); 

const long utcOffsetInSeconds = 19800;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

char auth[] = BLYNK_AUTH_TOKEN;

char ssid[] = "NoobMaster20";
char pass[] = "K.L@2000";

//char ssid[] = "HUAWEI Y5 2017";
//char pass[] = "1234abcd";

//char ssid[] = "SLT_Fiber_Optic";
//char pass[] = "Life1Mal7i";


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
String data;

int BAT= A0;             
float RatioFactor=5.714;

int value = LOW;
float Tvoltage=0.0;
float Vvalue=0.0,Rvalue=0.0;
/*
void pullDown(){
  digitalWrite(10,LOW);
  digitalWrite(9,LOW);
  digitalWrite(12,LOW);
  digitalWrite(4,LOW);
  digitalWrite(5,LOW);

  Serial.write("K");
  delay(20);
  Serial.write("r");
  delay(20);
  Serial.write("p");
  delay(20);
  Serial.write("J");
  
}
  
*/
void setup()
{  
  
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  Serial.println("testSetupBegin");
  
  
  pinMode(16,OUTPUT); // loop status
  //pinMode(10,OUTPUT); // D
  //pinMode(9,OUTPUT); // K
  //pinMode(13,INPUT); // main power in
  //pinMode(12,OUTPUT); // S
  //pinMode(4,OUTPUT); // kl room
  //pinMode(5,OUTPUT); // L 
  //pinMode(15,INPUT); // pir outside
  //pinMode(14,INPUT); // 12v status
  //pinMode(0,INPUT); // pir inside 
  
  digitalWrite(16,LOW);
  
  Serial.println("testSetupBegin");
  
  timer.setInterval(1000L, myTimerEvent);
  
  timeClient.begin();

  dht.begin();


  starter = false;
  
  delay(100);
  
  }
  void loop(){
    Blynk.run();
  timer.run();
  }
/*
void loop()
{
  digitalWrite(16,HIGH);

boolean isconnected = Blynk.connected(); 
while(isconnected == false){
  digitalWrite(16,LOW);
}

while (!starter){
  Serial.write("r");
  delay(30);
  Serial.write("p");
  delay(30);
  Serial.write("V");

  starter = true;
}

if(Tvoltage > 14.5){
  Serial.write("Y");
  delay(20);
  Blynk.logEvent("full_charged");
  Blynk.virtualWrite(V21, LOW);
  Blynk.virtualWrite(V18, LOW);
  Blynk.virtualWrite(V3, LOW);
  Serial.write("V");
  
}
  
  Blynk.run();
  timer.run();
  timeClient.update();
  proVolt();
  Status5V();
  Status12V();
  livingroom_pir();
  outside_pir();
  voltMeter();
  send_DHT();
  int Time = timeClient.getHours();
  int MiN = timeClient.getMinutes();
  int sec = timeClient.getSeconds();
  
  Blynk.virtualWrite(V17, Time);
  Blynk.virtualWrite(V19, MiN);
  Blynk.virtualWrite(V20, sec);
  

  //18 > 17 && 22 > 23
  
  if (Time > 18 && Time < '23' && digitalRead(13)==LOW){ 
      digitalWrite(12,HIGH);
      digitalWrite(5,HIGH);
      digitalWrite(9,HIGH);
      digitalWrite(10,HIGH);
      Elight = false;  
  }
  
  else {
      Elight = true;
  }
    

  while(!Elight){//////////////////////////////////////////////////// 2nd loop
  digitalWrite(16,HIGH);
  Blynk.run();
  send_DHT();
  timer.run();
  timeClient.update();
  Status5V();
  Status12V();
  proVolt();
  livingroom_pir();
  outside_pir();
  voltMeter();
  int Time = timeClient.getHours();
  int MiN = timeClient.getMinutes();
  int sec = timeClient.getSeconds();
  
  Blynk.virtualWrite(V17, Time);
  Blynk.virtualWrite(V19, MiN);
  Blynk.virtualWrite(V20, sec);
  
    
    if (Time > 18 && Time < '23' && digitalRead(13)==LOW){
      Elight = false;
    }
    else {
      digitalWrite(12,LOW);
      digitalWrite(5,LOW);
      digitalWrite(9,LOW);
      digitalWrite(10,LOW);
      Elight = true;
    }
  digitalWrite(16,LOW);
  delay(5);
  }
  digitalWrite(16,LOW);
  delay(5);
} ///////////////////////////////////////// loop end



void Status5V(){
  if (data == "M"){
    Blynk.virtualWrite(V13, HIGH);
  }
  if(data == "m") {
    Blynk.virtualWrite(V13, LOW);
  }
}
void Status12V(){
  if (digitalRead(14)==HIGH){
    Blynk.virtualWrite(V14, HIGH);
  }
  else {
    Blynk.virtualWrite(V14, LOW);
  }
}

void proVolt(){
 if (Serial.available()>0){
    data = Serial.read();
     Blynk.virtualWrite(V11, data);
 }
}

void livingroom_pir(){
 if (digitalRead(0) == HIGH){
        Blynk.logEvent("motionIn","Read from PIR 2");
 }
}

void outside_pir(){
 if (digitalRead(15)==HIGH){
        Blynk.logEvent("motionOut","Read from PIR 1");
 }
}

void voltMeter(){
 
  for(unsigned int i=0;i<10;i++){
  Vvalue=Vvalue+analogRead(BAT);        
  delay(5);                              
  }
  Vvalue=(float)Vvalue/10.0;            
  Rvalue=(float)(Vvalue/1024.0)*5;      
  Tvoltage=Rvalue*RatioFactor;          

  Blynk.virtualWrite(V9, Tvoltage);
}


void send_DHT()
{
  int h = dht.readHumidity();
  int t = dht.readTemperature();

    Blynk.virtualWrite(V5, t);
    Blynk.virtualWrite(V6, h);
}

BLYNK_WRITE(V22) { // battery Selecter
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

BLYNK_WRITE(V23) { // charger
  
    if (param.asInt()) {
      Serial.write("C"); // turn on volt meter
      delay(20);
      Blynk.logEvent("full_charged","Full Charged");
      Blynk.virtualWrite(V18, HIGH); // charger LED
      Blynk.virtualWrite(V21, HIGH);  // volte meter SW
      Serial.write("T");
      
    } 
    
    else {
      Blynk.virtualWrite(V18, LOW); // charger LED
      Blynk.virtualWrite(V21, HIGH);  // volte meter SW
      Serial.write("Y");
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

BLYNK_WRITE(V10) { // pir 1
    if (param.asInt()) {
      Serial.write("f");
    } else {
     Serial.write("g");
    }
}

BLYNK_WRITE(V12) { // led light mode
    if (param.asInt()) {
      Serial.write("H");
    } else {
     Serial.write("J");
    }
}

BLYNK_WRITE(V15) { // pir 2
    if (param.asInt()) {
      Serial.write("F");
    } else {
     Serial.write("G");
    }
}

BLYNK_WRITE(V21) { // volt meter off/on
    if (param.asInt()) {
      Serial.write("C");
    } else {
     Serial.write("V");
    }
}

*/
