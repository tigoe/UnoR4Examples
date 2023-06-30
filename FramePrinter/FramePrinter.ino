/*
  Frame pixel printer. 
  Takes the frame animation from the LED Matrix Demo
  and translates it into a 2-dimensional printed array 
  that actually looks like the screen. Sort of.
  
  modified from Martino Facchin's LED Matrix demo  
  30 Jun 2023 
*/


// include the LED Matrix library from the Uno R4 core:
#include "Arduino_LED_Matrix.h"
// make an instance of the library:
ArduinoLEDMatrix matrix;
//include the "animation.h" header file that stores the frames for the animation
#include "animation.h"

void setup() {
  // initialize serial, wait 3 sec for serial monitor to open:
  Serial.begin(9600);
  if (!Serial) delay(3000);
  // the count of frames in the animation array:
  int framesLength = 57;
  // a counter to keep track of your X position in the matrix display:
  int x = 0;
  // loop over the frames array. The two dimensions of it don't mean
  // anything to your screen position, they're just big groups
  // of bits jammed together into the biggest variable type available.
  // so we're calling the two dimensions i and j:
  for (int i = 0; i < framesLength; i++) {
    for (int j = 0; j < 4; j++) {
      // the variable type for the array has 32 bits.
      // So count from 31 to 0, getting each bit from
      // highest to lowest:
      for (int bit = 31; bit > -1; bit--) {
        // read the bit:
        int thisPixel = bitRead(frames[i][j], bit);
        // if the bit is a 0, draw a . (nice and small):
        if (thisPixel == 0) {
          Serial.print(".");
          // if the bit is a 1, draw a 1:
        } else {
          Serial.print(thisPixel);
        }
        // increment the x position:
        x++;
        // every 12 pixels, you're at the end of a row.
        // so print a newline:
        if (x % 12 == 0) Serial.println();
      }
    }
    // now you're at the end of a 12x8 pixel frame.
    Serial.println();
    Serial.println("------------");
    // reset the x counter:
    x = 0;
  }
  // now actually show the animation.
  //load frames from the animation.h file
  matrix.loadSequence(frames);
  // start the matrix
  matrix.begin();
  matrix.autoscroll(300);
  //play the animation on the matrix
  matrix.play(true);
}

void loop() {
}
