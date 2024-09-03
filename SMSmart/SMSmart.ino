
//بخش تعریف سخت افزار
#include <SoftwareSerial.h>

#define SIM800_TX_PIN 3

#define SIM800_RX_PIN 2

#define tsensorPin A5

#define flame 4

SoftwareSerial mySerial(SIM800_TX_PIN, SIM800_RX_PIN);

//بخش متغیر ها
int device_1 = 12;
int device_2 = 11;
int device_3 = 10;
int device_4 = 9;
int index = 0;
String number = "";
String message = "";
char incomingByte;
String incomingData;
bool atCommand = true;
bool wantmove = false;
float LDRvalue = 0;
int LEDbright = 0;
int thresh = 300;
int ledPin = 13;
int PIRinputPin = 8;
int pirState = LOW;
int val = 0;
int _timeout;
String _buffer;



void setup() {
  Serial.begin(9600);
  _buffer.reserve(50);
  mySerial.begin(9600);



  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(PIRinputPin, INPUT);
  pinMode(7, OUTPUT);
  pinMode(A3, INPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);  
  pinMode(flame,OUTPUT);


  while (!mySerial.available()) {
    mySerial.println("AT");
    delay(1000);
    Serial.println("connecting....");
  }

  Serial.println("Connected..");

  mySerial.println("AT+CMGF=1");
  delay(1000);
  mySerial.println("AT+CNMI=1,2,0,0,0");
  delay(1000);
  mySerial.println("AT+CMGL=\"REC UNREAD\"");

  Serial.println("Ready to received Commands..");

  digitalWrite(12, HIGH);
  digitalWrite(11, HIGH);
  digitalWrite(10, HIGH);
  digitalWrite(9, HIGH);
  digitalWrite(13, LOW);
  digitalWrite(flame,HIGH);
}

void loop() {
  int reading = analogRead(tsensorPin); 
  float voltage = reading * (5.0 / 1024.0);
  float temperatureC = voltage * 100; 
  val = digitalRead(8);

  if (mySerial.available()) {
    delay(100);

    // Serial buffer
    while (mySerial.available()) {
      incomingByte = mySerial.read();
      incomingData += incomingByte;
    }

    delay(10);
    if (atCommand == false) {
      receivedMessage(incomingData);
    } else {
      atCommand = false;
    }


    if (incomingData.indexOf("OK") == -1) {
      mySerial.println("AT+CMGDA=\"DEL ALL\"");
      delay(1000);
      atCommand = true;
    }

    incomingData = "";
  }

  LDRvalue = analogRead(A3);

  if (LDRvalue > thresh) {  //کد دیمر
    LEDbright = map(LDRvalue, 0, 1023, 0, 255);
    analogWrite(7, LEDbright);
  } else {
    analogWrite(7, 0);
  }

  val = digitalRead(PIRinputPin);

  if (val == HIGH && wantmove == true) {
    digitalWrite(ledPin, HIGH);
    delay(3000);
    digitalWrite(ledPin,LOW);

    if (pirState == LOW) {
      Serial.println("Motion detected!");
      pirState = HIGH;
    }
  } else {
    digitalWrite(ledPin, LOW);

    if (pirState == HIGH) {
      Serial.println("Motion ended!");
      pirState = LOW;
    }
  }

  if (temperatureC>30){
    while(temperatureC<25){
     digitalWrite(5, HIGH);      
    }
  }

  if(temperatureC<20){
    while(temperatureC>25){
      digitalWrite(6, HIGH);
    }
    
  }

if (digitalRead(flame)==LOW){
  mySerial.println("ATD+ +989944658007;"); 
  updateSerial();
  delay(20000); 
  mySerial.println("ATH");
  updateSerial();
}  
}

void receivedMessage(String inputString) {

  //Get The number of the sender
  index = inputString.indexOf('"') + 1;
  inputString = inputString.substring(index);
  index = inputString.indexOf('"');
  number = inputString.substring(0, index);
  Serial.println("Number: " + number);

  //Get The Message of the sender
  index = inputString.indexOf("\n") + 1;
  message = inputString.substring(index);
  message.trim();
  Serial.println("Message: " + message);
  message.toUpperCase();


  int pir = digitalRead(8);

  if (message.indexOf("R1 ON") > -1) {
    digitalWrite(device_1, LOW);
    delay(1000);
    Serial.println("Command: Device 1 Turn On.");
  }


  if (message.indexOf("R1 OFF") > -1) {
    digitalWrite(device_1, HIGH);
    delay(1000);
    Serial.println("Command: Device 1 Turn OFF.");
  }

  if (message.indexOf("R2 ON") > -1) {
    digitalWrite(device_2, LOW);
    delay(1000);
    Serial.println("Command: Device 2 Turn On.");
  }

  if (message.indexOf("R2 OFF") > -1) {
    digitalWrite(device_2, HIGH);
    delay(1000);
    Serial.println("Command: Device 2 Turn OFF.");
  }

  if (message.indexOf("R3 ON") > -1) {
    digitalWrite(device_3, LOW);
    delay(1000);
    Serial.println("Command: Device 3 Turn On.");
  }

  if (message.indexOf("R3 OFF") > -1) {
    digitalWrite(device_3, HIGH);
    delay(1000);
    Serial.println("Command: Device 3 Turn Off.");
  }

  if (message.indexOf("R4 ON") > -1) {
    digitalWrite(device_4, LOW);
    delay(1000);
    Serial.println("Command: Device 4 Turn On.");
  }

  if (message.indexOf("R4 OFF") > -1) {
    digitalWrite(device_4, HIGH);
    delay(1000);
    Serial.println("Command: Device 4 Turn Off.");
  }

  if (message.indexOf("AR ON") > -1) {
    digitalWrite(device_4, LOW);
    digitalWrite(device_3, LOW);
    digitalWrite(device_2, LOW);
    digitalWrite(device_1, LOW);
    delay(1000);
    Serial.println("Command: All devices on.");
  }
  
  if (message.indexOf("AR OFF") > -1) {
    digitalWrite(device_4, HIGH);
    digitalWrite(device_3, HIGH);
    digitalWrite(device_2, HIGH);
    digitalWrite(device_1, HIGH);
    delay(1000);
    Serial.println("Command: All devices off.");
  }

  if (message.indexOf("WM OFF") > -1) {
    wantmove = false;
    Serial.println("Command: Move Sensor off.");
  }

  if (message.indexOf("WM ON") > -1) {
    wantmove = true;
    Serial.println("Command: Move Sensor on.");
  }
}

void updateSerial() {
  delay(500);
  while (Serial.available()) {
    mySerial.write(Serial.read());
  }
  while (mySerial.available()) {
    Serial.write(mySerial.read());
  }
}