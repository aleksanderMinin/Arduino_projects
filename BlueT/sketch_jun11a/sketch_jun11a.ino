int uploadpin = 7;
int resetPin = 12;

void setup()
{
  Serial.begin(115200);
// pinMode(Led, OUTPUT);
//  digitalWrite(Led, HIGH);
  digitalWrite(resetPin, HIGH);
  delay(200);
  pinMode(13, OUTPUT);
  pinMode(resetPin, OUTPUT); 
  Serial.println("reset");
  delay(200);
}
void(* resetFunc) (void) = 0;
void loop()
{
  // проверка на загрузку нового скетча
  if( Serial.peek()=='0' && millis()<600000) 
  {
    for( int i=0; i<10; i++ )  // повторить проверку
    {
      if( Serial.read()=='0' && Serial.read()==' ' )
      {
        Serial.write(0x14);  // ответить avrdude.exe
        Serial.write(0x10);  //  для синхронизации 
        //digitalWrite(uploadpin,HIGH);  // подтянуть ресет к земле
        Serial.print(3);
        delay(200);
        digitalWrite(resetPin, LOW);
        Serial.println("this never happens");
      }
      delay(10);  // небольшая задержка
      Serial.print(2);
    }
  }
  Serial.print(1);
  delay(1000);
 /* digitalWrite(Led, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(5000);              // wait for a second
  digitalWrite(Led, LOW);    // turn the LED off by making the voltage LOW
  delay(1000); */
}
