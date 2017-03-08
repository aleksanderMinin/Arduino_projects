/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO 
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN takes care 
  of use the correct LED pin whatever is the board used.
  If you want to know what pin the on-board LED is connected to on your Arduino model, check
  the Technical Specs of your board  at https://www.arduino.cc/en/Main/Products
  
  This example code is in the public domain.

  modified 8 May 2014
  by Scott Fitzgerald
  
  modified 2 Sep 2016
  by Arturo Guadalupi
*/


// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  //Serial.begin(115200);
  Serial.begin(115200);
}

void talk(int spd) {
  Serial.begin(spd);
  Serial.print('A');
  delay(1);
  Serial.print('T');
  delay(1);
  Serial.print('+');
  delay(1);
  Serial.print('B');
  delay(1);
  Serial.print('A');
  delay(1);
  Serial.print('U');
  delay(1);
  Serial.print('D');
  delay(1);
  Serial.print('4');
  delay(1000);
  Serial.print('A');
  delay(1);
  Serial.print('T');
  delay(1);
  Serial.print('+');
  delay(1);
  Serial.print('P');
  delay(1);
  Serial.print('N');
  delay(1);
  Serial.end();
  delay(1000);
  Serial.print('A');
  delay(1);
  Serial.print('T');
  delay(1);
  Serial.println(""); 
}

// the loop function runs over and over again forever
void loop() {
  // put your main code here, to run repeatedly:
  talk(1200);
                delay(500);
  talk(2400);
            delay(500);
  talk(4800);
            delay(500);
  talk(9600);
            delay(500);
  talk(19200);
            delay(500);
  talk(38400);
            delay(500);
  talk(57600);
              delay(500);
  talk(115200);
          delay(500);
  talk(230400);
          delay(500);
  talk(460800);
          delay(500);
  talk(921600);
          delay(500);
  talk(1382400);
            delay(500);
  
  digitalWrite(LED_BUILTIN,HIGH);
  delay(9999999999999);
}
