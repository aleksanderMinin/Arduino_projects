/* программа для управления спаркой вентиляторов радиатора 
 * УАЗ патриот с радиатором от TLC100
 * при включании запускаются вентиляторы на 2 сек для проверки работы
 * включается одна обмотка одного вентилятора при 80*
 * при 85* включается один на полную
 * при 90 включается второй на полную
 * 2016год 
 */
int hardon = 0;             // флаг ручного включения вентиляторов на максимум
int sms;                    // сообщение с ПК
int analogPin = 7;          // аналоговый вход для датчик температуры
int temp;                   // температура ОЖ
// the setup function runs once when you press reset or power the board
void setup() {
  // инициализация пинов реле
  pinMode(13, OUTPUT);
  pinMode(9, OUTPUT);   //реле 1
  pinMode(10, OUTPUT);  //реле 2
  pinMode(11, OUTPUT);  //реле 3
  pinMode(12, OUTPUT);  //реле 4
  // реле на HIGH  будут включать вентилятор
  digitalWrite(9, LOW);   //первая обмотка вентилятора 1
  digitalWrite(10, LOW);  //вторая обмотка вентилятора 1
  digitalWrite(11, LOW);  //первая обмотка вентилятора 2
  digitalWrite(12, LOW);  //вторая обмотка вентилятора 2
  Blick(4000);
  // отключаем вентиляторы последовательно для проверки и выявления неисправного узла
  digitalWrite(9, HIGH);
  Blick(1000);
  digitalWrite(10, HIGH);
  Blick(1000);
  digitalWrite(11, HIGH);
  Blick(1000);
  digitalWrite(12, HIGH);
  Blick(1000);

  analogReference(EXTERNAL);  // определение опорного напряжения от 0 до 5 вольт!!!
  Serial.begin(9600); // открывает последовательный порт, устанавливает скорость 9600 бит
}

int Blick(int time) {
  delay(time/2);
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(time/2);              // wait for a time
  digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
}


/*//ф-ия без возвращения перем.
 * если послать 1 включатся вентиляторы на полную
 * выключить можно выключив зажигание или вручную
 * если послать 0 то войдут в штатный режим
*/
void Talk(){
  if (Serial.available() > 0) {
    sms = Serial.read()- '0';
    if (sms == 1) {     // включаем вентиляторы
      hardon = 1;     
      Serial.println("hardon now");
      Serial.println(hardon);
      digitalWrite(9, LOW);
      digitalWrite(9, LOW);
      digitalWrite(9, LOW);
      digitalWrite(9, LOW);
    }
    else if (sms == 0) {  // выключаем вентиляторы
        hardon = 0;
        Serial.println("hardon now");
        Serial.println(hardon);
        digitalWrite(9, HIGH);
        digitalWrite(10, HIGH);
        digitalWrite(11, HIGH);
        digitalWrite(12, HIGH);
        Blick(1000);       
      }
      else
      {
        Serial.println("Invalid!");
      } 
    }
}

// the loop function runs over and over again forever
void loop() {

  Serial.println(hardon);
  Talk();   //читаем сообщение с ПК
  analogReference(EXTERNAL);                       //прикидываем какое бортовое напряжение
  temp = map (analogRead(analogPin),0,1023,0,115); //считываем температуру и переводим из битов в *С
  Serial.println(temp);
  
  if (temp >= 0 && temp < 75 && hardon != 1) {
      digitalWrite(9, HIGH);
      digitalWrite(10, HIGH);
      digitalWrite(11, HIGH);
      digitalWrite(12, HIGH);
  }
     
  if (temp >= 80 && temp < 85 && hardon != 1) {
      digitalWrite(9, LOW);
      digitalWrite(10, HIGH);
      digitalWrite(11, HIGH);
      digitalWrite(12, HIGH);
    }
       
  if (temp >= 90 && temp < 94 && hardon != 1) {
    digitalWrite(9, LOW);
    digitalWrite(10, LOW);
    digitalWrite(11, HIGH);
    digitalWrite(12, HIGH);
  }  
          
  if (temp >= 95) {
    digitalWrite(9, LOW);
    digitalWrite(10, LOW);
    digitalWrite(11, LOW);
    digitalWrite(12, LOW);
  }
    
  Blick(500);
}
