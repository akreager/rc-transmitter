// ============================================================
// analogInput.ino
// Generic analog input calibration and reading.
//
// Each analog input has calibration data (center/min/max/deadzone).
// The calibration routine is pin-generic — the same state machine
// works for any analog input. Vehicle configurations map pins
// to functions (e.g., analogInputs[0] → steering).
//
// Based on calibration approach by Joram_:
// https://www.instructables.com/Easy-Analog-Joystick-Calibration-and-Centering/
// ============================================================

// Take multiple samples from an analog pin and return the average.
long sampleAnalogInput(int idx) {
  long total = 0;
  for (int i = 0; i < ACAL_SAMPLES; i++) {
    total += analogRead(analogInputs[idx].pin);
    delay(ACAL_SAMPLE_DELAY);
  }
  return total / ACAL_SAMPLES;
}

// Read an analog input and apply calibration + deadzone.
// Returns 0-127 with 64 as center (RoboClaw mixed mode).
byte readCalibratedInput(int idx) {
  AnalogCalData &inp = analogInputs[idx];

  if (!inp.calibrated) {
    // No calibration — simple linear map, default direction
    return (byte)map(analogRead(inp.pin), 0, 1023, 0, 127);
  }

  long raw = analogRead(inp.pin);

  // Apply deadzone around center
  if (abs(raw - inp.calCenter) <= inp.deadZone) {
    return 64;
  }

  // Handle both orientations (min can be above or below center)
  byte result;
  if (inp.calMin < inp.calCenter) {
    // Normal orientation: min < center < max
    if (raw < inp.calCenter) {
      result = (byte)map(raw, inp.calMin, inp.calCenter, 0, 64);
    } else {
      result = (byte)map(raw, inp.calCenter, inp.calMax, 64, 127);
    }
  } else {
    // Inverted orientation: min > center > max
    if (raw > inp.calCenter) {
      result = (byte)map(raw, inp.calMin, inp.calCenter, 0, 64);
    } else {
      result = (byte)map(raw, inp.calCenter, inp.calMax, 64, 127);
    }
  }

  return constrain(result, 0, 127);
}

// Start calibration for a given analog input index.
void startAnalogCal(int idx) {
  aCalInputIdx = idx;
  aCalStep = ACAL_CENTER;
}

// Sample the current calibration step and advance to next.
// Called when the user taps the "Sample" button on the cal screen.
void sampleCalibrationStep() {
  if (aCalInputIdx < 0 || aCalInputIdx >= NUM_ANALOG_INPUTS) return;

  AnalogCalData &inp = analogInputs[aCalInputIdx];

  switch (aCalStep) {
    case ACAL_CENTER:
      inp.calCenter = sampleAnalogInput(aCalInputIdx);
      aCalStep = ACAL_MIN;
      break;

    case ACAL_MIN:
      inp.calMin = sampleAnalogInput(aCalInputIdx);
      aCalStep = ACAL_MAX;
      break;

    case ACAL_MAX:
      inp.calMax = sampleAnalogInput(aCalInputIdx);
      aCalStep = ACAL_DONE;
      break;

    default:
      break;
  }
}

// Apply the active calibration — marks the input as calibrated.
void applyAnalogCal() {
  if (aCalInputIdx < 0 || aCalInputIdx >= NUM_ANALOG_INPUTS) return;

  AnalogCalData &inp = analogInputs[aCalInputIdx];
  inp.calibrated = true;

  // Print to serial so user can record values
  Serial.println(F("--- Analog Calibration Applied ---"));
  Serial.print(F("  Input index: ")); Serial.println(aCalInputIdx);
  Serial.print(F("  Pin: A"));        Serial.println(inp.pin - A0);
  Serial.print(F("  Center: "));      Serial.println(inp.calCenter);
  Serial.print(F("  Min: "));         Serial.println(inp.calMin);
  Serial.print(F("  Max: "));         Serial.println(inp.calMax);
  Serial.print(F("  DeadZone: "));    Serial.println(inp.deadZone);

  aCalStep = ACAL_IDLE;
  aCalInputIdx = -1;
}

// Cancel calibration without applying.
void cancelAnalogCal() {
  aCalStep = ACAL_IDLE;
  aCalInputIdx = -1;
}
