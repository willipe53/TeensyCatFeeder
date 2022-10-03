#include <EEPROM.h>
#include <TimeLib.h>
#include <Servo.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <EEPROM.h>

#define SCREEN_WIDTH  128 // pixels
#define SCREEN_HEIGHT  32 
#define PRESS_TIME   1000 // milliseconds
#define BUTTON         17 // GPIO
#define MAGNET          3
#define SHOULDER        0 //array index
#define ELBOW           1
#define WRIST           2
#define TS_NONE         0
#define TS_WAKE         1
#define TS_HOUR         2
#define TS_MIN          3
#define DIM_SCREEN     20 //after 20 seconds
 
//default preferences
int servoTop         = 2500; //milliseconds
int servoBottom      = 1400;
int servoMiddle      = 1400;
int servoSide        =  500;
int servoElbowOffset =  250;
int jiggleTimes      =    3;
int wakeTime         =  500; //5AM, wake time of 0 means vacation mode, no feeding

struct Prefs {
  int servoTop;
  int servoBottom;
  int servoMiddle;
  int servoSide;
  int servoElbowOffset;
  int jiggleTimes;
  int wakeTime;
};
Prefs prefs;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1, 1000000);
Servo shoulderServo;
Servo elbowServo;
Servo wristServo;
Servo servoMotor[] = {shoulderServo, elbowServo, wristServo};

int servoPins[]     = {7, 6, 5};
String servoNames[] = {"Shoulder", "Elbow", "Wrist"};
char lastTimeStr[128];
int cursorPos = 130; //just offscreen
int lastButtonState = LOW;
int thisButtonState;
int timeSetMode = TS_NONE;
unsigned long pressedTime  = 0;
unsigned long releasedTime = 0;
unsigned long sleepDisplayAt;
bool servosOn = false;
bool isPressing = false;
bool isLongDetected = false;
bool isLocked = false;
bool displayAwake = true;
bool doneForToday = false;

String inputChars = "olsewjtdcbp";
String outputNames[] = {"Open the lid",                            //o
                        "Lock or unlock magnet (1 or 0)",          //l
                        "Move shoulder to x",                      //s
                        "Move elbow to x",                         //e
                        "Move wrist to x",                         //w
                        "Save number of jiggles to x",             //j
                        "Save wake time as x (HHMM) - 0 = off",    //t
                        "Save clock date as x (YYYYMMDD))",        //d
                        "Save clock time as x (HHMM))",            //c
                        "Save as new open",                        //b
                        "Print all values"};                       //p

void processCommand(String cmd) {
  Serial.printf("Command received: %s\n", cmd.c_str());
  String key = cmd.substring(0,1);
  long valu = cmd.substring(1,cmd.length()-1).toInt();
  if (key.equals("o")) openLid();   
  else if (key.equals("l")) toggleMagnet(valu);            
  else if (key.equals("s")) moveServo(SHOULDER, valu);   
  else if (key.equals("e")) moveServo(ELBOW, valu);   
  else if (key.equals("w")) moveServo(WRIST, valu);   
  else if (key.equals("j")) saveJiggleTimes(valu);   
  else if (key.equals("t")) saveWakeTime(valu);            
  else if (key.equals("d")) resetDateTo(valu);            
  else if (key.equals("c")) resetTimeTo(valu);            
  else if (key.equals("b")) saveNewOpenPosition();   
  else if (key.equals("p")) printValues();            
  else Serial.printf("Unrecognized input: no values will be changed\n");
}

void startDisplayDimmerTimer() {
  sleepDisplayAt = millis() + (DIM_SCREEN * 1000); 
  displayAwake = true;
  Serial.printf("Now: %d, sleep set for: %d\n", millis(), sleepDisplayAt);
}

void resetDateTo(long valu) {
  String cmd = String(valu);
  int yr = cmd.substring(0,4).toInt();
  int mo = cmd.substring(4,6).toInt();
  int dd = cmd.substring(6,8).toInt();  
  setTime(hour(),minute(),0,dd,mo,yr);
}

