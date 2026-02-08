/*
  Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 10); // CE, CSN

const byte address[6] = "00001";

// Max size of this struct is 32 bytes - NRF24L01 buffer limit
struct Data_Package {
  byte throttle;
  byte steeting;
};

Data_Package data; //Create a variable with the above structure

void setup() {
  radio.begin();
  radio.openWritingPipe(address);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();

  //set default data values
  data.throttle = 64;
  data.steeting = 64;
}

void loop() {
  // put your main code here, to run repeatedly:
  data.throttle = map(analogRead(A1), 0, 1023, 127, 0);
  data.steeting = map(analogRead(A0), 0, 1023, 0, 127);

  radio.write(&data, sizeof(Data_Package));
}
