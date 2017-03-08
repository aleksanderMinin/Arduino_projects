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
int relay1Pin = 16;
int relay2Pin = 17;
int relay3Pin = 15;
int relay4Pin = 18;
int analogPin = 0;          // аналоговый вход для датчик температуры
int temp;                   // температура ОЖ

void setup() {
  // инициализация пинов реле
  pinMode(13, OUTPUT);
  pinMode(relay1Pin, OUTPUT);  //реле 1
  pinMode(relay2Pin, OUTPUT);  //реле 2
  pinMode(relay3Pin, OUTPUT);  //реле 3
  pinMode(relay4Pin, OUTPUT);  //реле 4
  //
  // реле на HIGH,  будут выключать вентилятор
  digitalWrite(relay1Pin, LOW);  //первая обмотка вентилятора 1
  digitalWrite(relay2Pin, LOW);  //вторая обмотка вентилятора 1
  digitalWrite(relay3Pin, LOW);  //первая обмотка вентилятора 2
  digitalWrite(relay4Pin, LOW);  //вторая обмотка вентилятора 2
  Blick(4000);
  //
  // отключаем вентиляторы последовательно для проверки и выявления неисправного узла
  digitalWrite(relay1Pin, HIGH);
  Blick(1000);
  digitalWrite(relay2Pin, HIGH);
  Blick(1000);
  digitalWrite(relay3Pin, HIGH);
  Blick(1000);
  digitalWrite(relay4Pin, HIGH);
  Blick(1000);
    
  //analogReference(EXTERNAL); // определение опорного напряжения от 0 до 5 вольт!!!
  pinMode(analogPin,INPUT);  //инициализация пина с термодатчиком
  Serial.begin(9600);        // открывает последовательный порт, устанавливает скорость 9600 бит
}

//вместо дэлай с миганием)
int Blick(int time) {
  delay(time/2);
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(time/2);            // wait for a time
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
      Serial.print("hardon now ");
      Serial.println(hardon);
      digitalWrite(relay1Pin, LOW);
      digitalWrite(relay2Pin, LOW);
      digitalWrite(relay3Pin, LOW);
      digitalWrite(relay4Pin, LOW);
    }
    else if (sms == 0) {  // выключаем вентиляторы
        hardon = 0;
        Serial.print("hardon now ");
        Serial.println(hardon);
        digitalWrite(relay1Pin, HIGH);
        digitalWrite(relay2Pin, HIGH);
        digitalWrite(relay3Pin, HIGH);
        digitalWrite(relay4Pin, HIGH);
        Blick(1000);       
      }
      else
      {
        Serial.println("Invalid!");
      } 
    }
}

void loop() {

  Serial.println(hardon);   // вывод на ПК состояния ручного управления
  Talk();                   //читаем сообщение с ПК
//  temp = map (analogRead(analogPin),250,1023,0,140); //считываем температуру и переводим из битов в *С
  temp = analogRead(analogPin);
  Serial.println(temp);   //вывод температуры в консоль
  
  //if (temp >= 0 && temp < 75 && hardon != 1) {  //все выключено = прогрев
  if (temp >= 90 && hardon != 1) {  //все выключено = прогрев
      digitalWrite(relay1Pin, HIGH);
      digitalWrite(relay2Pin, HIGH);
      digitalWrite(relay3Pin, HIGH);
      digitalWrite(relay4Pin, HIGH);
  }
     
  //if (temp >= 80 && temp < 85 && hardon != 1) { // включения 1 обмотки 1го мотора = лёгкий обдув
  if (temp >= 70 && temp < 80 && hardon != 1) { // включения 1 обмотки 1го мотора = лёгкий обдув
      digitalWrite(relay1Pin, LOW);
      digitalWrite(relay2Pin, HIGH);
      digitalWrite(relay3Pin, HIGH);
      digitalWrite(relay4Pin, HIGH);
    }
       
  //if (temp >= 90 && temp < 94 && hardon != 1) { // включает 1 мотор полность в случае нагрева
  if (temp >= 57 && temp < 65 && hardon != 1) { // включает 1 мотор полность в случае нагрева
    digitalWrite(relay1Pin, LOW);
    digitalWrite(relay2Pin, LOW);
    digitalWrite(relay3Pin, HIGH);
    digitalWrite(relay4Pin, HIGH);
  }  
          
  //if (temp >= 95) {             // включает два мотора на полную
  if (temp < 57) {             // включает два мотора на полную
    digitalWrite(relay1Pin, LOW);
    digitalWrite(relay2Pin, LOW);
    digitalWrite(relay3Pin, LOW);
    digitalWrite(relay4Pin, LOW);
  }
    
  Blick(1000);
}
