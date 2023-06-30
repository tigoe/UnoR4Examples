/*
  LED Matrix position test with Serial input.
  To turn on any pixel in the LED matrix,
  enter it's position via the serial monitor like so:
  x, y, state
  where x is the horizontal position, y is the vertical, and state is 1 or 0,
  or HIGH or LOW. 
  don't send a new line or carriage return or you'll turn off
  pixel (0,0).
  
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
byte pixels[8][12];

void setup() {
  // Serial is not really needed but helps for debugging:
  Serial.begin(9600);
  // start the screen:
  matrix.begin();
}


void loop() {
  // read serial input, looking for
  // three comma-seprated numerals:
  if (Serial.available()) {
    int x = Serial.parseInt();
    int y = Serial.parseInt();
    int state = Serial.parseInt();
    // print them out:
    Serial.print("(");
    Serial.print(x);
    Serial.print(",");
    Serial.print(y);
    Serial.print(") = ");
    Serial.println(state);

    // set the point in the pixel array with the values you got:
    pixels[y][x] = state;

    // update the screen:
    matrix.renderBitmap(pixels, 8, 12);
  }
}
