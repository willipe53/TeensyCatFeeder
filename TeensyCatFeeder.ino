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
  
struct Prefs {
  int wrist_top;
  int elbow_top;
  int shoulder_side;
  int shoulder_front;
  int jiggles;
  int wake;
};
Prefs prefs = { 2500, 2250, 500, 1400, 3, 5 };

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
      else if (isLocked) openLid(true);
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

  //manually set time at serial terminal    
  if (Serial.available()) {
    String cmd = Serial.readString();
    int yr = cmd.substring(0,4).toInt();
    int mo = cmd.substring(4,6).toInt();
    int dd = cmd.substring(6,8).toInt();
    int hr = cmd.substring(9,11).toInt();
    int mn = cmd.substring(12,14).toInt();
    bool am = cmd.substring(14).startsWith("A");
    if (!am) hr+=12;
    setTime(hr,mn,0,dd,mo,yr);
  }

}
