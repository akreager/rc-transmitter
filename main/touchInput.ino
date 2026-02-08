// ============================================================
// touchInput.ino
// Touch state machine for resistive touchscreen
//
// Solves the ghost-press problem: after a button fires or a
// screen changes, the state machine enters WAIT_RELEASE and
// ignores all input until the finger is physically lifted.
// A debounce timer adds additional protection against
// rapid repeated taps.
// ============================================================

void touchUpdate() {
  TSPoint p = ts.getPoint();

  // NOTE: If your touch pins are shared with other SPI/digital
  // functions, you may need to restore pin modes here:
  //   pinMode(XM, OUTPUT);
  //   pinMode(YP, OUTPUT);
  // With the pin table layout (A10/A11/D28/D29) this shouldn't
  // be necessary since they don't overlap with SPI1 pins.

  bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);

  switch (touchState) {

    case TOUCH_IDLE:
      if (pressed) {
        // Check debounce timer
        unsigned long now = millis();
        if (now - lastTouchTime < TOUCH_DEBOUNCE_MS) {
          // Too soon after last press, ignore
          break;
        }

        touchState = TOUCH_PRESSED;
        lastTouchTime = now;
        mapTouchCoords(p);

        // Fire button check on this single frame
        processTouch(touchX, touchY);
      }
      break;

    case TOUCH_PRESSED:
      // Transition: finger still down = held, finger up = idle
      if (pressed) {
        touchState = TOUCH_HELD;
      } else {
        touchState = TOUCH_IDLE;
      }
      break;

    case TOUCH_HELD:
      if (!pressed) {
        touchState = TOUCH_IDLE;
      }
      // Future: long-press detection could go here by tracking
      // how long we've been in HELD state.
      break;

    case TOUCH_WAIT_RELEASE:
      // Eat all input until finger is fully lifted.
      // This is the key to preventing ghost presses on screen
      // transitions -- no new press can register until the
      // physical touch ends.
      if (!pressed) {
        touchState = TOUCH_IDLE;
      }
      break;
  }
}

void mapTouchCoords(TSPoint &p) {
  // Map raw resistive values to pixel coordinates.
  // Rotation 2 = 180° from default, so axes are inverted.
  touchX = map(p.x, TS_MINX, TS_MAXX, SCREEN_W, 0);
  touchY = map(p.y, TS_MINY, TS_MAXY, SCREEN_H, 0);

  // Clamp to screen bounds
  touchX = constrain(touchX, 0, SCREEN_W - 1);
  touchY = constrain(touchY, 0, SCREEN_H - 1);
}

// Call this after any action that should block further input
// until the finger is lifted (button press, screen change, etc.)
void requireRelease() {
  touchState = TOUCH_WAIT_RELEASE;
}
