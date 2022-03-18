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
  
//default servo positions
int servoTop         = 2500;
int servoBottom      = 1400;
int servoMiddle      = 1400;
int servoSide        =  500;
int servoElbowOffset =  250;
int jiggleTimes      =    3;
int wakeHour         =    5;

struct Prefs {
  int servoTop;
  int servoBottom;
  int servoMiddle;
  int servoSide;
  int servoElbowOffset;
  int jiggleTimes;
  int wakeHour;
};
Prefs prefs;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1, 1000000);
Servo shoulderServo;
Servo elbowServo;
Servo wristServo;
Servo servoMotor[] = {shoulderServo, elbowServo, wristServo};

int servoPins[]    = {7, 6, 5};
String servoNames[] = {"Shoulder", "Elbow", "Wrist"};
bool servosOn = false;
int cursorPos = 130; //just offscreen
int lastButtonState = LOW;
int thisButtonState;
unsigned long pressedTime  = 0;
unsigned long releasedTime = 0;
bool isPressing = false;
bool isLongDetected = false;
int timeSetMode = 0;
char lastTimeStr[128];
bool isLocked = false;
String dow[] = {"","Sunday","Monday","Tuesday","Wednesday","Thursday","Friday"};

String inputChars = "olsewjtcbp";
String outputNames[] = {"Open the lid",                            //o
                        "Lock or unlock magnet (1 or 0)",          //l
                        "Move shoulder to x",                      //s
                        "Move elbow to x",                         //e
                        "Move wrist to x",                         //w
                        "Save number of jiggles to x",             //j
                        "Save wake time as x (24 hour clock)",     //t
                        "Save clock time as x (YYYYMMDDHHMM))",    //c
                        "Save as new open",                        //b
                        "Print all values"};                       //p

void processCommand(String cmd) {
  //commands come from AWS IoT or serial terminal
  Serial.printf("Command received: %s\n", cmd.c_str());
  String key = cmd.substring(0,1);
  int valu = cmd.substring(1,cmd.length()-1).toInt();
  if (key.equals("o")) openLid();   
  else if (key.equals("l")) toggleMagnet(valu);            
  else if (key.equals("s")) moveServo(SHOULDER, valu);   
  else if (key.equals("e")) moveServo(ELBOW, valu);   
  else if (key.equals("w")) moveServo(WRIST, valu);   
  else if (key.equals("j")) saveJiggleTimes(valu);   
  else if (key.equals("t")) saveWakeTime(valu);            
  else if (key.equals("c")) resetClockTo(valu);            
  else if (key.equals("b")) saveNewOpenPosition();   
  else if (key.equals("p")) printValues();            
  else Serial.printf("Unrecognized input: no values will be changed\n");
}

void resetClockTo(int valu) {
  //YYYYMMDDHHMM
  String cmd = String(valu);
  int yr = cmd.substring(0,4).toInt();
  int mo = cmd.substring(4,6).toInt();
  int dd = cmd.substring(6,8).toInt();
  int hr = cmd.substring(8,10).toInt();
  int mn = cmd.substring(10,12).toInt();
  setTime(hr,mn,0,dd,mo,yr);
}

time_t getTeensy3Time() {
  return Teensy3Clock.get();
}

void toggleMagnet(int valu) {
  digitalWrite(MAGNET, valu);
  isLocked = valu;
  Serial.printf("Reset magnet to %d\n", isLocked);
}

void setup()  {
  Serial.begin(9600);
  loadPreferences(true);
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(MAGNET, OUTPUT);
  toggleMagnet(0);
  setSyncProvider(getTeensy3Time);
  Serial.println("Please enter time in the form of YYYYMMDD HH:MM[AM|PM], e.g. 20200317 11:41AM");
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextSize(1);
  display.setTextColor(WHITE);
}

void loop() {
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
      if (timeSetMode > 0) Serial.printf("Advancing the setting under the cursor for time item %d.", timeSetMode);
      else if (isLocked) openLid();
      else toggleMagnet(1);
    }
  }
  if (isPressing == true && isLongDetected == false) {
    long pressDuration = millis() - pressedTime;
    if (pressDuration > PRESS_TIME) {
      Serial.println("A long press is detected");
      isLongDetected = true;
      timeSetMode = (timeSetMode == 4) ? 0 : ++timeSetMode;
      Serial.printf("Switching to time set mode %d.\n", timeSetMode);
    }
  }
  lastButtonState = thisButtonState;

  //drive display
  display.clearDisplay();
  drawTime();
  drawLock();
  drawWake();
  display.display();

  //manual testing
  if (Serial.available() > 0) processCommand(Serial.readString());

}
