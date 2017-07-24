/* программа для управления спаркой вентиляторов радиатора
  УАЗ патриот с радиатором от TLC100
  при включании запускаются вентиляторы на X сек для проверки работы
  включается одна обмотка одного вентилятора при 80
  при 85* включается один на полную
  при 90 включается второй на полную
  2016год
  ячейка EEPROM[0..8] - точки температуры
  ячейка EEPROM[9] - флаг вкл/выкл вентиляторы в начале пуска (check)
  ячейка EEPROM[10] - время работы в первое включение          (cooltime)
  ячейка EEPROM[11] - время работы основного цикла программы  (delaytime)
*/
#include <Scheduler.h>
#include <HID.h>
#include <EEPROM.h>
#include "mcp_can.h"
#include <SPI.h>
#include <OneWire.h>

byte manual = 0;       // ручное управление
String sms;                 // сообщение с терминала
String smsbuffer = "";
int relayPin[5] = {0, 5, 6, 7, 8}; // пины на реле 1,2,3,4
int analogPin = 0;          // пин аналоговый вход для датчика температуры
boolean smsResult = false;  // результат правильного сообщения с терминала
int temp_val;               // временная переменная для вычисления
int temp[9];                // границы вкл/выкл температуры(точки) (0 - лишний)
bool relays[5];             // состояние реле
int temp_toj;               // значение с датчика температуры
byte i;                     // переменная для циклов
int delaytime;              // время задержки основного цикла работы EEPROM 11
OneWire ds(4);              // пин датчика температуры

const byte averageFactor = 5;   // коэффициент сглаживания показаний (0 не ставить!)
                                // чем выше, тем больше "инерционность"
//шота для обмена по шине CAN
long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];

MCP_CAN CAN0(10);	// Set CS to pin 10

void setup() {
  pinMode(analogPin, INPUT); //инициализация пина с термодатчиком
  Serial.begin(115200);      // открывает последовательный порт, устанавливает скорость 9600 бит
  //Serial.setTimeout(20);
  CAN0.begin(CAN_500KBPS);   // init can bus : baudrate = 500k
  pinMode(13, OUTPUT);
  pinMode(9, INPUT);         //пин 9 INT MCP модуля
  // pinMode(4,INPUT);       // пин 4 датчика температуры
  for (i = 1; i <= 4; i++)   // инициализация пинов реле
    pinMode(relayPin[i], OUTPUT);

  // реле на LOW,  будут включать вентилятор
  // реле на HIGH,  будут выключать вентилятор

  delaytime = EEPROM.read(11) * 1000;

  //вкл/выкл вентиляторы в начале пуска
  if (EEPROM.read(9) == 1) {
    RelayState(1, 1, 1, 1);        //включаем все вентиляторы
	delay(EEPROM.read(10) * 1000); //время работы в начале пуска(cooltime)

    /* отключаем вентиляторы последовательно для проверки
      и выявления неисправного*/
    for (i = 1; i <= 4; i++) {
      digitalWrite(relayPin[i], HIGH);
      delay(1000);
    }
  }

  for (i = 1; i <= 8; i++) //считываем точки температуры из постоянной памяти
    temp[i] = EEPROM.read(i);

  Scheduler.startLoop(loopTalk);
  Scheduler.startLoop(loopTempToj);
  //Scheduler.startLoop(loopTemp);
  //Scheduler.startLoop(loopMCPRecive);
}

// перезагрузка
void(* resetFunc) (void) = 0;

