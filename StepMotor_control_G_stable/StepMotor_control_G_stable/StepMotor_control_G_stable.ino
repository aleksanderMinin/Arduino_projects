#include <EEPROM.h>
  int stepPin = 46; //z-step = 46
  int dirPin =  48;  //z-dir = 48
  int enPin =   A8 ;   //z-en = 8
  int led = 13;
  boolean light = true;
  int i = 0;
  int speedos = 1;
  boolean en = true;
  boolean rot1 = true;
  long way = 0;
  int usk = 0;
  float g = 0;
  String sms;
     
void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
pinMode(stepPin, OUTPUT);
pinMode(dirPin, OUTPUT);
pinMode(enPin, OUTPUT);
pinMode(led, OUTPUT);
digitalWrite(enPin, LOW);
digitalWrite(stepPin, LOW);
digitalWrite(dirPin, rot1);
}
void Talk() {
  if (Serial.available() > 0) {   //если есть сообщение
    sms = Serial.readString();    //читаем его
    Serial.print("I get->");
    Serial.println(sms);
    delay(100);
  
      if (sms.substring(0,1) == "c") {
        Serial.println("change way");
        rot1 = !rot1;
        digitalWrite(dirPin, rot1);
      }
      else {
        if (sms.substring(0,1) == "e") {
          Serial.println("enPin");
          en = !en;
          digitalWrite(enPin, en);
        }
         else {
            /*way = (int(sms[0])-48)*1000;
            way = (int(sms[1])-48)*100;
            way = (int(sms[1])-48)*10;
            way = int(sms[3])-48;*/
            
            /*way = int(sms[0])*1000;
            way = int(sms[1])*100 + way;
            way = int(sms[2])*10 + way;
            way = int(sms[3]) + way;*/
            way = sms.toInt();
            Serial.print("I get rotate -> ");
            Serial.println(way);
            
            for (i = 0;i <=way; i++) {
              /*if (i < usk || (way - usk) < i){     
                g = abs(i-0.5*way)/100;
                int(g);
              }
              else { 
                g = 0;
                }*/
              digitalWrite(stepPin, HIGH);
              delay(speedos+g);
              digitalWrite(stepPin, LOW);
              /*light = !light;
              digitalWrite(led, light);*/
              delay(speedos+g);
            }
         }
      }
 }
}

void loop() {
  // put your main code here, to run repeatedly:
  Talk();   
  delay(100);
}
