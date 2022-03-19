void loadPreferences(bool prefsExist) {
  EEPROM.get(0, prefs);
  if (prefs.wakeTime <= 0) {
    Serial.printf("Detected 0 for wake...\n");
    prefsExist = false;
  }
  if (!prefsExist) writeCurrentValsToPrefs();
  servoTop         = prefs.servoTop;
  servoBottom      = prefs.servoBottom;
  servoMiddle      = prefs.servoMiddle;
  servoSide        = prefs.servoSide;
  servoElbowOffset = prefs.servoElbowOffset;
  jiggleTimes      = prefs.jiggleTimes;
  wakeTime         = prefs.wakeTime;
}

void writeCurrentValsToPrefs() {
  Serial.printf("Rewriting default values\n");
  prefs.servoTop         = servoTop;
  prefs.servoBottom      = servoBottom;
  prefs.servoMiddle      = servoMiddle;
  prefs.servoSide        = servoSide;
  prefs.servoElbowOffset = servoElbowOffset;
  prefs.jiggleTimes      = jiggleTimes;
  prefs.wakeTime         = wakeTime;
  EEPROM.put(0, prefs);
}

void printValues() {
  Serial.println();
  Serial.printf("Servos on:%d\n", servosOn);
  Serial.printf("Time set mode:%d {TS_NONE, TS_HOUR, TS_MIN, TS_WAKE}\n", timeSetMode);
  Serial.printf("Is locked:%d\n", isLocked);
  Serial.printf("Display awake:%d\n", displayAwake);
  Serial.printf("Display will sleep at:%d (%dms from now=%d)\n", sleepDisplayAt, sleepDisplayAt-millis(), millis());
  Serial.printf("Done for today:%d\n", doneForToday);
  Serial.printf("Top (Wrist): %d\n", servoTop);    
  Serial.printf("Bottom (Wrist): %d\n", servoBottom);    
  Serial.printf("Middle (Shoulder): %d\n", servoMiddle);    
  Serial.printf("Side (Shoulder): %d\n", servoSide);    
  Serial.printf("Elbow Offset (Wrist - Elbow): %d\n", servoElbowOffset);    
  Serial.printf("Wake time: %d (%02d:%02d)\n", wakeTime, getWakeHour(), getWakeMin());    
  Serial.printf("Current time: %04d%02d%02d%02d%02d\n", year(), month(), day(), hour(), minute());
  Serial.printf("Number of jiggles: %d\n", jiggleTimes);    
  Serial.println();
  Serial.printf("Available serial commands:\n");  
  for (unsigned int i=0; i<inputChars.length(); i++) {    
    String key = inputChars.substring(i,i+1);
    Serial.printf("  %s - %s\n", key.c_str(), outputNames[i].c_str());  
  }
}

void saveJiggleTimes(int valu) {
  jiggleTimes = valu;
  writeCurrentValsToPrefs();
}

void saveWakeTime(int valu) {
  wakeTime = valu;
  writeCurrentValsToPrefs();
}

void saveNewOpenPosition() {
  servoTop = servoMotor[WRIST].readMicroseconds();
  servoSide = servoMotor[SHOULDER].readMicroseconds();
  servoElbowOffset = servoMotor[WRIST].readMicroseconds() - servoMotor[ELBOW].readMicroseconds();
  writeCurrentValsToPrefs();         
}
