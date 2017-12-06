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


DS1302 rtc(CLK_RST, CLK_IO, CLK_SCLK);

IRrecv irrecv(IR);
decode_results results;

Servo servo;

void setup()
{
  Serial.begin(9600);
  Serial.println("Witamy w tescie");
  servo.attach(SERVO);
  pinMode(PUMP, OUTPUT);
  pinMode(SERVO_ENABLE, OUTPUT);
  irrecv.enableIRIn();
  rtc.halt(false);
  
}

void loop()
{
  printTime();
  delay(1000);
  printIR();
  delay(1000);
  testPump();
  delay(1000);
  testWS();
  delay(1000);
  testServo();
  delay(1000);
  
}


void printTime()
{
  Serial.println("testing RTC...");
  // Get the current time and date from the chip.
  Time t = rtc.time();

  // Format the time and date and insert into the temporary buffer.
  char buf[50];
  snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d",
           t.yr, t.mon, t.date,
           t.hr, t.min, t.sec);

  // Print the formatted string to serial so we can see the time.
  Serial.println(buf);
}

void printIR()
{
  Serial.println("testing IR...");
  int i = 0;
  for(i=0; i < 100; i++)
  {
  
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    irrecv.resume(); // Receive the next value
  }
  delay(10);
  }
}

void testPump()
{
  Serial.println("testing pump...");
  digitalWrite(PUMP, HIGH);
  delay(2000);
  digitalWrite(PUMP, LOW);
}

void testWS()
{
  Serial.println("testing water sensor...");
  int i;
  for(i = 0; i < 10; i++)
  {
    Serial.print("analog input value: ");
    Serial.println(analogRead(WS));
    delay(500);
  }
}

void testServo()
{
  digitalWrite(SERVO_ENABLE, HIGH);
  Serial.println("Testing servo...");
  servo.writeMicroseconds(0);
  delay(3000);
  servo.writeMicroseconds(1500);
  delay(1000);
  digitalWrite(SERVO_ENABLE, LOW);
}

