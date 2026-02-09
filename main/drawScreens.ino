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
// SHARED UI HELPERS
// ************************************************************

// Draw a standard button with drop shadow
void drawButton(int16_t x, int16_t y, int16_t w, int16_t h,
                uint16_t color, const __FlashStringHelper *label) {
  // Drop shadow
  tft.fillRect(x + BTN_SHADOW, y + BTN_SHADOW, w, h, SHADOW);
  // Button face
  tft.fillRect(x, y, w, h, color);
  // Label — centered horizontally (approximate)
  int16_t textW = strlen_P((const char *)label) * 18;  // size 3 = ~18px/char
  int16_t textX = x + (w - textW) / 2;
  int16_t textY = y + (h - 21) / 2;  // size 3 = ~21px tall
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(3);
  tft.setCursor(textX, textY);
  tft.print(label);
}

// Draw a menu-style button at a given row position
void drawMenuButton(int16_t y, uint16_t color, const __FlashStringHelper *label) {
  drawButton(MENU_BTN_X, y, MENU_BTN_W, MENU_BTN_H, color, label);
}

// Draw a crosshair at a given screen position
void drawCrosshair(int16_t x, int16_t y, uint16_t color) {
  tft.drawLine(x - 10, y, x + 10, y, color);
  tft.drawLine(x, y - 10, x, y + 10, color);
  tft.drawCircle(x, y, 6, color);
}

// Erase a crosshair by drawing over it in black
void eraseCrosshair(int16_t x, int16_t y) {
  drawCrosshair(x, y, ILI9341_BLACK);
}

// Clear the content area below a standard 28px header, above back button
void clearContentArea() {
  tft.fillRect(2, 30, SCREEN_W - 4, 232, ILI9341_BLACK);
}

// Clear the content area below a 51px calibration header, above back button
void clearCalibContent() {
  tft.fillRect(2, 52, SCREEN_W - 4, 210, ILI9341_BLACK);
}


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

  // --- Buttons ---
  drawMStopButton();
  drawButton(SETUP_X, SETUP_Y, SETUP_W, SETUP_H,
             ILI9341_DARKGREY, F("Setup"));
}

void setupHomeButtons() {
  addButton(MSTOP_X, MSTOP_Y, MSTOP_W, MSTOP_H, onMStopPress);
  addButton(SETUP_X, SETUP_Y, SETUP_W, SETUP_H, onSetupPress);
}

// --- M-Stop Button ---

void drawMStopButton() {
  if (motorStopActive) {
    drawButton(MSTOP_X, MSTOP_Y, MSTOP_W, MSTOP_H,
               COLOR_GO_GREEN, F("ENABLE"));
  } else {
    drawButton(MSTOP_X, MSTOP_Y, MSTOP_W, MSTOP_H,
               COLOR_STOP_RED, F("M-STOP"));
  }
}

void onMStopPress() {
  motorStopActive = !motorStopActive;
  drawMStopButton();
}

