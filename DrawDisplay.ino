void drawLock() {
  if (!displayAwake) return;
  display.fillRoundRect(0, 5, 15, 15, 3, WHITE);
  if (isLocked) {
    display.drawCircle(7, 4, 4, WHITE);
  } else {
    display.drawCircle(15, 4, 4, WHITE);
    display.fillRect(15, 5, 6, 10, BLACK);    
  }
}

void drawWake() {
  if (!displayAwake) return;
  display.setTextColor(WHITE);
  display.setCursor(20, 8);
  if (wakeTime == 0) {
    display.print("Vacation mode");
  } else {
    display.print("Next food at ");
    if (timeSetMode == TS_WAKE) display.setTextColor((second() % 2) ? BLACK : WHITE);
    int wake = getWakeHour();
    if (wake > 12) {
      wake -= 12;
      display.print(wake);
      display.print("PM");
    } else {
      display.print(wake);
      display.print("AM");    
    }
  }
}

void drawTime() {
  if (!displayAwake) return;
  display.setTextColor(WHITE);
  char timeStr[128];
  sprintf(timeStr, "%2d:%02d:%02d%sM", hourFormat12(), minute(), second(), (isAM() ? "A" : "P"));
  int16_t  x1, y1;
  uint16_t w, h;
  if (strcmp(lastTimeStr, timeStr) == 0) return;
  display.getTextBounds(timeStr, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((128-w)/2, 22);
  if (timeSetMode == TS_HOUR) display.setTextColor((second() % 2) ? BLACK : WHITE);
  display.print(hourFormat12());
  display.setTextColor(WHITE);
  display.print(":");
  if (timeSetMode == TS_MIN) display.setTextColor((second() % 2) ? BLACK : WHITE);
  if (minute()<10) display.print(0);
  display.print(minute());
  display.setTextColor(WHITE);
  display.print(":");
  if (second()<10) display.print(0);
  display.print(second());
  if (timeSetMode == TS_HOUR) display.setTextColor((second() % 2) ? BLACK : WHITE);
  display.print((isAM() ? "AM" : "PM"));
  //display.print(timeStr);  
}
