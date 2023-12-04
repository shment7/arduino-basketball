#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <NewPingNew.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int buzzerPin = 12;
const int sonarPin = 13;
const int servoPin = 3;

LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
Servo servo; 
NewPingNew sonar(sonarPin, sonarPin, 200);
SoftwareSerial mySoftwareSerial(6, 5); 
DFRobotDFPlayerMini myDFPlayer;

const long TIME = 120000;
const int WIN_SCORE = 10, MAX_DIST = 17;
int dist, sound, angle, spd = 0, score = 0;
boolean up = true, isStart = true, isFinish = false;
long startGameTime;

void setup() 
{
  mySoftwareSerial.begin(9600);
  Serial.begin(115200);
  if(!myDFPlayer.begin(mySoftwareSerial)) 
  {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true);
  }
  servo.attach(servoPin); 
  myDFPlayer.setTimeOut(500);
  myDFPlayer.volume(10); 
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
  lcd.begin(16,2);
  lcd.backlight();
  pinMode(buzzerPin, OUTPUT);
  randomSeed(analogRead(0));
}

void loop() 
{ 
  if(isStart)
  {
    servo.write(90);
    delay(100);
    isStart = false;
    isFinish = false;
    score = 0;
    spd = 0;
    lcd.clear();
    lcd.print(0);
    myDFPlayer.play(13);
    startGameTime = millis();
  }
  
  if(millis() - startGameTime > TIME)
  {
    beep(2093,1000);
    isFinish = true;
    isStart = true;
    sound = random(9,14);
    myDFPlayer.play(sound);
    delay(5000);
  }

  if(!isFinish)
  {
    delay(50);                     
    dist = sonar.ping_cm(); 
    if(dist < MAX_DIST)
    {
      score++;
      spd++;
      lcd.clear();
      lcd.print(score);
      sound = random(1,9);
      myDFPlayer.play(sound);
      delay(100);
    }
    
    if(up)
    {
      angle = angle + spd;
      if(angle > 180)
      {
        up = false;
      }
      else
      {
        servo.write(angle);
        delay(10);
      }
    }
    else
    {
      angle = angle - spd;
      if(angle < 0)
      {
        up = true;
      }
      else
      {
        servo.write(angle);
        delay(10);
      }
    }
  }

  if(score >= WIN_SCORE)
  {
    isFinish = true;
    isStart = true;
    myDFPlayer.play(14);
    delay(150000);
  }
}

void beep(int frequencyInHertz, long timeInMilliseconds)
{ 
  int x;
  long delayAmount = (long)(1000000/frequencyInHertz);
  long loopTime = (long)((timeInMilliseconds*1000)/(delayAmount*2));
  for (x=0; x<loopTime; x++)
  {
    digitalWrite(buzzerPin,HIGH);
    delayMicroseconds(delayAmount);
    digitalWrite(buzzerPin,LOW);
    delayMicroseconds(delayAmount);
  }
}