void resetTimeTo(long valu) {
  String cmd = String(valu);
  int hr = cmd.substring(0,2).toInt();
  int mn = cmd.substring(2,4).toInt();
  setTime(hr,mn,0,day(),month(),year());
//  Serial.printf("valu:%d yr:%d mo%d dd%d hr%d mn%d\n", valu, yr, mo, dd, hr, mn);
}

time_t getTeensy3Time() {
  return Teensy3Clock.get();
}

void toggleMagnet(int valu) {
  digitalWrite(MAGNET, valu);
  isLocked = valu;
  Serial.printf("Reset magnet to %d\n", isLocked);
}

bool isWakeupTime() {
  if (wakeTime == 0) return false;
  if ((hour() == getWakeHour()) && (minute() >= getWakeMin()) && (doneForToday == false)) {
    doneForToday = true;
    return true;
  }
  return false;
}

int getWakeHour() {
  return (wakeTime / 100);
}

int getWakeMin() {
  return (wakeTime % 100);
}

void advanceDigit() {
  if (timeSetMode == TS_HOUR) {
    int hr = hour() + 1;
    if (hr > 24) hr = 1;
    setTime(hr,minute(),0,day(),month(),year());
  } else if (timeSetMode == TS_MIN) {
    int mn = minute() + 1;
    if (mn > 59) mn = 0;
    setTime(hour(),mn,0,day(),month(),year());
  } else if (timeSetMode == TS_WAKE) {
    int wake = getWakeHour() + 1;
    if (wake > 24) wake = 0;
    saveWakeTime(wake*100);  
  }
  Serial.printf("Advancing the setting under the cursor for time item %d.", timeSetMode);
}

void setup()  {
  Serial.begin(9600);
  loadPreferences(true);
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(MAGNET, OUTPUT);
  for (int i=0; i<3; i++) {
    pinMode(servoPins[i], OUTPUT);
    digitalWrite(servoPins[i], 0);
  }
  toggleMagnet(0);
  setSyncProvider(getTeensy3Time);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  printValues();
  startDisplayDimmerTimer();
}

void loop() {
  //see if it is time to open the arm
  if (isWakeupTime()) openLid();

  //advance to tomorrow, if an hour past since last feeding
  if (doneForToday && (hour() >= getWakeHour()+1)) doneForToday = false;
  
  //turn off display to avoid burn in
  if (millis() > sleepDisplayAt) displayAwake = false;
  
  //check button
  thisButtonState = digitalRead(BUTTON);
  if (lastButtonState == HIGH && thisButtonState == LOW) {        // button is pressed
    pressedTime = millis();
    isPressing = true;
    isLongDetected = false;
  } else if (lastButtonState == LOW && thisButtonState == HIGH) { // button is released
    isPressing = false;
    releasedTime = millis();
    long pressDuration = releasedTime - pressedTime;
    if (pressDuration < PRESS_TIME) {
      Serial.println("A short press is detected");
      if (!displayAwake) startDisplayDimmerTimer();
      else if (timeSetMode > TS_NONE) advanceDigit();
      else if (isLocked) openLid();
      else toggleMagnet(1);
    }
  }
  if (isPressing == true && isLongDetected == false) {
    long pressDuration = millis() - pressedTime;
    if (pressDuration > PRESS_TIME) {
      Serial.println("A long press is detected");
      isLongDetected = true;
      if (timeSetMode == TS_WAKE) {
        timeSetMode = TS_NONE;
      } else {
        timeSetMode++;
      }
      Serial.printf("Switching to time set mode %d.\n", timeSetMode);
    }
  }
  lastButtonState = thisButtonState;

  //drive display
  display.clearDisplay();
  drawLock();
  drawWake();
  drawTime();
  display.display();

  //manual testing
  if (Serial.available() > 0) processCommand(Serial.readString());

}
