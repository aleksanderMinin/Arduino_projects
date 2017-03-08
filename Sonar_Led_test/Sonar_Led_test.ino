int echoPin = 8; 
int trigPin = 9; 
int mm = 0;
int val;
int stepin = 25;

 #include <PololuLedStrip.h>
 // Create an ledStrip object and specify the pin it will use.
PololuLedStrip<10> ledStrip;

// Create a buffer for holding the colors (3 bytes per color).
#define LED_COUNT 60
rgb_color colors[LED_COUNT];


void setup() { 
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT); 

  // Start up the serial port, for communication with the PC.
  Serial.begin(115200);
  Serial.println("Ready to receive colors.");
  
} 
 
void loop() { 
  int duration, cm; 
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2); 
  digitalWrite(trigPin, HIGH); 
  delayMicroseconds(10); 
  digitalWrite(trigPin, LOW); 
  duration = pulseIn(echoPin, HIGH); 
  cm = duration / 5;
  Serial.print(cm); 
  Serial.println(" cm"); 
  
  //mm = mm % 765;
  val = mm - map(abs(cm),1,3000,0,765);
  if (abs(val) < 50)
    {
      mm = map(abs(cm),1,3000,0,765);
    }
  else
    { val = map(abs(cm),1,3000,0,765);
      if ( mm > val )
        { 
          mm = mm - stepin;          
        }
      else 
        { 
          
          mm = mm + stepin;          
        }
    }
  Serial.print(mm); 
  Serial.println(" = map mm"); 
  
  rgb_color color;
  
  if (mm > 510)
    {color.blue = 255;
     color.green = 255;
     color.red = mm - 510;  
     }
   else 
    if ( mm > 255)
      { color.green = 255;
        color.blue = mm - 255;
        color.red = 0;
      }
      else 
        {color.green = mm;
        color.blue = 0;
        color.red = 0;
        }

  Serial.print("Showing color: ");
  Serial.print(color.red);
  Serial.print(",");
  Serial.print(color.green);
  Serial.print(",");
  Serial.println(color.blue);
  
  //формирование цветов для диодов
  for(uint16_t i = 0; i < (LED_COUNT / 2); i++)
  {
    colors[LED_COUNT / 2 + i] = color;
    colors[LED_COUNT / 2 - i] = color;
    if (color.blue >= 10) 
      {color.blue = color.blue - 10 ;}
    else 
      {color.blue = 0;};
    if (color.green >= 10) 
      {color.green = color.green - 10;}
    else 
      {color.green = 0;};
    if (color.red >= 10) 
      {color.red = color.red - 10;}
    else 
      {color.red = 0;}
  }

  // Отправка данных диодам.
  ledStrip.write(colors, LED_COUNT);  
  delay(40);
}
