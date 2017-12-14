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

typedef struct
{
  Time::Day day;
  uint8_t hour;
  uint8_t minute;
}  feedingtime;

//defining all important thingies
DS1302 rtc(CLK_RST, CLK_IO, CLK_SCLK);
Servo servo;
IRrecv irrecv(IR);
int water_level;

//defining model parameters
const int water_full = 600;
const int water_empty = 300;
int food_refill_time_ms = 1000; //miliseconds

feedingtime feedingcalendar[10]; //this is supposed to be an array with feeding dates
                                 //and i really don't know if this will work

feedingtime feedingtest;
String dayAsString(const Time::Day day) {
  switch (day) {
    case Time::kSunday: return "Sunday";
    case Time::kMonday: return "Monday";
    case Time::kTuesday: return "Tuesday";
    case Time::kWednesday: return "Wednesday";
    case Time::kThursday: return "Thursday";
    case Time::kFriday: return "Friday";
    case Time::kSaturday: return "Saturday";
  }
  return "(unknown day)";
}
void setup()
{
  servo.attach(SERVO);
  pinMode(PUMP, OUTPUT);
  pinMode(SERVO_ENABLE, OUTPUT);
  irrecv.enableIRIn();
  rtc.halt(false);
  //following things are here only for development testing purposes
  feedingtest.day = Time::kWednesday;
  feedingtest.hour = 22;
  feedingtest.minute = 20;
  Serial.begin(9600);
  Serial.println("halo halo");
}

void loop()
{
   Time t = rtc.time();

  // Name the day of the week.
  const String day = dayAsString(t.day);

  // Format the time and date and insert into the temporary buffer.
  char buf[50];
  snprintf(buf, sizeof(buf), "%s %04d-%02d-%02d %02d:%02d:%02d",
           day.c_str(),
           t.yr, t.mon, t.date,
           t.hr, t.min, t.sec);

  // Print the formatted string to serial so we can see the time.
  Serial.println(buf);

  Serial.println("should we feed bro?");
  if(isfeedingtime(feedingtest))
    Serial.println("Yeaaah");
   else
    Serial.println("Nope.");
    delay(2000);
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

bool isfeedingtime(feedingtime ftime)
{
  Time currenttime = rtc.time();
  if((currenttime.day == ftime.day)
    &&(currenttime.hr == ftime.hour)
    &&(currenttime.min == ftime.minute)) return true;
    else return false;
}

