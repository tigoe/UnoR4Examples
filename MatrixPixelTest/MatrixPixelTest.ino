/*
  A test of the Uno R4 LED Matrix pixels.
  Turns them all on one at a time

  created 28 Jun 2023
  by Tom Igoe
*/
#include "Arduino_LED_Matrix.h"

const byte canvasWidth = 12;
const byte canvasHeight = 8;

ArduinoLEDMatrix matrix;
byte pixels[canvasHeight][canvasWidth] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

void setup() {
  Serial.begin(9600);
  matrix.begin();
}

void loop() {
  // loop over the X pixels:
  for (int x = 0; x < canvasWidth; x++) {
    // loop over the Y pixels:
    for (int y = 0; y < canvasHeight; y++) {
      // turn each one on:
      point(x, y, 1);
      matrix.renderBitmap(pixels, 8, 12);
      delay(100);
    }
  }
  delay(1000);
    // loop over the X pixels:
  for (int x = 0; x < canvasWidth; x++) {
      // loop over the Y pixels:
    for (int y = 0; y < canvasHeight; y++) {
       // turn each one off:
      point(x, y, 0);
      matrix.renderBitmap(pixels, 8, 12);
      delay(100);
    }
  }
  delay(1000);
}

void point(int x, int y, int state) {
  pixels[y][x] = state;
}