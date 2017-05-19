/**************************************************************
wheeled-ladybug.ino
BlackBug Engineering
01.05.2017
https://github.com/dbprof/wheeled-ladybug
***************************************************************/

#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"  // подключаем библиотеку драйвера моторов
//#include <iarduino_HC_SR04_int.h>    // подключаем библиотеку для работы с датчиком
#include<Ultrasonic.h> // подключаем библиотеку для работы с датчиком

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 

Adafruit_DCMotor *RightMotor = AFMS.getMotor(2);
Adafruit_DCMotor *LeftMotor = AFMS.getMotor(1);

// объявляем объект для работы с датчиком, указывая номера arduino подключенные к выводам TRIG и ECHO датчика
//iarduino_HC_SR04_int hcsr(2,3);
Ultrasonic ultrasonic(2,3);

// объявляем переменную признака авторежима
bool isAuto;

// Создаем переменную для команд Bluetooth
char vcmd;

// Создаем переменные максимальной скорости для каждого мотора
int iMaxSpeedRM;
int iMaxSpeedLM;

// Создаем переменные для управления ультразвуковым дальномером
int echoPin = 9; 
int trigPin = 8; 

void setup() {
  Serial.begin(9600);

  AFMS.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz

  // Устанавливаем максимальную скорость моторов
  iMaxSpeedRM = 55;
  iMaxSpeedLM = 50;

  // Усатнавливаем пины для дальномера
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT); 

  // устанавливаем переменную признака авторежима
  isAuto = false;
}

// Установка максимальной разрешенной скорости
void setMaxSpeed() {
  RightMotor->setSpeed(iMaxSpeedRM);
  LeftMotor->setSpeed(iMaxSpeedLM);
}

// Установка скорости с правым приоритетом
void setMaxRSpeed() {
  RightMotor->setSpeed(iMaxSpeedRM);
  LeftMotor->setSpeed(iMaxSpeedLM/2);
}

// Установка скорости с левым приоритетом
void setMaxLSpeed() {
  RightMotor->setSpeed(iMaxSpeedRM/2);
  LeftMotor->setSpeed(iMaxSpeedLM);
}

// Движение вперед
void moveF() {
  RightMotor->run(FORWARD);
  LeftMotor->run(FORWARD);
}

// Движение назад
void moveB() {
  RightMotor->run(BACKWARD);
  LeftMotor->run(BACKWARD);
}

// Движение влево
void moveL() {
  RightMotor->run(FORWARD);
  LeftMotor->run(BACKWARD);
}

// Движение вправо
void moveR() {
  RightMotor->run(BACKWARD);
  LeftMotor->run(FORWARD);
}

// Остановка движения
void moveS() {
  RightMotor->run(RELEASE);
  LeftMotor->run(RELEASE);
}

void loop() {
  
  if (Serial.available())
  {
    vcmd = (char)Serial.read();
    
    // Включить автопилот
    if (vcmd == 'X')
    {
      isAuto = true;
    }
    // Выключить автопилот
    if (vcmd == 'x')
    {
      isAuto = false;
    }

    if (isAuto)
    {
      if (ultrasonic.Ranging(CM) >= 30)
      {
        vcmd = 'F';
      }
      else
      {
        //vcmd = 'S';
        setMaxSpeed();
        moveB();
        delay(300);
        moveR();
        delay(200);
        vcmd = 'F';
      }
    }

    //Управление программой на Android: Bluetooth RC Controller
    //F – вперед
    //B – назад
    //L – влево
    //R – вправо
    //G – прямо и влево
    //I – прямо и вправо
    //H – назад и влево
    //J – назад и вправо
    //S – стоп 
    //W – передняя фара включена
    //w – передняя фара выключена
    //U – задняя фара включена
    //u – задняя фара выключена
    //V – звуковой сигнал включен
    //v – звуковой сигнал выключен
    //X – сигнал “аварийка” включен
    //x - сигнал “аварийка” выключен
    //0 – скорость движения 0%
    //1 – скорость движения 10%
    //2 – скорость движения 20%
    //3 – скорость движения 30%
    //4 – скорость движения 40%
    //5 – скорость движения 50%
    //6 – скорость движения 60%
    //7 – скорость движения 70%
    //8 – скорость движения 80%
    //9 – скорость движения 90%
    //q – скорость движения 100% 
    
    
    // Вперед
    if (vcmd == 'F') 
    {
      setMaxSpeed();
      moveF();
    }
    // Назад
    if (vcmd == 'B')
    {
      setMaxSpeed();
      moveB();
    }
    // Влево
    if (vcmd == 'L')
    {
      setMaxSpeed();
      moveL();
    }    
    // Вправо
    if (vcmd == 'R')
    {
      setMaxSpeed();
      moveR();
    }
    
    // Вперед и влево
    if (vcmd == 'G') 
    {
      setMaxRSpeed();
      moveF();
    }
    // Вперед и вправо
    if (vcmd == 'I')
    {
      setMaxLSpeed();
      moveF();
    }
    // Назад и влево
    if (vcmd == 'H')
    {
      setMaxRSpeed();
      moveB();
    }
    // Назад и вправо
    if (vcmd == 'J')
    {
      setMaxLSpeed();
      moveB();
    }
    
    // Стоп
    if (vcmd == 'S')
    {
      moveS();
    } 
  }
}
