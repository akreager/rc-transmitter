// ============================================================
// drawScreens.ino
// Screen drawing, button registration, and per-frame updates.
//
// Each screen has up to three functions:
//   draw*Screen()   - draws static UI (background, labels, borders)
//   setup*Buttons() - registers buttons with the screen manager
//   update*Screen() - redraws dynamic data (telemetry, state)
//
// To add a new screen, follow this pattern and wire it into
// changeScreen() and screenUpdate() in screenManager.ino.
// ============================================================


// ************************************************************
// HOME / STATUS SCREEN
// ************************************************************

void drawHomeScreen() {
  tft.fillScreen(ILI9341_BLACK);

  // --- Header bar ---
  tft.fillRect(0, 0, SCREEN_W, 28, ILI9341_LIGHTGREY);

  // --- Border (double line) ---
  tft.drawRect(0, 0, SCREEN_W, SCREEN_H, ILI9341_LIGHTGREY);
  tft.drawRect(1, 1, SCREEN_W - 2, SCREEN_H - 2, ILI9341_LIGHTGREY);

  // --- Title ---
  tft.setTextColor(ILI9341_BLACK);
  tft.setTextSize(3);
  tft.setCursor(69, 2);
  tft.print(F("Status"));

  // --- Telemetry labels ---
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

  // --- M-Stop button (initial draw) ---
  drawMStopButton();
}

void setupHomeButtons() {
  addButton(MSTOP_X, MSTOP_Y, MSTOP_W, MSTOP_H, onMStopPress);
}

// --- M-Stop Button Drawing ---

void drawMStopButton() {
  // Drop shadow
  tft.fillRect(MSTOP_X + MSTOP_SHADOW, MSTOP_Y + MSTOP_SHADOW,
               MSTOP_W, MSTOP_H, SHADOW);

  if (motorStopActive) {
    // Motors are STOPPED → show green button to enable them
    tft.fillRect(MSTOP_X, MSTOP_Y, MSTOP_W, MSTOP_H, COLOR_GO_GREEN);
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(3);
    tft.setCursor(MSTOP_X + 42, MSTOP_Y + 12);
    tft.print(F("ENABLE"));
  } else {
    // Motors are RUNNING → show red button to stop them
    tft.fillRect(MSTOP_X, MSTOP_Y, MSTOP_W, MSTOP_H, COLOR_STOP_RED);
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(3);
    tft.setCursor(MSTOP_X + 30, MSTOP_Y + 12);
    tft.print(F("M-STOP"));
  }
}

void onMStopPress() {
  motorStopActive = !motorStopActive;
  drawMStopButton();   // Redraw button with new state
}

// --- Home Screen Telemetry Updates ---

void updateHomeScreen() {
  int mainBattery = rawBatt / 10;
  int temp1 = rawTemp1 / 10;
  int temp2 = rawTemp2 / 10;
  int currentM1 = rawCurrentMotor1 / 100;
  int currentM2 = rawCurrentMotor2 / 100;

  tft.setTextSize(3);
  tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);

  // Battery voltage (tenths of a volt)
  tft.setCursor(112, 30);
  tft.print(mainBattery);
  tft.print(".");
  tft.print(rawBatt - (mainBattery * 10));
  tft.print("V ");

  // Temperature 1 (tenths of a degree C)
  tft.setCursor(112, 54);
  tft.print(temp1);
  tft.print(".");
  tft.print(rawTemp1 - (temp1 * 10));
  tft.print((char)247);
  tft.print("C ");

  // Temperature 2
  tft.setCursor(112, 78);
  tft.print(temp2);
  tft.print(".");
  tft.print(rawTemp2 - (temp2 * 10));
  tft.print((char)247);
  tft.print("C ");

  // Motor 1 current (hundredths of an amp)
  tft.setCursor(80, 126);
  tft.print(currentM1);
  tft.print(".");
  tft.print(rawCurrentMotor1 - (currentM1 * 100));
  tft.print("A ");

  // Motor 2 current
  tft.setCursor(80, 174);
  tft.print(currentM2);
  tft.print(".");
  tft.print(rawCurrentMotor2 - (currentM2 * 100));
  tft.print("A ");
}


// ************************************************************
// THROTTLE CALIBRATION SCREEN
// ************************************************************

void drawThrottleCalibScreen() {
  tft.fillScreen(ILI9341_BLACK);

  // Header
  tft.fillRect(0, 0, SCREEN_W, 51, ILI9341_NAVY);
  tft.drawRect(0, 0, SCREEN_W, SCREEN_H, ILI9341_NAVY);
  tft.drawRect(1, 1, SCREEN_W - 2, SCREEN_H - 2, ILI9341_NAVY);
  tft.setTextColor(ILI9341_LIGHTGREY);
  tft.setTextSize(3);
  tft.setCursor(50, 2);
  tft.print(F("Throttle"));
  tft.setCursor(22, 26);
  tft.print(F("Calibration"));

  // TODO: draw calibration UI elements
  // (live bar graph, instructions, next/back buttons)
}

void setupThrottleCalibButtons() {
  // TODO: register back/next/start buttons
  // Example:
  // addButton(12, 263, 100, 45, onThrottleCalibBack);
  // addButton(128, 263, 100, 45, onThrottleCalibNext);
}


// ************************************************************
// STEERING CALIBRATION SCREEN
// ************************************************************

void drawSteerCalibScreen() {
  tft.fillScreen(ILI9341_BLACK);

  // Header
  tft.fillRect(0, 0, SCREEN_W, 51, ILI9341_DARKCYAN);
  tft.drawRect(0, 0, SCREEN_W, SCREEN_H, ILI9341_DARKCYAN);
  tft.drawRect(1, 1, SCREEN_W - 2, SCREEN_H - 2, ILI9341_DARKCYAN);
  tft.setTextColor(ILI9341_LIGHTGREY);
  tft.setTextSize(3);
  tft.setCursor(50, 2);
  tft.print(F("Steering"));
  tft.setCursor(22, 26);
  tft.print(F("Calibration"));

  // TODO: draw calibration UI elements
}

void setupSteerCalibButtons() {
  // TODO: register back/next/start buttons
}


// ************************************************************
// UTILITY / DEBUG
// ************************************************************

// Draw crosshairs for screen alignment debugging
void drawHelper() {
  tft.drawRect(SCREEN_W / 2, 0, 2, SCREEN_H, ILI9341_YELLOW);
  tft.drawRect(0, SCREEN_H / 2, SCREEN_W, 2, ILI9341_YELLOW);
}

// Clear content area below a calibration header
void clearCalibContent() {
  tft.fillRect(2, 52, SCREEN_W - 4, SCREEN_H - 54, ILI9341_BLACK);
}
