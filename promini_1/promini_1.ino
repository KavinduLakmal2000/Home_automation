#define sensorPin A1

String DATA;
boolean mainP = true;

int rawV = 0;
float i = 0.00;

int autocharge = 0;
boolean automode;

void setup() {
  Serial.begin(9600);

  //6 digitalpin is free,

  pinMode(13,OUTPUT); // loop test
  pinMode(12,OUTPUT); // battery select
  pinMode(11,OUTPUT); // charger
  pinMode(10,OUTPUT); // power cut
  pinMode(9,OUTPUT); // KL Light on off
  pinMode(7,OUTPUT); // reset
  pinMode(8,INPUT);  // main power status in
  pinMode(5,OUTPUT); // inside pir power
  pinMode(A0,INPUT); // battery volt in
  pinMode(4,OUTPUT); // battery or 230v mode
  pinMode(3,OUTPUT); // outside pir power
  pinMode(2,INPUT); // 5v status in
  pinMode(6,INPUT); // 12v status in
  pinMode(16,OUTPUT); // volt meter on/off (A2)
  pinMode(17,OUTPUT); // buzzer (A3)
  pinMode(21,OUTPUT); // outside light (A7)

  digitalWrite(10,LOW);
  digitalWrite(4,LOW);
  digitalWrite(17,LOW);
  digitalWrite(21,LOW);
  digitalWrite(9,HIGH);
  
}

void loop() {

digitalWrite(13,HIGH);
i = 3.95;
batData();
read5V();
read12V();
B_chrge();

if (automode){
  autocharge++;
  digitalWrite(12,HIGH);
  digitalWrite(11,HIGH); 
  
  if (autocharge == 5000){
    autocharge = 0;
    automode = false;
    digitalWrite(11,LOW);
    digitalWrite(12,LOW); 
  }
}
else{
  digitalWrite(11,LOW);
  digitalWrite(12,LOW); 
}

if(digitalRead(8)==LOW){
  mainP = false;
}
else {
  mainP = true;
}
////////////////////////////////////////////////////////////when power is gone
while (!mainP){
  digitalWrite(13,LOW);
  i = 3.75;
  batData();
  read5V();
  read12V();
  
  if(digitalRead(8)==HIGH){
  mainP = true;
  autocharge = 0;
  automode = true;
}
digitalWrite(12,LOW);
digitalWrite(11,LOW);
digitalWrite(10,LOW);
////////////////////////////////////outside light //////////////////////
if(data()=="83"){ // S
 digitalWrite(21,HIGH);
}
if (data()=="115"){ // s
  digitalWrite(21,LOW);
}
//////////////////////////////////// Buzzer ////////////////////////////
if(data()=="68"){ // D
 digitalWrite(9,HIGH);
}
if (data()=="100"){ // d
  digitalWrite(9,LOW);
}
//////////////////////////////////// kl light///////////////////////////
if(data()=="75"){ // K
 digitalWrite(9,HIGH);
}
if (data()=="76"){ // L
  digitalWrite(9,LOW);
}
//////////////////////////////////reset //////////////////////////
if(data()=="79"){ // O
 digitalWrite(7,HIGH);
}
if (data()=="112"){ // p
  digitalWrite(7,LOW);
}
///////////////////////////////////pir on off //////////////////////////////
if(data()=="102"){ // f
 digitalWrite(5,HIGH);
}
if (data()=="103"){ // g
  digitalWrite(5,LOW);
}

if(data()=="70"){ // F
 digitalWrite(3,HIGH);
}
if (data()=="71"){ // G
  digitalWrite(3,LOW);
}
////////////////////////////////////battery selecter ///////////////////////////
if (data()=="113"){ // q
 digitalWrite(12,HIGH);
}
if (data()=="119"){ // w
  digitalWrite(12,LOW);
}
///////////////////////////////////volt meter on/off ////////////////////////////////
if(data()=="67"){ // C
 digitalWrite(16,HIGH);
}
if (data()=="86"){ // V
  digitalWrite(16,LOW);
}

}
//////////////////////////////////////////////////////////////////// main loop /////////////////////////////////////////////////////////
  
////////////////////////////////////////// b select ////////////
if (data()=="113"){ // q
 digitalWrite(12,HIGH);
}
if (data()=="119"){ // w
  digitalWrite(12,LOW);
}
////////////////////////////////// charger ///////////////////////

if(data()=="84"){ // T
 digitalWrite(11,HIGH);
}
if (data()=="89"){ // Y
  digitalWrite(11,LOW);
}
/////////////////////////////////// power cut ///////////////////

if(data()=="69"){ // E
 digitalWrite(10,HIGH);
}
if (data()=="114"){ // r
  digitalWrite(10,LOW);
}

////////////////////////////////// kl light /////////////////
if(data()=="75"){ // K
 digitalWrite(9,HIGH);
}
if (data()=="76"){ // L
  digitalWrite(9,LOW);
}
//////////////////////////////////reset //////////////////////////
if(data()=="79"){ // O
 digitalWrite(7,HIGH);
}
if (data()=="112"){ // p
  digitalWrite(7,LOW);
}
///////////////////////////////////pir on off //////////////////////////////
if(data()=="102"){ // f
 digitalWrite(5,HIGH);
}
if (data()=="103"){ // g
  digitalWrite(5,LOW);
}

if(data()=="70"){ // F
 digitalWrite(3,HIGH);
}
if (data()=="71"){ // G
  digitalWrite(3,LOW);
}
///////////////////////////////battery mode selecter ////////////////////////////////////
if(data()=="72"){ // H
 digitalWrite(4,LOW);
}
if (data()=="74"){ // J
  digitalWrite(4,HIGH);
}

///////////////////////////////////volt meter on/off ////////////////////////////////
if(data()=="67"){ // C
 digitalWrite(16,HIGH);
}
if (data()=="86"){ // V
  digitalWrite(16,LOW);
}



     
delay(200);
} ////////////////////////////////////////// end loop

String data (){
  
if(Serial.available()>0){
    DATA = Serial.read();
  }
  
  return DATA;
  
}

void batData(){
  ////////////////////////////////////////battery read ////////////////////////////
  rawV = (analogRead(A0)*i) / 1010;
   
  if (rawV == 1){
    Serial.write("1");   
  }
  else if (rawV == 2){
    Serial.write("2");   
  }
  else if (rawV == 3){
    Serial.write("3");   
  }
  else if (rawV == 4){
    Serial.write("4");   
  }
  else if (rawV == 5){
    Serial.write("5");   
  }
  else{
    Serial.write("0");
  }
}

void tempProtect(){
  
  int reading = analogRead(sensorPin);
  float voltage = reading * (5.0 / 1024.0);
  float temperatureC = voltage * 100;

  if(temperatureC > 40){
    digitalWrite(10,HIGH);
    delay(500);
    digitalWrite(4,LOW);
  }
  
}

void read5V(){
  if(digitalRead(2)==HIGH){
    Serial.write("M");
  }
  else{
    Serial.write("m");
  }
}

void read12V(){
  if (digitalRead(6)==HIGH){
    Serial.write("X");
  }
  else{
    Serial.write("x");
  }
}

void B_chrge(){
  if (digitalRead(11) == HIGH){
    Serial.write("N");
  }
  else{
    Serial.write("n");
  }
}
