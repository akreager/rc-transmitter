#ifndef CONFIG_H
#define CONFIG_H

// ============================================================
// HARDWARE PIN ASSIGNMENTS
// !! Verify these match your physical wiring !!
//
// These defaults are from the pin assignment table (Sheet4).
// If your wiring matches the old test sketches instead,
// see the commented alternatives below each section.
// ============================================================

// --- nRF24L01 (default SPI bus) ---
#define CE_PIN    9
#define CSN_PIN   10

// --- ILI9341 Display (SPI1 bus) ---
// SPI1 pins: CLK=27, MISO=1, MOSI=26 (active when using &SPI1)
#define TFT_CS    0     // D0
#define TFT_DC    2     // D2
#define TFT_LITE  3     // D3, backlight

// --- Resistive Touchscreen ---
#define YP  A10   // D24, must be analog pin
#define XM  A11   // D25, must be analog pin
#define XP  28    // D28, digital
#define YM  29    // D29, digital

#define TS_RESISTANCE 300   // Ohms across X plate (measure with multimeter)

// --- Touch Calibration ---
// These are from the test sketches. Re-run calibration if you
// change orientation or if touch coordinates seem wrong.
#define TS_MINX       150
#define TS_MINY       120
#define TS_MAXX       920
#define TS_MAXY       940
#define MINPRESSURE   10
#define MAXPRESSURE   1000

// --- Joystick ---
#define JOY_STEERING  A0    // D14
#define JOY_THROTTLE  A1    // D15

// ============================================================
// DISPLAY CONSTANTS
// ============================================================
#define SCREEN_W      240
#define SCREEN_H      320
#define TFT_ROTATION  2     // USB connector at top

// ============================================================
// CUSTOM COLORS (RGB565)
// ============================================================
#define SHADOW            0x2965
#define COLOR_STOP_RED    ILI9341_RED
#define COLOR_GO_GREEN    0x2DC6    // Slightly muted green

// ============================================================
// UI TIMING
// ============================================================
#define TOUCH_DEBOUNCE_MS     250   // Min ms between accepted presses
#define TELEMETRY_UPDATE_MS   100   // How often to refresh display values

// ============================================================
// SHARED TYPES
// These are defined here so all .ino files can see them
// (Arduino IDE concatenates .ino files alphabetically after main)
// ============================================================

// --- Touch State Machine ---
enum TouchState {
  TOUCH_IDLE,           // No touch detected
  TOUCH_PRESSED,        // Touch just detected this frame (fires once)
  TOUCH_HELD,           // Touch continuing from previous frame
  TOUCH_WAIT_RELEASE    // Waiting for finger lift before accepting new input
};

// --- Screen IDs ---
enum ScreenID {
  SCREEN_HOME,
  SCREEN_CALIBRATE_THROTTLE,
  SCREEN_CALIBRATE_STEERING,
  SCREEN_COUNT
};

// --- Button Definition ---
// Each screen registers its own set of buttons.
// onPress is called once per tap (ghost-press protected).
typedef void (*ButtonCallback)();
struct Button {
  int16_t x, y, w, h;
  ButtonCallback onPress;
  bool enabled;
};

#define MAX_BUTTONS 6

// ============================================================
// M-STOP BUTTON LAYOUT
// ============================================================
#define MSTOP_X       12
#define MSTOP_Y       220
#define MSTOP_W       216
#define MSTOP_H       45
#define MSTOP_SHADOW  4

#endif // CONFIG_H
