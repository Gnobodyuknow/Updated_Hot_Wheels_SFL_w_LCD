
//By Eddie Studer - Oct 2020
//Diecast electronic finish line
//Time starts when gate is opened.
//First car to break IR beam is the lane winner
//Displays Elapsed time and winner on a 16x2 LCD w I2C.
//times resets when start gate is closed.
//Calculated time is in microseconds and gets converted to Seconds when displayed. 
//Races can be that close.
//Uses Arduino Nano, 


#include <FastGPIO.h>
#include <hd44780.h>
#include <LiquidCrystal.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2); // double check your LCD address first using: i2C_scanner

int analogOutPin1 = A1; // finish line beam lane 1 on A1 PIN
int analogOutPin2 = A2; // finish line beam lane 2 on A2 PIN

const int ledPin1 = 11; //lane 1 winning LED on D11 PIN
const int ledPin2 = 12; //Lane 2 winning LED in D12 PIN
int StartSwitch = 2;

int sensorValueFinish1 = 0; 
int sensorValueFinish2 = 0;
int StartState = 0;

int sensorThresh = 500; //Sets the trigger sensing threshold of the IR receivers. ~1000 = high

unsigned long timeLane1; //finish line lane 1
unsigned long timeLane2; //finish line lane 2 
float StartTime = 0;
float ET1 = 0;
float ET2 = 0;

void setup() 
{

  Serial.begin(9600);
  pinMode(StartSwitch, INPUT);
  pinMode(ledPin1, OUTPUT); //lane 1
  pinMode(ledPin2, OUTPUT); //lane 2
  digitalWrite(ledPin1, HIGH);
  digitalWrite(ledPin2, HIGH);
  delay(1000);
  digitalWrite(ledPin1, LOW);
  digitalWrite(ledPin2, LOW);
  StartTime = 0;
    
      lcd.begin(16,2);
      lcd.backlight();
      lcd.setCursor(3,0);
      lcd.print("GlenHollow");    
      lcd.setCursor(4,1);
      lcd.print("Raceway"); 
      delay(2000);
      lcd.setCursor(0,1);
      lcd.print("Ready to race...");   
}

void loop() 
{
  StartState = digitalRead(StartSwitch); //Check state of start gate. 0 = closed & 1 = open.
  if(StartState == 0)
  {
     StartTime = 0;
     ET1 = 0;
     ET2 = 0;
  }
  
  StartState = digitalRead(StartSwitch); //if start gate is trigger, race started. 1 = open.
  if(StartState == 1 && StartTime == 0)
  {
    StartTime = micros(); //use micro seconds since the races can be that close!
  }
  
  //read the analog in value of IR's:
  sensorValueFinish1 = analogRead(analogOutPin1);
  sensorValueFinish2 = analogRead(analogOutPin2);

   // remove "//" from below for Debugging and checking actual sensor values. 
   //             You may have to adjust sensorThresh value for better accuracy.
   //             The sensors should read around 900-1000 then go "low" (less than 400) when a car passes through the beam.
   //Serial.println(StartTime);
  // Serial.println(StartState);
  // Serial.print("Sensor Finish1 = " );
  // Serial.println(sensorValueFinish1);
  // Serial.print("Sensor Finish2 = " );
  // Serial.println(sensorValueFinish2);
  // delay(50);


// wait/check for the Finish Line sensors to be triggered
  
  if(sensorValueFinish1 < sensorThresh && ET1 == 0)
  {
    timeLane1 = micros();
    ET1 = timeLane1 - StartTime;
    ET1 = ET1 / 1000000;
  }

 if(sensorValueFinish2 < sensorThresh && ET2 == 0)
  {
    timeLane2 = micros();
    ET2 = timeLane2 - StartTime;
    ET2 = ET2 / 1000000;
  }


if (ET1 < ET2 && ET1 != 0 && ET2 != 0)// Set winner Lane 1, turn on winner LED
   {
   digitalWrite(ledPin1, HIGH);
   }

 if(ET1 > ET2 && ET2 != 0 && ET1 != 0) // Set winner Lane 2, turn on winner LED
   {
   digitalWrite(ledPin2, HIGH);
   }

 if(ET1 > 0 && ET2 > 0) // Race is over, display times for both lanes. // both cars have to finish to display times.
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("L1" );
    lcd.setCursor(4,0);
    lcd.print("ET:");
    lcd.setCursor(6,0);
    lcd.print(ET1, 4);
    if (digitalRead(ledPin1) == HIGH)
       {
         lcd.setCursor(12,0);
         lcd.print("*WIN*");
       }
    lcd.setCursor(0,1);
    lcd.print("L2" );
    lcd.setCursor(4,1);
    lcd.print("ET:");
    lcd.setCursor(6,1);
    lcd.print(ET2, 4);
      if (digitalRead(ledPin2) == HIGH)
       {
         lcd.setCursor(12,1);
         lcd.print("*WIN*");
       }
    delay(8000);
    digitalWrite(ledPin1, LOW); //turn off lane winner LED
    digitalWrite(ledPin2, LOW); //turn off lane winner LED
    timeLane1= 0; 
    timeLane2= 0; 
    StartTime = 0;
    ET1 = 0;
    ET2 = 0;
  }

}