/* Общение с внешним миром
  без возвращения перем.
  Команда - РЕЗУЛЬТАТ
  tmpNtoj   шаблон сообщения для задания точки температуры
     N      порядковый номер точки температуры от 1 до check8
      toj   трех значное значение температуры
  ram       получить значение точек температур действующий
  flash     получить значение точек температур в постоянной памяти
  help      вывод справки
  reset     программная перезагрука модуля
  on        включить вентиляторы на полную постоянно
  off       выключить вентиляторы постоянно
  no        штатный режим
  cooltimeX установить время "X" работы вентиляторов при начале работы (проверка работы) от 1 до 9сек!
  0         узначть значение cooltime в eeprom
  check     флаг вкл/выкл вентиляторы в начале пуска
  manualXX  включить вручную в определенном состоянии 01..4 отдельные реле. 05-14 для приложения
*/
//xxxMmMmMmCdC
//234567890123
//2-------3---
void loopTalk() {
  if (Serial.available() > 0 || smsbuffer.length() > 0) {   //если есть сообщение

	  //добавляем сообщения в буфер
    smsbuffer = smsbuffer + char('\n') + Serial.readString();

    //выбираем из буфера строку
	  sms = "";
    while ( byte(smsbuffer[0]) != 10 && smsbuffer.length() > 0){
      sms = sms + smsbuffer[0];
      smsbuffer.remove(0,1);
    }

    if ( smsbuffer[0] == char('\n') ) //убираем перенос после строки
      smsbuffer.remove(0,1);

    Serial.print("I get->");
    Serial.println(sms);
    smsResult = false;

    if (sms.substring(0, 2) == "on") {    // включаем все реле
      manual = -1;
      Serial.println("manual on");
	    RelayState(1, 1, 1, 1);
      smsResult = true;
    }
    if (sms.substring(0, 3) == "off") { // выключаем все реле
      manual = -2;
      Serial.println("manual off");
	    RelayState(0, 0, 0, 0);
      smsResult = true;
    }
    if (sms.substring(0, 2) == "no") {
  		manual = 0;
  		Serial.println("manual no");
  		smsResult = true;
  	}
    // 14 положений отдельного включения
    if (sms.substring(0, 6) == "manual") {
  		manual = (int(sms[7]) - 48) * 10 + int(sms[6]) - 48;
        switch (manual) {
          case 1:
            RelayState(1,0,0,0);
            break;
          case 2:
            RelayState(0,1,0,0);
            break;
          case 3:
            RelayState(0,0,1,0);
            break;
          case 4:
            RelayState(0,0,0,1);
            break;
          case 5:
            RelayState(1,1,0,0);
            break;
          case 6:
            RelayState(1,0,1,0);
            break;
          case 7:
            RelayState(1,0,0,1);
            break;
          case 8:
            RelayState(0,1,1,0);
            break;
          case 9:
            RelayState(0,1,0,1);
            break;
          case 10:
            RelayState(0,0,1,1);
            break;
          case 11:
            RelayState(1,1,1,0);
            break;
          case 12:
            RelayState(1,1,0,1);
            break;
          case 13:
            RelayState(1,0,1,1);
            break;
          case 14:
            RelayState(0,1,1,1);
            break;
          default:
            manual = 0;
          break;
        }
  	}
    //задание точки температуры
    //N - порядковый номер точки, ххх - трехзначное число с датчика температуры
    //tmpNxxx - set tempreture of dot, N - nomber of dot, xxx - temp of dot
    if (sms.substring(0, 3) == "tmp") { //записываем значение в eeprom для первого реле
      //записываем в память значения из терминала
      EEPROM.write(int(sms[3]) - 48, byte((int(sms[4]) - 48) * 100 + (int(sms[5]) - 48) * 10 + (int(sms[6]) - 48)));
      smsResult = true;
    }
    if (sms.substring(0, 5) == "flash") {  //вывод в терминал из памяти сохраненных точек
      Serial.println("Thats what in EEPROM now:");
      for (i = 1; i <= 8; i++) {
        Serial.print("temp_");
        Serial.print(i);
        Serial.print("_");
        Serial.println(EEPROM.read(i));
        delay(150);
      }
      Serial.print("Check in start -> ");
      Serial.println(EEPROM.read(9));
      delay(150);
      Serial.print("cooltime -> ");
      Serial.println(EEPROM.read(10));
      delay(150);
      Serial.print("delaytime -> ");
      Serial.println(EEPROM.read(11));
      smsResult = true;
    }
    if (sms.substring(0, 3) == "ram") {  //вывод в терминал из оперативки сохраненных точек
      Serial.println("Thats what in RAM now:");
      for (i = 1; i <= 8; i++) {
        Serial.print("temp_");
        Serial.print(i);
        Serial.print("->");
        Serial.println(temp[i]);
        delay(150);
      }
      smsResult = true;
    }
    if (sms.substring(0, 5) == "check") {
      if (EEPROM.read(9) != 0) {
        EEPROM.write(9, 0);
        Serial.print("Check now ->");
        Serial.println(EEPROM.read(9));
      }
      else {
        EEPROM.write(9, 1);
        Serial.print("Check now ->");
        Serial.println(EEPROM.read(9));
      }
      smsResult = true;
    }
    if (sms.substring(0, 8) == "cooltime") {
      if (sms[8] == '0')  {
        Serial.print("cooltime now -> ");
        Serial.println(EEPROM.read(10));
      }
      if (sms[8] > '0' && sms[8] <= '9') {
        EEPROM.write(10, byte(int(sms[8]) - 48));
        Serial.print("cooltime now -> ");
        Serial.println(EEPROM.read(10));
      }
      else
        Serial.println("cooltime time must be [0..9] ");
      smsResult = true;
    }
    if (sms.substring(0, 9) == "delaytime") {
      if (sms[9] == '0') {
        Serial.print("delaytime now -> ");
        Serial.println(delaytime);
      }
      if (sms[9] > '0' && sms[9] <= '9') {
        EEPROM.write(11, byte((int(sms[9]) - 48)));
        delaytime = EEPROM.read(11) * 1000;
        Serial.print("delaytime now -> ");
        Serial.println(EEPROM.read(11));
      }
      else
        Serial.println("delay time must be [0..9] ");
      smsResult = true;
    }
    if (sms.substring(0, 4) == "help") {
      Serial.println("on/off/no - to manual switch all relay");
      Serial.println("manualN - to manual turn On only N number relay. 5-14 some variants to on");
      Serial.println("tmpNxxx - set tempreture of dot, N - number of dot, xxx - temp of dot");
      Serial.println("flash - read settings from EEPROM");
      Serial.println("ram - read dots from RAM");
      Serial.println("check - chenge on/off relay in start");
      Serial.println("cooltimeN - set N [1..9]sec on/off relay in start, 0 - read this time");
      Serial.println("delaytimeN - set N [1..9]sec to delay in work, 0 - read this time");
      Serial.println("reset - reset module");
      smsResult = true;
    }
    if (sms.substring(0, 5) == "reset") {
      resetFunc();
      smsResult = true;
    }
    if (!smsResult) {
      Serial.println("not a standard message!");
      Serial.println("print help for help");
    }
  }
  yield();
}