void onSetupPress() {
  changeScreen(SCREEN_SETUP);
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
// SETUP MENU SCREEN
// ************************************************************

#define SETUP_MENU_START_Y  50
#define SETUP_MENU_SPACING  54

void drawSetupScreen() {
  tft.fillScreen(ILI9341_BLACK);

  // --- Header bar ---
  tft.fillRect(0, 0, SCREEN_W, 28, ILI9341_DARKGREY);
  tft.drawRect(0, 0, SCREEN_W, SCREEN_H, ILI9341_DARKGREY);
  tft.drawRect(1, 1, SCREEN_W - 2, SCREEN_H - 2, ILI9341_DARKGREY);

  // --- Title ---
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(3);
  tft.setCursor(75, 2);
  tft.print(F("Setup"));

  // --- Menu buttons ---
  drawMenuButton(SETUP_MENU_START_Y + SETUP_MENU_SPACING * 0,
                 ILI9341_PURPLE, F("Touch Cal"));

  drawMenuButton(SETUP_MENU_START_Y + SETUP_MENU_SPACING * 1,
                 ILI9341_NAVY, F("A0 Cal"));

  drawMenuButton(SETUP_MENU_START_Y + SETUP_MENU_SPACING * 2,
                 ILI9341_DARKCYAN, F("A1 Cal"));

  // --- Back button at bottom ---
  drawMenuButton(268, ILI9341_DARKGREY, F("Back"));
}

void setupSetupButtons() {
  addButton(MENU_BTN_X, SETUP_MENU_START_Y + SETUP_MENU_SPACING * 0,
            MENU_BTN_W, MENU_BTN_H, onSetupTouchCal);
  addButton(MENU_BTN_X, SETUP_MENU_START_Y + SETUP_MENU_SPACING * 1,
            MENU_BTN_W, MENU_BTN_H, onSetupA0Cal);
  addButton(MENU_BTN_X, SETUP_MENU_START_Y + SETUP_MENU_SPACING * 2,
            MENU_BTN_W, MENU_BTN_H, onSetupA1Cal);
  addButton(MENU_BTN_X, 268, MENU_BTN_W, MENU_BTN_H, onSetupBack);
}

void onSetupTouchCal() {
  changeScreen(SCREEN_CALIBRATE_TOUCH);
}

void onSetupA0Cal() {
  changeScreen(SCREEN_CALIBRATE_THROTTLE);
}

void onSetupA1Cal() {
  changeScreen(SCREEN_CALIBRATE_STEERING);
}

void onSetupBack() {
  changeScreen(SCREEN_HOME);
}


// ************************************************************
// TOUCH CALIBRATION SCREEN
// ************************************************************

// Target screen coordinates (where crosshairs are drawn)
const int16_t tCalTargetX[TCAL_NUM_TARGETS] = {
  TCAL_TARGET_0_X, TCAL_TARGET_1_X, TCAL_TARGET_2_X
};
const int16_t tCalTargetY[TCAL_NUM_TARGETS] = {
  TCAL_TARGET_0_Y, TCAL_TARGET_1_Y, TCAL_TARGET_2_Y
};

void drawTouchCalibScreen() {
  tft.fillScreen(ILI9341_BLACK);

  // Header
  tft.fillRect(0, 0, SCREEN_W, 28, ILI9341_PURPLE);
  tft.drawRect(0, 0, SCREEN_W, SCREEN_H, ILI9341_PURPLE);
  tft.drawRect(1, 1, SCREEN_W - 2, SCREEN_H - 2, ILI9341_PURPLE);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(3);
  tft.setCursor(39, 2);
  tft.print(F("Touch Cal"));

  // Start calibration sequence
  tCalState = TCAL_WAIT_TARGET;
  tCalTarget = 0;

  // Draw first crosshair and instruction
  drawCalibInstruction();
  drawCrosshair(tCalTargetX[0], tCalTargetY[0], ILI9341_YELLOW);

  // Back button
  drawMenuButton(268, ILI9341_DARKGREY, F("Back"));
}

void setupTouchCalibButtons() {
  addButton(MENU_BTN_X, 268, MENU_BTN_W, MENU_BTN_H, onTouchCalibBack);
  // Note: crosshair taps are NOT buttons. They're detected
  // in updateTouchCalibScreen() via the tapDetected flag.
}

void onTouchCalibBack() {
  tCalState = TCAL_IDLE;
  changeScreen(SCREEN_SETUP);
}

void drawCalibInstruction() {
  // Clear instruction area
  tft.fillRect(2, 30, SCREEN_W - 4, 16, ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(12, 32);
  tft.print(F("Tap target "));
  tft.print(tCalTarget + 1);
  tft.print(F(" of "));
  tft.print(TCAL_NUM_TARGETS);
}

void updateTouchCalibScreen() {
  if (tCalState == TCAL_WAIT_TARGET && tapDetected) {
    tapDetected = false;    // Consume the tap

    // Record raw values for this target
    tCalRawX[tCalTarget] = rawTouchX;
    tCalRawY[tCalTarget] = rawTouchY;

    // Show what we recorded
    tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
    tft.setTextSize(1);
    tft.setCursor(12, 56 + tCalTarget * 12);
    tft.print(F("T"));
    tft.print(tCalTarget + 1);
    tft.print(F(": raw("));
    tft.print(tCalRawX[tCalTarget]);
    tft.print(F(","));
    tft.print(tCalRawY[tCalTarget]);
    tft.print(F(")  "));

    // Erase current crosshair
    eraseCrosshair(tCalTargetX[tCalTarget], tCalTargetY[tCalTarget]);

    // Advance to next target
    tCalTarget++;

    if (tCalTarget < TCAL_NUM_TARGETS) {
      // Draw next crosshair
      drawCalibInstruction();
      drawCrosshair(tCalTargetX[tCalTarget], tCalTargetY[tCalTarget],
                    ILI9341_YELLOW);
    } else {
      // All targets collected — compute and show results
      tCalState = TCAL_DONE;
      computeAndShowCalibration();
    }
  }
}

void computeAndShowCalibration() {
  // Use targets 0 (top-left area) and 1 (bottom-right area) to compute mapping.
  // Target 2 (center) is used for verification.
  //
  // For rotation 2, mapping is: screenX = map(rawX, minX, maxX, SCREEN_W, 0)
  // From two known (screen, raw) pairs we solve for minX and maxX.

  int sx0 = tCalTargetX[0], sy0 = tCalTargetY[0];
  int sx1 = tCalTargetX[1], sy1 = tCalTargetY[1];
  long rx0 = tCalRawX[0], ry0 = tCalRawY[0];
  long rx1 = tCalRawX[1], ry1 = tCalRawY[1];

  int newMaxX, newMinX, newMaxY, newMinY;

  if (sx0 != sx1) {
    newMaxX = (int)((sx0 * rx1 - sx1 * rx0) / (sx0 - sx1));
    long rangeX = (long)SCREEN_W * (newMaxX - rx0) / sx0;
    newMinX = newMaxX - (int)rangeX;
  } else {
    newMaxX = tsMaxX;
    newMinX = tsMinX;
  }

  if (sy0 != sy1) {
    newMaxY = (int)((sy0 * ry1 - sy1 * ry0) / (sy0 - sy1));
    long rangeY = (long)SCREEN_H * (newMaxY - ry0) / sy0;
    newMinY = newMaxY - (int)rangeY;
  } else {
    newMaxY = tsMaxY;
    newMinY = tsMinY;
  }

  // Verify with target 2 (center)
  int verifyX = map(tCalRawX[2], newMinX, newMaxX, SCREEN_W, 0);
  int verifyY = map(tCalRawY[2], newMinY, newMaxY, SCREEN_H, 0);
  int errorX = abs(verifyX - tCalTargetX[2]);
  int errorY = abs(verifyY - tCalTargetY[2]);

  // --- Display results ---
  clearContentArea();

  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(12, 35);
  tft.print(F("Calibration done!"));

  tft.setTextSize(2);
  tft.setTextColor(ILI9341_LIGHTGREY);

  tft.setCursor(12, 60);
  tft.print(F("Old X: "));
  tft.print(tsMinX);
  tft.print(F(" - "));
  tft.print(tsMaxX);

  tft.setCursor(12, 80);
  tft.print(F("New X: "));
  tft.setTextColor(ILI9341_GREEN);
  tft.print(newMinX);
  tft.print(F(" - "));
  tft.print(newMaxX);

  tft.setTextColor(ILI9341_LIGHTGREY);
  tft.setCursor(12, 105);
  tft.print(F("Old Y: "));
  tft.print(tsMinY);
  tft.print(F(" - "));
  tft.print(tsMaxY);

  tft.setCursor(12, 125);
  tft.print(F("New Y: "));
  tft.setTextColor(ILI9341_GREEN);
  tft.print(newMinY);
  tft.print(F(" - "));
  tft.print(newMaxY);

  // Verification result
  tft.setCursor(12, 155);
  tft.setTextColor(ILI9341_WHITE);
  tft.print(F("Verify err: "));
  if (errorX < 15 && errorY < 15) {
    tft.setTextColor(ILI9341_GREEN);
  } else {
    tft.setTextColor(ILI9341_YELLOW);
  }
  tft.print(errorX);
  tft.print(F("px, "));
  tft.print(errorY);
  tft.print(F("px"));

  // Stash computed values for Apply
  tCalRawX[0] = newMinX;
  tCalRawX[1] = newMaxX;
  tCalRawY[0] = newMinY;
  tCalRawY[1] = newMaxY;

  // Draw Apply and Retry buttons
  drawButton(12, 190, 100, 40, COLOR_GO_GREEN, F("Apply"));
  drawButton(128, 190, 100, 40, ILI9341_ORANGE, F("Retry"));

  // Re-register buttons for this state
  clearButtons();
  addButton(12, 190, 100, 40, onTouchCalibApply);
  addButton(128, 190, 100, 40, onTouchCalibRetry);
  addButton(MENU_BTN_X, 268, MENU_BTN_W, MENU_BTN_H, onTouchCalibBack);
}

void onTouchCalibApply() {
  tsMinX = tCalRawX[0];
  tsMaxX = tCalRawX[1];
  tsMinY = tCalRawY[0];
  tsMaxY = tCalRawY[1];

  tCalState = TCAL_IDLE;

  Serial.println(F("--- Touch Calibration Applied ---"));
  Serial.print(F("  tsMinX = ")); Serial.println(tsMinX);
  Serial.print(F("  tsMaxX = ")); Serial.println(tsMaxX);
  Serial.print(F("  tsMinY = ")); Serial.println(tsMinY);
  Serial.print(F("  tsMaxY = ")); Serial.println(tsMaxY);
  Serial.println(F("Update TS_DEFAULT_* in config.h to make permanent."));

  changeScreen(SCREEN_SETUP);
}

void onTouchCalibRetry() {
  tCalState = TCAL_IDLE;
  changeScreen(SCREEN_CALIBRATE_TOUCH);
}


// ************************************************************
// ANALOG INPUT CALIBRATION SCREENS
// Shared logic for calibrating any analog input.
// SCREEN_CALIBRATE_THROTTLE = input 1 (A1), navy header
// SCREEN_CALIBRATE_STEERING = input 0 (A0), cyan header
// ************************************************************

// Get the header color for the current analog cal screen
uint16_t getAnalogCalColor() {
  return (currentScreen == SCREEN_CALIBRATE_THROTTLE)
         ? ILI9341_NAVY : ILI9341_DARKCYAN;
}

// Draw the shared analog calibration screen layout
void drawAnalogCalHeader(const __FlashStringHelper *line1,
                         const __FlashStringHelper *line2,
                         uint16_t color, int inputIdx) {
  tft.fillScreen(ILI9341_BLACK);

  // Header
  tft.fillRect(0, 0, SCREEN_W, 51, color);
  tft.drawRect(0, 0, SCREEN_W, SCREEN_H, color);
  tft.drawRect(1, 1, SCREEN_W - 2, SCREEN_H - 2, color);
  tft.setTextColor(ILI9341_LIGHTGREY);
  tft.setTextSize(3);
  tft.setCursor(50, 2);
  tft.print(line1);
  tft.setCursor(22, 26);
  tft.print(line2);

  // Start calibration
  startAnalogCal(inputIdx);

  // Draw the first step UI
  drawAnalogCalStep();

  // Back button
  drawMenuButton(268, ILI9341_DARKGREY, F("Back"));
}

// Draw the current calibration step instructions and buttons
void drawAnalogCalStep() {
  clearCalibContent();

  tft.setTextSize(2);
  tft.setTextColor(ILI9341_WHITE);

  switch (aCalStep) {
    case ACAL_CENTER:
      tft.setCursor(12, 60);
      tft.print(F("Step 1: CENTER"));
      tft.setCursor(12, 82);
      tft.print(F("Release input to"));
      tft.setCursor(12, 100);
      tft.print(F("center/rest position."));
      break;

    case ACAL_MIN:
      tft.setCursor(12, 60);
      tft.print(F("Step 2: MINIMUM"));
      tft.setCursor(12, 82);
      tft.print(F("Hold input at full"));
      tft.setCursor(12, 100);
      tft.print(F("minimum position."));
      break;

    case ACAL_MAX:
      tft.setCursor(12, 60);
      tft.print(F("Step 3: MAXIMUM"));
      tft.setCursor(12, 82);
      tft.print(F("Hold input at full"));
      tft.setCursor(12, 100);
      tft.print(F("maximum position."));
      break;

    default:
      break;
  }

  // Live value labels
  tft.setTextColor(ILI9341_LIGHTGREY);
  tft.setCursor(12, 130);
  tft.print(F("Raw:"));
  tft.setCursor(12, 155);
  tft.print(F("Mapped:"));

  // Sample button (full width for easier touch targeting)
  drawMenuButton(210, getAnalogCalColor(), F("Sample"));

  // Register buttons
  clearButtons();
  addButton(MENU_BTN_X, 210, MENU_BTN_W, MENU_BTN_H, onAnalogCalSample);
  addButton(MENU_BTN_X, 268, MENU_BTN_W, MENU_BTN_H, onAnalogCalBack);
}

// Draw the results after all 3 steps are done
void drawAnalogCalResults() {
  clearCalibContent();

  AnalogCalData &inp = analogInputs[aCalInputIdx];

  tft.setTextSize(2);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(12, 58);
  tft.print(F("Calibration done!"));

  tft.setTextColor(ILI9341_GREEN);
  tft.setCursor(12, 85);
  tft.print(F("Center: "));
  tft.print(inp.calCenter);

  tft.setCursor(12, 107);
  tft.print(F("Min:    "));
  tft.print(inp.calMin);

  tft.setCursor(12, 129);
  tft.print(F("Max:    "));
  tft.print(inp.calMax);

  // Show the mapped output range
  tft.setTextColor(ILI9341_LIGHTGREY);
  tft.setCursor(12, 158);
  tft.print(F("Output: 0-64-127"));

  // Apply and Retry buttons
  drawButton(12, 210, 100, 40, COLOR_GO_GREEN, F("Apply"));
  drawButton(128, 210, 100, 40, ILI9341_ORANGE, F("Retry"));

  clearButtons();
  addButton(12, 210, 100, 40, onAnalogCalApply);
  addButton(128, 210, 100, 40, onAnalogCalRetry);
  addButton(MENU_BTN_X, 268, MENU_BTN_W, MENU_BTN_H, onAnalogCalBack);
}

// Per-frame update: show live raw and mapped values
void updateAnalogCalScreen() {
  if (aCalStep == ACAL_IDLE || aCalStep == ACAL_DONE) return;
  if (aCalInputIdx < 0) return;

  int raw = analogRead(analogInputs[aCalInputIdx].pin);
  int mapped = readCalibratedInput(aCalInputIdx);

  tft.setTextSize(3);
  tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);

  tft.setCursor(90, 127);
  tft.print(raw);
  tft.print(F("    "));

  tft.setCursor(90, 152);
  tft.print(mapped);
  tft.print(F("    "));

  // Bar graph
  int barW = map(raw, 0, 1023, 0, 200);
  tft.fillRect(20, 190, barW, 12, ILI9341_GREEN);
  tft.fillRect(20 + barW, 190, 200 - barW, 12, ILI9341_DARKGREY);
  tft.drawLine(120, 187, 120, 205, ILI9341_WHITE);   // Center line
}

// --- Shared Analog Cal Callbacks ---

void onAnalogCalSample() {
  sampleCalibrationStep();

  if (aCalStep == ACAL_DONE) {
    drawAnalogCalResults();
  } else {
    drawAnalogCalStep();
  }
}

void onAnalogCalApply() {
  applyAnalogCal();
  changeScreen(SCREEN_SETUP);
}

void onAnalogCalRetry() {
  cancelAnalogCal();
  // Re-enter the same screen
  changeScreen(currentScreen);
}

void onAnalogCalBack() {
  cancelAnalogCal();
  changeScreen(SCREEN_SETUP);
}

// --- Throttle (A1) Calibration Screen ---

void drawThrottleCalibScreen() {
  drawAnalogCalHeader(F("A1 Input"), F("Calibration"),
                      ILI9341_NAVY, 1);
}

void setupThrottleCalibButtons() {
  // Buttons are registered by drawAnalogCalStep()
}

// --- Steering (A0) Calibration Screen ---

void drawSteerCalibScreen() {
  drawAnalogCalHeader(F("A0 Input"), F("Calibration"),
                      ILI9341_DARKCYAN, 0);
}

void setupSteerCalibButtons() {
  // Buttons are registered by drawAnalogCalStep()
}


// ************************************************************
// UTILITY / DEBUG
// ************************************************************

// Draw crosshairs for screen alignment debugging
void drawHelper() {
  tft.drawRect(SCREEN_W / 2, 0, 2, SCREEN_H, ILI9341_YELLOW);
  tft.drawRect(0, SCREEN_H / 2, SCREEN_W, 2, ILI9341_YELLOW);
}
