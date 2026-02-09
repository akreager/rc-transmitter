// ============================================================
// screenManager.ino
// Manages screen state, button registration, and transitions.
//
// Each screen has three parts:
//   draw*Screen()   - draws static elements (called once on entry)
//   setup*Buttons() - registers touchable buttons for that screen
//   update*Screen() - refreshes dynamic content (called each frame)
//
// To add a new screen:
//   1. Add an entry to ScreenID in config.h
//   2. Create draw/setup/update functions in drawScreens.ino
//   3. Add cases to changeScreen() and screenUpdate() below
// ============================================================

// --- Button Management ---

void clearButtons() {
  buttonCount = 0;
  for (int i = 0; i < MAX_BUTTONS; i++) {
    buttons[i].enabled = false;
  }
}

// Register a button. Returns the button index, or -1 if full.
int addButton(int16_t x, int16_t y, int16_t w, int16_t h, ButtonCallback onPress) {
  if (buttonCount >= MAX_BUTTONS) return -1;
  int idx = buttonCount;
  buttons[idx].x = x;
  buttons[idx].y = y;
  buttons[idx].w = w;
  buttons[idx].h = h;
  buttons[idx].onPress = onPress;
  buttons[idx].enabled = true;
  buttonCount++;
  return idx;
}

void disableButton(int idx) {
  if (idx >= 0 && idx < MAX_BUTTONS) {
    buttons[idx].enabled = false;
  }
}

void enableButton(int idx) {
  if (idx >= 0 && idx < MAX_BUTTONS) {
    buttons[idx].enabled = true;
  }
}

// --- Touch Hit Testing ---
// Called from touchUpdate() on the single frame a press is detected.

void processTouch(int16_t x, int16_t y) {
  for (int i = 0; i < buttonCount; i++) {
    if (!buttons[i].enabled) continue;

    bool hit = (x >= buttons[i].x) &&
               (x <  buttons[i].x + buttons[i].w) &&
               (y >= buttons[i].y) &&
               (y <  buttons[i].y + buttons[i].h);

    if (hit) {
      // Fire the callback
      buttons[i].onPress();

      // Block further input until finger lifts.
      // This is what prevents the ghost-press problem:
      // even if the callback changes screens and a new button
      // occupies this same region, no press will register until
      // the user physically lifts their finger.
      requireRelease();
      return;   // One button per touch event
    }
  }
}

// --- Screen Transitions ---

void changeScreen(ScreenID screen) {
  // 1. Clear all buttons from previous screen
  clearButtons();

  // 2. Update current screen tracker
  currentScreen = screen;

  // 3. Draw the new screen and register its buttons
  switch (screen) {
    case SCREEN_HOME:
      drawHomeScreen();
      setupHomeButtons();
      break;

    case SCREEN_SETUP:
      drawSetupScreen();
      setupSetupButtons();
      break;

    case SCREEN_CALIBRATE_TOUCH:
      drawTouchCalibScreen();
      setupTouchCalibButtons();
      break;

    case SCREEN_CALIBRATE_THROTTLE:
      drawThrottleCalibScreen();
      setupThrottleCalibButtons();
      break;

    case SCREEN_CALIBRATE_STEERING:
      drawSteerCalibScreen();
      setupSteerCalibButtons();
      break;

    default:
      break;
  }

  // 4. Always require release after a screen change.
  // Even if the touch state machine already set WAIT_RELEASE
  // from the button press, this covers cases where changeScreen()
  // is called from code (e.g., a timeout or failsafe).
  requireRelease();

  // 5. Clear any pending tap flag so the previous screen's tap
  // doesn't get consumed by the new screen's update function.
  tapDetected = false;
}

// --- Per-Frame Screen Updates ---
// Called from loop() at the telemetry refresh rate.

void screenUpdate() {
  switch (currentScreen) {
    case SCREEN_HOME:
      updateHomeScreen();
      break;

    case SCREEN_SETUP:
      // Static menu, no per-frame updates needed
      break;

    case SCREEN_CALIBRATE_TOUCH:
      updateTouchCalibScreen();
      break;

    case SCREEN_CALIBRATE_THROTTLE:
      updateAnalogCalScreen();
      break;

    case SCREEN_CALIBRATE_STEERING:
      updateAnalogCalScreen();
      break;

    default:
      break;
  }
}
