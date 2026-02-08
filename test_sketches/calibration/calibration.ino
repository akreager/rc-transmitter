/*
 * Based on instructable by Joram_
 * https://www.instructables.com/Easy-Analog-Joystick-Calibration-and-Centering/
 */
//Input pins
#define analog_input_wheel A0
#define analog_input_trigger A1

//Calibrated variables
byte range_steering_left = 0;      //full left turn
byte range_steering_right = 127;   //full right turn
byte range_steering_center = 64;   //steering stop
byte deadZone_wheel = 5;           //wheel deadzone, return center value for center +- deadzone(in steps of 1/1024)

byte range_throttle_reverse = 0;   //full throttle reverse
byte range_throttle_forward = 127; //full throttle forward
byte range_throttle_center = 64;   //throttle stop
byte deadZone_trigger = 5;         //trigger deadzone, return center value for center +- deadzone(in steps of 1/1024)

byte steering;
byte throttle;

//Calibration variables
unsigned int wheel_raw_value;
unsigned int trigger_raw_value;

unsigned long wheel_calibrated_center;
unsigned long wheel_calibrated_min;
unsigned long wheel_calibrated_max;
unsigned long trigger_calibrated_center;
unsigned long trigger_calibrated_min;
unsigned long trigger_calibrated_max;

int calibration_samples = 10;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(analog_input_wheel, INPUT);
  pinMode(analog_input_trigger, INPUT);

  calibrate_wheel();
  Serial.println();
  calibrate_trigger();
  Serial.println();
  Serial.println("---Calibration Complete---");
  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (wheel_calibrated_min < wheel_calibrated_center) {
    wheel_raw_value = analogRead(analog_input_wheel);
    if (wheel_raw_value < (wheel_calibrated_center - deadZone_steering)) {
      steering = map(wheel_raw_value, wheel_calibrated_min, wheel_calibrated_center, range_steering_left, range_steering_center);
    }
    else if (wheel_raw_value > (wheel_calibrated_center + deadZone_steering)) {
      steering = map(wheel_raw_value, wheel_calibrated_center, wheel_calibrated_max, range_steering_center, range_steering_right);
    }
    else {
      steering = range_steering_center;
    }
  }
  else if (wheel_calibrated_min > wheel_calibrated_center) {
    wheel_raw_value = analogRead(analog_input_wheel);
    if (wheel_raw_value > (wheel_calibrated_center + deadZone_steering)) {
      steering = map(wheel_raw_value, wheel_calibrated_min, wheel_calibrated_center, range_steering_left, range_steering_center);
    }
    else if (wheel_raw_value < (wheel_calibrated_center - deadZone_steering)) {
      steering = map(wheel_raw_value, wheel_calibrated_center, wheel_calibrated_max, range_steering_center, range_steering_right);
    }
    else {
      steering = range_steering_center;
    }
  }
  else {
    Serial.println("WHEEL CALIBRATION ERROR!");
    delay(500);
    Serial.println("Recalibration necessary...");
    delay(2000);
    calibrate_wheel();
  }

  if (trigger_calibrated_min < trigger_calibrated_center) {
    trigger_raw_value = analogRead(analog_input_trigger);
    if (trigger_raw_value < (trigger_calibrated_center - deadZone_throttle)) {
      throttle = map(trigger_raw_value, trigger_calibrated_min, trigger_calibrated_center, range_throttle_reverse, range_throttle_center);
    }
    else if (trigger_raw_value > (trigger_calibrated_center + deadZone_throttle)) {
      throttle = map(trigger_raw_value, trigger_calibrated_center, trigger_calibrated_max, range_throttle_center, range_throttle_forward);
    }
    else {
      throttle = range_throttle_center;
    }
  }
  else if (trigger_calibrated_min > trigger_calibrated_center) {
    trigger_raw_value = analogRead(analog_input_trigger);
    if (trigger_raw_value > (trigger_calibrated_center + deadZone_throttle)) {
      throttle = map(trigger_raw_value, trigger_calibrated_min, trigger_calibrated_center, range_throttle_reverse, range_throttle_center);
    }
    else if (trigger_raw_value < (trigger_calibrated_center - deadZone_throttle)) {
      throttle = map(trigger_raw_value, trigger_calibrated_center, trigger_calibrated_max, range_throttle_center, range_throttle_forward);
    }
    else {
      throttle = range_throttle_center;
    }
  }
  else {
    Serial.println("TRIGGER CALIBRATION ERROR!");
    delay(500);
    Serial.println("Recalibration necessary...");
    delay(2000);
    calibrate_trigger();
  }

  Serial.print(wheel_raw_value);
  Serial.print(", ");
  Serial.print(steering);
  Serial.print(": ");
  Serial.print(trigger_raw_value);
  Serial.print(", ");
  Serial.println(throttle);
}

