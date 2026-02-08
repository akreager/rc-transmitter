void drawHelper(){
  tft.drawRect(120, 0, 2, 320, ILI9341_YELLOW);
  tft.drawRect(0, 160, 240, 2, ILI9341_YELLOW);
}

void drawHome() {
  tft.fillScreen(ILI9341_BLACK);
  tft.fillRect(0, 0, 240, 28, ILI9341_LIGHTGREY);
  tft.drawRect(0, 0, 240, 320, ILI9341_LIGHTGREY);
  tft.drawRect(1, 1, 238, 318, ILI9341_LIGHTGREY);
  tft.setTextColor(ILI9341_BLACK);
  tft.setTextSize(3);
  tft.setCursor(69, 2);
  tft.print(F("Status"));
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(4, 30);
  tft.print(F("Batt:"));
  tft.setCursor(4, 54);
  tft.print(F("Temp1:"));
  tft.setCursor(4, 78);
  tft.print(F("Temp2:"));
  tft.setCursor(4, 102);
  tft.print(F("M1 Current:"));
  tft.setCursor(4, 150);
  tft.print(F("M2 Current:"));
}

void drawHome1() {
  tft.fillScreen(ILI9341_BLACK);
  tft.fillRect(0, 0, 240, 28, ILI9341_LIGHTGREY);
  tft.drawRect(0, 0, 240, 320, ILI9341_LIGHTGREY);
  tft.drawRect(1, 1, 238, 318, ILI9341_LIGHTGREY);
  tft.setTextColor(ILI9341_BLACK);
  tft.setTextSize(3);
  tft.setCursor(69, 2);
  tft.print(F("Status"));
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(4, 30);
  tft.print(F("Batt:"));
  tft.setCursor(4, 54);
  tft.print(F("Temp1:"));
  tft.setCursor(4, 78);
  tft.print(F("Temp2:"));
  tft.setCursor(4, 102);
  tft.print(F("M1 Current:"));
  tft.setCursor(4, 150);
  tft.print(F("M2 Current:"));
  tft.fillRect(16, 267, 216, 45, SHADOW);
  tft.fillRect(12, 263, 216, 45, ILI9341_DARKGREY);
  tft.setCursor(41, 275);
  tft.print(F("Calibrate"));
}

void drawSetupMenu() {

}

void drawThrottleCalib() {
  tft.fillScreen(ILI9341_BLACK);
  tft.fillRect(0, 0, 240, 51, ILI9341_NAVY);
  tft.drawRect(0, 0, 240, 320, ILI9341_NAVY);
  tft.drawRect(1, 1, 238, 318, ILI9341_NAVY);
  tft.setCursor(50, 2);
  tft.setTextColor(ILI9341_LIGHTGREY);
  tft.setTextSize(3);
  tft.print("Throttle");
  tft.setCursor(22, 26);
  tft.print("Calibration");
}

void drawSteerCalib() {
  tft.fillScreen(ILI9341_BLACK);
  tft.fillRect(0, 0, 240, 51, ILI9341_DARKCYAN);
  tft.drawRect(0, 0, 240, 320, ILI9341_DARKCYAN);
  tft.drawRect(1, 1, 238, 318, ILI9341_DARKCYAN);
  tft.setCursor(50, 2);
  tft.setTextColor(ILI9341_LIGHTGREY);
  tft.setTextSize(3);
  tft.print("Steering");
  tft.setCursor(22, 26);
  tft.print("Calibration");
}

void clearCalib() {
  tft.fillRect(2, 51, 236, 267, ILI9341_BLACK);
}
