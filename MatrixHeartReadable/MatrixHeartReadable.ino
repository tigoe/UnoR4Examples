/*
  LED Matrix example to explain 
  ways of storing the bits of a frame
  
  created 30 Jun 2021
  by Tom Igoe
*/

// include the LED Matrix library from the Uno R4 core:
#include "Arduino_LED_Matrix.h"
// make an instance of the library:
ArduinoLEDMatrix matrix;
// pixel array that's the screen buffer
// this way takes more memory, but makes it easier
// to visualize the screen in the array:
byte heart[8][12] = {
  { 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0 },
  { 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0 },
  { 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0 },
  { 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
  { 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

// The same image in a pixel array stored in 32-bit integers
// for more efficient storage. It's written in hexadecimal values:
unsigned long pixels[] = {
  0x3184a444,
  0x42081100,
  0xa0040000
};
// the same 32-bit integer array written as binary values:
//  unsigned long pixels[] = {
//   0b00110001100001001010010001000100,
//   0b01000010000010000001000100000000,
//   0b10100000000001000000000000000000
// };

// an empty frame, for clearing the screen:
unsigned long blankFrame[] = {
  0, 0, 0
};


void setup() {
  // Serial is not really needed but helps for debugging:
  Serial.begin(9600);
  // start the screen:
  matrix.begin();
  // wait for serial monitor to open:
  while (!Serial)
    ;
  Serial.println("The bits, as stored in the array of 32-bit ints:");
  // iterate over the array:
  for (int b = 0; b < 3; b++) {
    // print the value of each 32-bit int:
    Serial.print("Hex: ");
    Serial.print(pixels[b], HEX);
    Serial.print("\tBinary: ");
    Serial.println(pixels[b], BIN);
  }
  // add a blank line:
  Serial.println();

  Serial.println("The bits, organized by matrix row:");
  // keep track of the total bits printed:
  int totalBits = 0;
  // iterate over the bytes of the array:
  for (int arrayIndex = 0; arrayIndex < 3; arrayIndex++) {
    // get each element in the array:
    unsigned long arrayElement = pixels[arrayIndex];
    // read the bits from highest to lowest:
    for (int bitNum = 31; bitNum >= 0; bitNum--) {
      // print the bit:
      Serial.print(bitRead(arrayElement, bitNum));
      // add one to the total number of bits:
      totalBits++;
      // every 12th bit, print a newline:
      if (totalBits % 12 == 0) Serial.println();
    }
  }
}

void loop() {
  // update the screen:
  // if you use the 8x12 byte array, use renderBitmap:
  matrix.renderBitmap(heart, 8, 12);
  delay(1000);
  // clear the screen:
  matrix.loadFrame(blankFrame);
  delay(1000);
  // if you use the unsigned long array, use loadPixels:
  matrix.loadFrame(pixels);
  delay(1000);
  // clear the screen:
  matrix.loadFrame(blankFrame);
  delay(1000);
}
