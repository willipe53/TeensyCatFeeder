void drawLock() {
  if (!displayAwake) return;
  display.fillRoundRect(0, 17, 15, 15, 3, WHITE);
  if (isLocked) {
    display.drawCircle(7, 16, 4, WHITE);
  } else {
    display.drawCircle(15, 16, 4, WHITE);
    display.fillRect(15, 17, 6, 10, BLACK);    
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
  display.setCursor((128-w)/2, 0);
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

void drawWake() {
  if (!displayAwake) return;
  display.setTextColor(WHITE);
  display.setCursor(20, 22);
  display.print("Next food at ");
  if (timeSetMode == TS_WAKE) display.setTextColor((second() % 2) ? BLACK : WHITE);
  display.print("5AM");
}
