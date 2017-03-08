//simple A4988 connection
//jumper reset and sleep together
//connect  VDD to Arduino 3.3v or 5v
//connect  GND to Arduino GND (GND near VDD)
//connect  1A and 1B to stepper coil 1
//connect 2A and 2B to stepper coil 2
//connect VMOT to power source (9v battery + term)
//connect GRD to power source (9v battery - term)


int stp = 54;  //connect pin to step
int dir = 55;  // connect pin to dir
int a ;     //  gen counter

void setup() 
{                
  pinMode(stp, OUTPUT); 
  pinMode(dir, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(38, OUTPUT);
  a = 0;
  digitalWrite(38, LOW);
}

void rotate(int steps)  //вращается в одну сторону на steps шагов
{
  do
  { 
    digitalWrite(stp, HIGH);
    delay(1);
    digitalWrite(stp, LOW);
    steps--;
  } while (steps>0);
}

void loop() 
{
  
  if (a <  1)  //sweep 200 step in dir 1
   {
    digitalWrite(13, HIGH);
    digitalWrite(dir, HIGH);
    a++;
    rotate(100);
    digitalWrite(13, LOW);             
    delay(50);
   }
  else 
   {
    digitalWrite(13, HIGH);
    digitalWrite(dir, LOW);
    a++;
    rotate(200);
    digitalWrite(13, LOW);
    delay(50);
   } 
  
  if (a>1)    //sweep 200 in dir 2
     {
      a = 0;  
      delay(200);
     }
   
}
