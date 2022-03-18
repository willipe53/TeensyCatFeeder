void openLid() {
  Serial.printf("Opening lid\n");
  toggleMagnet(0);
  delay(50);
  moveElbowWrist(servoTop);
  jiggleShoulder();
  killServos();
}

void jiggleShoulder() {
  Serial.printf("Jiggling shoulder\n");
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
    servoMotor[i].attach(servoPins[i]);    
  }
  servosOn = true;
}

void moveServo(int idx, int valu) {
  toggleMagnet(0);
  if (!(servosOn)) initServos();
  servoMotor[idx].writeMicroseconds(valu);  
  delay(100);
  int currMS = servoMotor[idx].readMicroseconds();
  Serial.printf("Requested %dms for %s (idx %d, pin %d). Servo reading %dms\n", 
                valu, servoNames[idx], idx, servoPins[idx], currMS);
}
