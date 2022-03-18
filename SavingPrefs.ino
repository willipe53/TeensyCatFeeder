void loadPreferences(bool prefsExist) {
/*

 MyObject customVar; //Variable to store custom object read from EEPROM.
  

  Serial.println( "Read custom object from EEPROM: " );
  Serial.println( customVar.field1 );
  Serial.println( customVar.field2 );
  Serial.println( customVar.name );
}
* 
struct Prefs {
  int wrist_top;
  int elbow_top;
  int shoulder_side;
  int shoulder_front;
  int jiggles;
  int wake;
};
Prefs prefs = { 2500, 2250, 500, 1400, 3, 5 };
* 
* 
* 
 * 
 */
  
  EEPROM.get(0, prefs);
  if (prefs.wake == 0) {
    Serial.printf("Detected 0 for wake...\n");
    prefsExist = false;
  }
  if (!prefsExist) writeCurrentValsToPrefs();
//  servoTop         = prefs.wrist_top;
//  servoMiddle      = prefs.getInt("m");
//  servoSide        = prefs.getInt("d");
//  servoElbowOffset = prefs.getInt("e");
//  jiggleTimes      = prefs.getInt("j");
}

void writeCurrentValsToPrefs() {
  Serial.printf("Rewriting default values\n");
//  prefs.putInt("t", servoTop);
//  prefs.putInt("m", servoMiddle);
//  prefs.putInt("d", servoSide);
//  prefs.putInt("e", servoElbowOffset);
//  prefs.putInt("j", jiggleTimes);
}

void printValues() {
//  Serial.printf("\nCurrent Top (Wrist): %d\n", servoTop);    
//  Serial.printf("Current Middle (Shoulder): %d\n", servoMiddle);    
//  Serial.printf("Current Side (Shoulder): %d\n", servoSide);    
//  Serial.printf("Current Elbow Offset (Wrist - Elbow): %d\n", servoElbowOffset);    
//  Serial.printf("Current number of jiggles: %d\n\n", jiggleTimes);    
  Serial.printf("Available serial commands:\n");  
//  for (int i=0; i<inputChars.length(); i++) {    
//    String key = inputChars.substring(i,i+1);
//    Serial.printf("  %s - %s\n", key.c_str(), outputNames[i].c_str());  
//  }
}

void saveJiggleTimes(int valu) {
//  jiggleTimes = valu;
  writeCurrentValsToPrefs();            
}

void saveNewOpenPosition() {
//EEPROM.put( eeAddress, customVar );
//servoTop = servoMotor[WRIST].readMicroseconds();
//  servoSide = servoMotor[SHOULDER].readMicroseconds();
//  servoElbowOffset = servoMotor[WRIST].readMicroseconds() - servoMotor[ELBOW].readMicroseconds();
//  writeCurrentValsToPrefs();         
}
