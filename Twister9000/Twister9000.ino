#include <DS1302.h> //Clock
#include <IRremote.h> //IR sensor
#include <Servo.h>

#define SERVO 9   //digital PWM servo pin
#define SERVO_ENABLE 11
#define CLK_RST 2 //three real time clock pins
#define CLK_IO 3
#define CLK_SCLK 4
#define IR 5      //IR sensor pin
#define WS A0     //water sensor analog in
#define PUMP 10   //water pump pin

struct feedingtime
{
  Time::Day day;
  uint8_t hour;
  uint8_t minute;
};

//defining all important thingies
DS1302 rtc(CLK_RST, CLK_IO, CLK_SCLK);
Servo servo;
IRrecv irrecv(IR);
int water_level;

//defining model parameters
const int water_full = 600;
const int water_empty = 300;
int food_refill_time_ms = 1000; //miliseconds

void setup()
{
  servo.attach(SERVO);
  pinMode(PUMP, OUTPUT);
  pinMode(SERVO_ENABLE, OUTPUT);
  irrecv.enableIRIn();
  rtc.halt(false);
}

void loop()
{

}

void refill_water()
{

  water_level = analogRead(WS);
  if (water_level > water_empty) return;

  digitalWrite(PUMP, HIGH);
  do
  {
    water_level = analogRead(WS);
    delay(100);
  } while (water_level < water_full);
  digitalWrite(PUMP, LOW);
}

void refill_food()
{
  digitalWrite(SERVO_ENABLE, HIGH);
  servo.writeMicroseconds(0);
  delay(food_refill_time_ms);
  servo.writeMicroseconds(1500);
  delay(1000);  //wating for sservo to stabilize
  digitalWrite(SERVO_ENABLE, LOW);
}


