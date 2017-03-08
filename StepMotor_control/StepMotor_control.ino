  int stepPin = 26;
  int dirPin = 28;
  int enPin = 24;
  int led = 13;
  boolean light = true;
  int i = 0;
  int speedos = 1;
  boolean rot1 = true;
  int way = 9600;
  int usk = 1600;
  float g = 0;
     
void setup() {
  // put your setup code here, to run once:
pinMode(stepPin, OUTPUT);
pinMode(dirPin, OUTPUT);
pinMode(enPin, OUTPUT);
pinMode(led, OUTPUT);
digitalWrite(enPin, LOW);
digitalWrite(stepPin, LOW);
digitalWrite(dirPin, rot1);
}

void loop() {
  // put your main code here, to run repeatedly:
  for (i = 0;i <=way; i++) {
    if (i < usk || (way - usk) < i){     
      g = abs(i-0.5*way)/100;
      int(g);
    }
    else { g = 0;}
      digitalWrite(stepPin, HIGH);
      delay(speedos+g);
      digitalWrite(stepPin, LOW);
      light = !light;
      digitalWrite(led, light);
      delay(speedos+g);
  }   
  delay(2000);
  rot1 = !rot1;
  digitalWrite(dirPin, rot1);
  
}
