/*
  Optimized fork of nRF24L01 for Arduino & Raspberry Pi/Linux Devices:
  TMRh20/RF24, https://github.com/tmrh20/RF24/
  nRF24/RF24 is licensed under the
  GNU General Public License v2.0

  Based on:
  https://howtomechatronics.com/tutorials/arduino/arduino-wireless-communication-nrf24l01-tutorial/
  https://howtomechatronics.com/projects/diy-arduino-rc-transmitter/

  GFX example for the Adafruit ILI9341 Breakout and Shield
  ----> http://www.adafruit.com/products/1651
  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Adafruit_BusIO_Register.h>
#include <Adafruit_SPIDevice.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

//nRF24 pins hardware +SPI
#define CE_PIN 9
#define CSN_PIN 10
RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = "00001";

//The display uses hardware SPI1, plus #0 & #2
#define TFT_CS 0
#define TFT_DC 2
Adafruit_ILI9341 tft = Adafruit_ILI9341(&SPI1, TFT_DC, TFT_CS);

//Custom color definitions
#define SHADOW 0x2965

//Joystick pins
#define joy_X A0
#define joy_Y A1

unsigned int rawBatt = 185;
unsigned int rawTemp1 = 225;
unsigned int rawTemp2 = 231;
int rawCurrentMotor1 = 521;
int rawCurrentMotor2 = 482;

unsigned int oldBatt;
unsigned int oldTemp1;
unsigned int oldTemp2;
int oldCurrentMotor1;
int oldCurrentMotor2;

//package to contain outgoing data
struct dataPackage {
  byte throttle;
  byte steering;
  byte flag_motor_stop;
};
dataPackage data;

//package to contain incoming acknowledge data
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
};
ackPackage ack;

void setup() {
  tft.begin();
  tft.setRotation(2);
  drawHome();

  radio.begin();
  radio.openWritingPipe(address);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MIN);
  radio.enableAckPayload();
  radio.setRetries(3, 5);

  //set default data values
  data.throttle = 64;
  data.steering = 64;
}

void loop() {
  // put your main code here, to run repeatedly:
  data.throttle = map(analogRead(A1), 0, 1023, 127, 0);
  data.steering = map(analogRead(A0), 0, 1023, 0, 127);

  radio.write(&data, sizeof(dataPackage));
  if (radio.isAckPayloadAvailable() ) {
    radio.read(&ack, sizeof(ackPackage));

    //reconstitute acknowledge bytes
    rawBatt = (ack.highBatt << 8 ) + ack.lowBatt;
    rawTemp1 = (ack.highTemp1 << 8) + ack.lowTemp1;
    rawTemp2 = (ack.highTemp2 << 8) + ack.lowTemp2;
    rawCurrentMotor1 = (ack.highIM1 << 8) + ack.lowIM1;
    rawCurrentMotor2 = (ack.highIM2 << 8) + ack.lowIM2;

    //convert received tenths/hundredths data to whole numbers
    int mainBattery = rawBatt / 10;
    int temp1 = rawTemp1 / 10;
    int temp2 = rawTemp2 / 10;
    int currentM1 = rawCurrentMotor1 / 100;
    int currentM2 = rawCurrentMotor2 / 100;

    tft.setTextSize(3);
    tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);

    //print battery value
    tft.setCursor(112, 30);
    tft.print(mainBattery);
    tft.print(".");
    tft.print(rawBatt - (mainBattery * 10));
    tft.print("V ");

    //print temp1 value
    tft.setCursor(112, 54);
    tft.print(temp1);
    tft.print(".");
    tft.print(rawTemp1 - (temp1 * 10));
    tft.print((char)247);
    tft.print("C ");

    //prtin temp2 value
    tft.setCursor(112, 78);
    tft.print(temp2);
    tft.print(".");
    tft.print(rawTemp2 - (temp2 * 10));
    tft.print((char)247);
    tft.print("C ");

    //print M1 current value
    tft.setCursor(80, 126);
    tft.print(currentM1);
    tft.print(".");
    tft.print(rawCurrentMotor1 - (currentM1 * 100));
    tft.print("A ");

    //print M2 current value
    tft.setCursor(80, 174);
    tft.print(currentM2);
    tft.print(".");
    tft.print(rawCurrentMotor2 - (currentM2 * 100));
    tft.print("A ");
  }
}