// вывод температуры в распред коробке
void Temp() {
  byte data[2];
  ds.reset();
  ds.write(0xCC);
  ds.write(0x44);
  delay(100);
  ds.reset();
  ds.write(0xCC);
  ds.write(0xBE);
  data[0] = ds.read();
  data[1] = ds.read();
  int Temp = (data[1] << 8) + data[0];
  Temp = Temp >> 4;
  Serial.print("Temperature ");
  Serial.println(Temp);
  yield();
}

// MCP CAN  BUS
void loopMCPRecive(){
  if (!digitalRead(2))                        // If pin 2 is low, read receive buffer
  {
    CAN0.readMsgBuf(&len, rxBuf);              // Read data: len = data length, buf = data byte(s)
    rxId = CAN0.getCanId();                    // Get message ID
    Serial.print("ID: ");
    Serial.print(rxId, HEX);
    Serial.print("  Data: ");
    for (int i = 0; i < len; i++)             // Print each byte of the data
    {
      if (rxBuf[i] < 0x10)                    // If data byte is less than 0x10, add a leading zero
      {
        Serial.print("0");
      }
      Serial.print(rxBuf[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
  yield();
}

// включение реле если 1 то включает
void RelayState(boolean r1, boolean r2, boolean r3, boolean r4) {
  if (r1) digitalWrite(relayPin[1], LOW);
  else digitalWrite(relayPin[1], HIGH);

  if (r2) digitalWrite(relayPin[2], LOW);
  else digitalWrite(relayPin[2], HIGH);

  if (r3) digitalWrite(relayPin[3], LOW);
  else digitalWrite(relayPin[3], HIGH);

  if (r4) digitalWrite(relayPin[4], LOW);
  else digitalWrite(relayPin[4], HIGH);
}

void loopTempToj() {
  int oldtemp_toj = temp_toj;
  temp_toj = analogRead(analogPin);
  // усреднение значение с датчика температуры ОЖ
  temp_toj = (oldtemp_toj * (averageFactor - 1) + temp_toj) / averageFactor;
  yield();
}

void loop() {

  sms = "toj_temp_";
  sms += temp_toj;
  Serial.println(sms);

  //все выключено = прогрев
  //if (temp >= 0 && temp < 82 && hardon != 1) {
  if (temp_toj >= temp[1] && manual == 0) RelayState(0, 0, 0, 0);

  // включения 1 обмотки 1го мотора = лёгкий обдув
  //if (temp >= 82 && temp < 85 && hardon != 1) {
  if (temp_toj >= temp[3] && temp_toj < temp[2] && manual == 0) RelayState(1, 0, 0, 0);

  // включает 1 мотор полность
  //if (temp >= 85 && temp < 87 && hardon != 1) {
  if (temp_toj >= temp[4] && temp_toj < temp[6] &&  manual == 0) RelayState(1, 1, 0, 0);

  // включает 1 мотор полность и половину второго
  //if (temp >= 87 && temp < 90 && hardon != 1) {
  if (temp_toj >= temp[7] && temp_toj < temp[6] &&  manual == 0) RelayState(1, 1, 1, 0);

  // включает два мотора на полную
  //if (temp >= 90) {
  if (temp_toj < temp[8] &&  manual == 0) RelayState(1, 1, 1, 1);

  delay(delaytime); // таймаут
}
