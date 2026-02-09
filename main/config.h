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

#define TS_RESISTANCE 314   // Ohms across X plate (measured with multimeter)

// --- Touch Calibration Defaults ---
// These are starting values. Runtime variables (tsMinX, etc.)
// can be updated by the touch calibration routine.
#define TS_DEFAULT_MINX   150
#define TS_DEFAULT_MINY   120
#define TS_DEFAULT_MAXX   920
#define TS_DEFAULT_MAXY   940
#define MINPRESSURE       10
#define MAXPRESSURE       1000

// --- Joystick ---
#define JOY_STEERING  A0    // D14
#define JOY_THROTTLE  A1    // D15

// --- Analog Input Calibration ---
// Generic calibration data per analog pin.
// Vehicle config maps pins to functions (e.g., A0 → steering).
#define NUM_ANALOG_INPUTS   2
#define ACAL_SAMPLES        10    // Number of reads to average per sample
#define ACAL_SAMPLE_DELAY   5     // ms between reads
#define ACAL_DEFAULT_DEADZONE 5   // Raw ADC units around center

struct AnalogCalData {
  uint8_t pin;            // Analog pin number
  long calCenter;         // Calibrated center (raw ADC average)
  long calMin;            // Calibrated min endpoint (raw ADC average)
  long calMax;            // Calibrated max endpoint (raw ADC average)
  byte deadZone;          // Dead zone around center (raw ADC units)
  bool calibrated;        // Has this input been through calibration?
};

enum AnalogCalStep {
  ACAL_IDLE,              // Not calibrating
  ACAL_CENTER,            // Waiting for user to position center
  ACAL_MIN,               // Waiting for user to position minimum
  ACAL_MAX,               // Waiting for user to position maximum
  ACAL_DONE               // Calibration complete, showing results
};

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

// --- Touch Calibration State ---
enum TouchCalibState {
  TCAL_IDLE,            // Not calibrating
  TCAL_WAIT_TARGET,     // Waiting for user to tap current target
  TCAL_DONE             // All targets collected, showing results
};

#define TCAL_NUM_TARGETS  3
// Target positions in screen pixels (where crosshairs are drawn)
// Spread across the screen for best calibration accuracy
#define TCAL_TARGET_0_X   20    // Near top-left (below header)
#define TCAL_TARGET_0_Y   60
#define TCAL_TARGET_1_X   220   // Near bottom-right (above back button)
#define TCAL_TARGET_1_Y   240
#define TCAL_TARGET_2_X   120   // Center (verification point)
#define TCAL_TARGET_2_Y   160

// --- Screen IDs ---
enum ScreenID {
  SCREEN_HOME,
  SCREEN_SETUP,
  SCREEN_CALIBRATE_TOUCH,
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
// HOME SCREEN BUTTON LAYOUT
// ============================================================
#define BTN_SHADOW    4

// M-Stop button
#define MSTOP_X       12
#define MSTOP_Y       200
#define MSTOP_W       216
#define MSTOP_H       45

// Setup button
#define SETUP_X       12
#define SETUP_Y       258
#define SETUP_W       216
#define SETUP_H       45

// Generic menu button (used on setup screen, etc.)
#define MENU_BTN_X    12
#define MENU_BTN_W    216
#define MENU_BTN_H    40

#endif // CONFIG_H
