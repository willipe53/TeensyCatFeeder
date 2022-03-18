//default servo positions
int servoTop         = 2500;
int servoBottom      = 1400;
int servoMiddle      = 1400;
int servoSide        =  500;
int servoElbowOffset =  250;
int jiggleTimes      =    3;

void openLid(bool jiggle) {
  Serial.printf("Opening lid with jiggle=%d\n", jiggle);
  toggleMagnet(0);
  delay(50);
  moveElbowWrist(servoTop);
  if (jiggle) jiggleShoulder();
  killServos();
}

void jiggleShoulder() {
  Serial.printf("Jiggling shoulder\n");
  moveElbowWrist(servoTop);
  delay(500);
  for (int i=0; i<jiggleTimes; i++) {
    moveServo(SHOULDER, servoSide);
    delay(500);
    moveServo(SHOULDER, servoSide+400);
    delay(500);
  }
  moveServo(SHOULDER, servoSide);
  delay(500);
}

void moveElbowWrist(int valu) {
  moveServo(WRIST, valu);
  moveServo(ELBOW, valu - servoElbowOffset);
}

void killServos() {
  delay(500);
  for (int i=0; i<3; i++) {
    servoMotor[i].detach();    
  }
  servosOn = false;
}

void initServos() {
  for (int i=0; i<3; i++) {
//    servoMotor[i].setPeriodHertz(50); 
    servoMotor[i].attach(servoPins[i]);    
  }
  servosOn = true;
}

void moveServo(int idx, int valu) {
  toggleMagnet(0);
  if (!(servosOn)) initServos();
  servoMotor[idx].writeMicroseconds(valu);  
  //oldMSVal[idx] = MS; 
  delay(100);
  int currMS = servoMotor[idx].readMicroseconds();
  Serial.printf("Requested %dms for %s (idx %d, pin %d). Servo reading %dms\n", 
                valu, servoNames[idx], idx, servoPins[idx], currMS);
}
