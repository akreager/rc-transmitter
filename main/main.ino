/*
  RC Transmitter - Teensy 4.1
  
  Wireless RC transmitter with touchscreen UI.
  Sends joystick data via nRF24L01 and displays telemetry
  received back from the rover via acknowledge payloads.

  Hardware:
    - Teensy 4.1
    - Adafruit 2.8" TFT Touch Breakout (https://www.adafruit.com/product/1770)
    - nRF24L01+ radio (SPI0)
    - Analog joystick (steering + throttle)
    - RoboClaw motor controller on rover side

  Libraries:
    - RF24 (TMRh20 fork)
    - Adafruit GFX + ILI9341
    - Teensy (bundled) TouchScreen

  Developed with assistance from Claude (Anthropic).
*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <TouchScreen.h>
#include "config.h"

// ============================================================
// HARDWARE OBJECTS
// ============================================================
RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = "00001";

Adafruit_ILI9341 tft = Adafruit_ILI9341(&SPI1, TFT_DC, TFT_CS);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, TS_RESISTANCE);

// ============================================================
// RADIO DATA STRUCTURES
// These must match the receiver's struct definitions exactly!
// ============================================================
struct dataPackage {
  byte throttle;
  byte steering;
  bool data_motor_stop;
};
dataPackage data;

struct ackPackage {
  byte highBatt;
  byte lowBatt;
  byte highTemp1;
  byte lowTemp1;
  byte highTemp2;
  byte lowTemp2;
  byte highIM1;
  byte lowIM1;
  byte highIM2;
  byte lowIM2;
  bool ack_motor_stop;
};
ackPackage ack;

// ============================================================
// TELEMETRY STATE
// ============================================================
unsigned int rawBatt = 0;
unsigned int rawTemp1 = 0;
unsigned int rawTemp2 = 0;
int rawCurrentMotor1 = 0;
int rawCurrentMotor2 = 0;

// ============================================================
// UI STATE
// ============================================================
bool motorStopActive = true;          // M-stop is ON by default (motors disabled)

// Touch state (managed by touchInput.ino)
TouchState touchState = TOUCH_IDLE;
int16_t touchX = 0;
int16_t touchY = 0;
unsigned long lastTouchTime = 0;

// Screen & button state (managed by screenManager.ino)
ScreenID currentScreen = SCREEN_HOME;
Button buttons[MAX_BUTTONS];
int buttonCount = 0;

// Telemetry display throttle
unsigned long lastDisplayUpdate = 0;

// ============================================================
// SETUP
// ============================================================
void setup() {
  Serial.begin(115200);   // Debug output (optional, remove for production)

  // Initialize display
  if (TFT_LITE >= 0) {
    pinMode(TFT_LITE, OUTPUT);
    analogWrite(TFT_LITE, 128);  // 50% brightness, adjust to taste
  }
  tft.begin();
  tft.setRotation(TFT_ROTATION);

  // Initialize radio
  radio.begin();
  radio.openWritingPipe(address);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MIN);
  radio.enableAckPayload();
  radio.setRetries(3, 5);

  // Default data values (64 = center/stopped for RoboClaw mixed mode)
  data.throttle = 64;
  data.steering = 64;
  data.data_motor_stop = true;

  // Draw initial screen
  changeScreen(SCREEN_HOME);
}

// ============================================================
// MAIN LOOP
// ============================================================
void loop() {
  // 1. Process touch input
  touchUpdate();

  // 2. Read joystick and set motor stop flag
  data.throttle = map(analogRead(JOY_THROTTLE), 0, 1023, 127, 0);
  data.steering = map(analogRead(JOY_STEERING), 0, 1023, 0, 127);
  data.data_motor_stop = motorStopActive;

  // 3. Transmit and receive acknowledge
  radio.write(&data, sizeof(dataPackage));
  if (radio.isAckPayloadAvailable()) {
    radio.read(&ack, sizeof(ackPackage));

    // Reconstitute 16-bit values from high/low bytes
    rawBatt = (ack.highBatt << 8) + ack.lowBatt;
    rawTemp1 = (ack.highTemp1 << 8) + ack.lowTemp1;
    rawTemp2 = (ack.highTemp2 << 8) + ack.lowTemp2;
    rawCurrentMotor1 = (ack.highIM1 << 8) + ack.lowIM1;
    rawCurrentMotor2 = (ack.highIM2 << 8) + ack.lowIM2;
  }

  // 4. Update current screen (throttled to avoid flicker)
  unsigned long now = millis();
  if (now - lastDisplayUpdate >= TELEMETRY_UPDATE_MS) {
    lastDisplayUpdate = now;
    screenUpdate();
  }
}
