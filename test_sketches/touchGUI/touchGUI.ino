/*
  GFX example for the Adafruit ILI9341 Breakout and Shield
  ----> http://www.adafruit.com/products/1651
  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
*/

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <TouchScreen.h>

// The display uses hardware SPI1, plus #6 & #3
#define TFT_CS 6
#define TFT_DC 3
Adafruit_ILI9341 tft = Adafruit_ILI9341(&SPI1, TFT_DC, TFT_CS);

/*
// Color definitions
#define ILI9341_BLACK 0x0000       ///<   0,   0,   0
#define ILI9341_NAVY 0x000F        ///<   0,   0, 123
#define ILI9341_DARKGREEN 0x03E0   ///<   0, 125,   0
#define ILI9341_DARKCYAN 0x03EF    ///<   0, 125, 123
#define ILI9341_MAROON 0x7800      ///< 123,   0,   0
#define ILI9341_PURPLE 0x780F      ///< 123,   0, 123
#define ILI9341_OLIVE 0x7BE0       ///< 123, 125,   0
#define ILI9341_LIGHTGREY 0xC618   ///< 198, 195, 198
#define ILI9341_DARKGREY 0x7BEF    ///< 123, 125, 123
#define ILI9341_BLUE 0x001F        ///<   0,   0, 255
#define ILI9341_GREEN 0x07E0       ///<   0, 255,   0
#define ILI9341_CYAN 0x07FF        ///<   0, 255, 255
#define ILI9341_RED 0xF800         ///< 255,   0,   0
#define ILI9341_MAGENTA 0xF81F     ///< 255,   0, 255
#define ILI9341_YELLOW 0xFFE0      ///< 255, 255,   0
#define ILI9341_WHITE 0xFFFF       ///< 255, 255, 255
#define ILI9341_ORANGE 0xFD20      ///< 255, 165,   0
#define ILI9341_GREENYELLOW 0xAFE5 ///< 173, 255,  41
#define ILI9341_PINK 0xFC18        ///< 255, 130, 198
 */

void setup() {
  // put your setup code here, to run once:
  tft.begin();
  tft.setRotation(2);
  //drawThrottleCalib();
  drawSteerCalib();

}

void loop() {
  // put your main code here, to run repeatedly:

}

void drawThrottleCalib() {
  tft.fillScreen(ILI9341_BLACK);
  tft.fillRect(0, 0, 240, 51, ILI9341_NAVY);
  tft.drawRect(0,0, 240, 320, ILI9341_NAVY);
  tft.drawRect(1,1, 238, 318, ILI9341_NAVY);
  //tft.fillRect(120, 0, 2, 51, ILI9341_BLACK);
  tft.setCursor(50, 2);
  tft.setTextColor(ILI9341_LIGHTGREY);
  tft.setTextSize(3);
  tft.print("Throttle");
  tft.setCursor(22, 26);
  tft.print("Calibration");
}

void drawSteerCalib() {
  tft.fillScreen(ILI9341_BLACK);
  tft.fillRect(0, 0, 240, 51, ILI9341_DARKCYAN);
  tft.drawRect(0,0, 240, 320, ILI9341_DARKCYAN);
  tft.drawRect(1,1, 238, 318, ILI9341_DARKCYAN);
  //tft.fillRect(120, 0, 2, 51, ILI9341_BLACK);
  tft.setCursor(50, 2);
  tft.setTextColor(ILI9341_LIGHTGREY);
  tft.setTextSize(3);
  tft.print("Steering");
  tft.setCursor(22, 26);
  tft.print("Calibration");
}
