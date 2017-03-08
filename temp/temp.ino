//считывание температуры с 0го пина и передача значения в терминал
int tempPin = 0;
int temp = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(tempPin,INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  temp = analogRead(tempPin);
  Serial.println(temp);
  delay(1000);
}
