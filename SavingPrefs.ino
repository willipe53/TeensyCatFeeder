void loadPreferences(bool prefsExist) {
  EEPROM.get(0, prefs);
  if (prefs.wakeHour <= 0) {
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
  wakeHour         = prefs.wakeHour;
}

void writeCurrentValsToPrefs() {
  Serial.printf("Rewriting default values\n");
  prefs.servoTop         = servoTop;
  prefs.servoBottom      = servoBottom;
  prefs.servoMiddle      = servoMiddle;
  prefs.servoSide        = servoSide;
  prefs.servoElbowOffset = servoElbowOffset;
  prefs.jiggleTimes      = jiggleTimes;
  prefs.wakeHour         = wakeHour;
  EEPROM.put(0, prefs);
}

void printValues() {
  Serial.printf("\nCurrent Top (Wrist): %d\n", servoTop);    
  Serial.printf("Current Bottom (Wrist): %d\n", servoBottom);    
  Serial.printf("Current Middle (Shoulder): %d\n", servoMiddle);    
  Serial.printf("Current Side (Shoulder): %d\n", servoSide);    
  Serial.printf("Current Elbow Offset (Wrist - Elbow): %d\n", servoElbowOffset);    
  Serial.printf("Current wake hour: %d\n\n", wakeHour);    
  Serial.printf("Current number of jiggles: %d\n\n", jiggleTimes);    
  Serial.printf("Available serial commands:\n");  
  for (int i=0; i<inputChars.length(); i++) {    
    String key = inputChars.substring(i,i+1);
    Serial.printf("  %s - %s\n", key.c_str(), outputNames[i].c_str());  
  }
}

void saveJiggleTimes(int valu) {
  jiggleTimes = valu;
  writeCurrentValsToPrefs();
}

void saveWakeTime(int valu) {
  wakeHour = valu;
  writeCurrentValsToPrefs();
}

void saveNewOpenPosition() {
  servoTop = servoMotor[WRIST].readMicroseconds();
  servoSide = servoMotor[SHOULDER].readMicroseconds();
  servoElbowOffset = servoMotor[WRIST].readMicroseconds() - servoMotor[ELBOW].readMicroseconds();
  writeCurrentValsToPrefs();         
}