void calibrate_wheel() {
  delay(1000);
  Serial.println("---Calibrating Wheel Input---");
  delay(1000);

  Serial.println();
  Serial.println("Release wheel to center position.");
  wheel_calibrated_center = 0;     //reset steering center
  delay(3000);

  Serial.println("Sampling wheel center:");
  delay(500);
  for (int i = 0; i < calibration_samples; i++) {    //take a number of readings
    Serial.print(".");
    wheel_calibrated_center += analogRead(analog_input_wheel);
    delay(5);
  }
  wheel_calibrated_center /= calibration_samples;    //average all readings
  Serial.println();
  Serial.print("Wheel center correction: ");
  Serial.println(wheel_calibrated_center);
  delay(3000);

  Serial.println();
  Serial.println("Hold wheel at full left turn.");
  wheel_calibrated_min = 0;    //reset the values
  delay(3000);

  Serial.println("Sampling full left turn:");
  delay(500);
  for (int i = 0; i < calibration_samples; i++) {    //take a number of readings
    Serial.print(".");
    wheel_calibrated_min += analogRead(analog_input_wheel);
    delay(5);
  }
  wheel_calibrated_min /= calibration_samples;    //average all readings
  Serial.println();
  Serial.print("Left turn correction: ");
  Serial.println(wheel_calibrated_min);
  delay(3000);

  Serial.println();
  Serial.println("Hold wheel at full right turn.");
  wheel_calibrated_max = 0;    //reset the values
  delay(3000);

  Serial.println("Sampling full right turn:");
  delay(500);
  for (int i = 0; i < calibration_samples; i++) {    //take a number of readings
    Serial.print(".");
    wheel_calibrated_max += analogRead(analog_input_wheel);
    delay(5);
  }
  wheel_calibrated_max /= calibration_samples;    //average all readings
  Serial.println();
  Serial.print("Right turn correction: ");
  Serial.println(wheel_calibrated_max);
  delay(3000);
}

void calibrate_trigger() {
  Serial.println("---Calibrating Trigger Input---");
  delay(1000);

  Serial.println();
  Serial.println("Release trigger to center position.");
  trigger_calibrated_center = 0;     //reset throttle center
  delay(3000);

  Serial.println("Sampling trigger center:");
  for (int i = 0; i < calibration_samples; i++) {    //take a number of readings
    Serial.print(".");
    trigger_calibrated_center += analogRead(analog_input_trigger);
    delay(5);
  }
  trigger_calibrated_center /= calibration_samples;    //average all readings
  Serial.println();
  Serial.print("Trigger center correction: ");
  Serial.println(trigger_calibrated_center);
  delay(3000);

  Serial.println();
  Serial.println("Hold the trigger in the full forward drive position.");
  trigger_calibrated_max = 0;    //reset the values
  delay(3000);

  Serial.println("Sampling full forward:");
  for (int i = 0; i < calibration_samples; i++) {    //take a number of readings
    Serial.print(".");
    trigger_calibrated_max += analogRead(analog_input_trigger);
    delay(5);
  }
  trigger_calibrated_max /= calibration_samples;    //average all readings
  Serial.println();
  Serial.print("Full forward correction: ");
  Serial.println(trigger_calibrated_max);
  delay(3000);

  Serial.println();
  Serial.println("Hold the trigger in the full reverse drive position.");
  trigger_calibrated_min = 0;    //reset the values
  delay(3000);

  Serial.println("Sampling full reverse:");
  for (int i = 0; i < calibration_samples; i++) {    //take a number of readings
    Serial.print(".");
    trigger_calibrated_min += analogRead(analog_input_trigger);
    delay(5);
  }
  trigger_calibrated_min /= calibration_samples;    //average all readings
  Serial.println();
  Serial.print("Full reverse correction: ");
  Serial.println(trigger_calibrated_min);
  delay(3000);
}
