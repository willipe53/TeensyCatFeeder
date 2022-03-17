void drawLock() {
  display.fillRoundRect(0, 17, 15, 15, 3, WHITE);
  if (isLocked) {
    display.drawCircle(7, 16, 4, WHITE);
  } else {
    display.drawCircle(15, 16, 4, WHITE);
    display.fillRect(15, 17, 6, 10, BLACK);    
  }
}

void drawTime() {
  display.setTextColor(WHITE);
  char timeStr[128];
  sprintf(timeStr, "%s %2d:%02d:%02d%sM", dow[weekday()].c_str(), hourFormat12(), minute(), second(), (isAM() ? "A" : "P"));
  int16_t  x1, y1;
  uint16_t w, h;
  if (strcmp(lastTimeStr, timeStr) == 0) return;
  display.getTextBounds(timeStr, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((128-w)/2, 0);
  if (timeSetMode == 1) display.setTextColor((second() % 2) ? BLACK : WHITE);
  display.print(dow[weekday()].c_str());
  display.setTextColor(WHITE);
  display.print(" ");
  if (timeSetMode == 2) display.setTextColor((second() % 2) ? BLACK : WHITE);
  display.print(hourFormat12());
  display.setTextColor(WHITE);
  display.print(":");
  if (timeSetMode == 3) display.setTextColor((second() % 2) ? BLACK : WHITE);
  if (minute()<10) display.print(0);
  display.print(minute());
  display.setTextColor(WHITE);
  display.print(":");
  if (second()<10) display.print(0);
  display.print(second());
  if (timeSetMode == 2) display.setTextColor((second() % 2) ? BLACK : WHITE);
  display.print((isAM() ? "AM" : "PM"));
  //display.print(timeStr);  
}

void drawWake() {
  display.setTextColor(WHITE);
  display.setCursor(20, 22);
  display.print("Next food at ");
  if (timeSetMode == 4) display.setTextColor((second() % 2) ? BLACK : WHITE);
  display.print("5AM");
}
