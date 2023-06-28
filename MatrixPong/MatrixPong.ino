/*
  Pong™ for the Arduino Uno R4 LED Matrix.
  To play: Move paddles up and down
  Circuit:
  * 10K potentiometer attached to A0 and A1

  Based on previous Pong™s:
  https://www.tigoe.com/pcomp/code/Processing/454/

  created 28 Jun 2021
  by Tom Igoe
*/

// include the LED Matrix library from the Uno R4 core:
#include "Arduino_LED_Matrix.h"
// make an instance of the library:
ArduinoLEDMatrix matrix;
// pixel array that's the screen buffer
// this way takes more memory, but makes it easier
// to visualize the screen in the array:
byte pixels[8][12] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

// x = 0-7, aka 8 pixels:
const int screenHeight = 7;
// y = 0-11, aka 12 pixels:
const int screenWidth = 11;

// ball position and direction variables:
int ballX = screenWidth / 2;
int ballY = screenHeight / 2;
int ballDirectionY = 1;
int ballDirectionX = 1;

// Y position for the middle of each paddle:
int rightPaddleY = 0;
int leftPaddleY = 0;

// timing variables for the game play speed and state:
// last time the ball was moved, in ms:
long timeStamp = 0;
// interval between ball moves, in ms:
long interval = 120;
// game state:
boolean gamePaused = false;

void setup() {
  // Serial is not really needed but helps for debugging:
  Serial.begin(9600);
  // start the screen:
  matrix.begin();
}


void loop() {
  // read input:
  readSensors();
  // If the game is paused:
  if (gamePaused) {
    if (millis() - timeStamp > interval * 10) {
      // if enough time has passed, start the game again:
      gamePaused = false;
    }
  }
  // if the game isn't paused, and enough time between ball moves
  // has passed, move the ball and update the timestamp:
  else {
    if (millis() - timeStamp > interval) {
      moveBall();
      timeStamp = millis();
    }
  }
  // update the screen:
  matrix.renderBitmap(pixels, 8, 12);
}

// draw a point on the screen at (x, y).
// state is whether it's on or off:
void point(int x, int y, int state) {
  // make sure x and y are within the screen dimensions:
  if (x <= screenWidth && x >= 0
      && y <= screenHeight
      && y >= 0) {
    // set the state in the pixel array:
    pixels[y][x] = state;
  }
}

void readSensors() {
  // erase the left paddle:
  setPaddle(0, leftPaddleY, LOW);
  // erase the right paddle:
  setPaddle(screenWidth, rightPaddleY, LOW);

  // read the sensors for X and Y values:
  leftPaddleY = map(analogRead(A0), 0, 1023, 0, 7);
  rightPaddleY = map(analogRead(A1), 0, 1023, 0, 7);

  // re-draw the  left paddle:
  setPaddle(0, leftPaddleY, HIGH);
  // re-draw the right paddle:
  setPaddle(screenWidth, rightPaddleY, HIGH);
}


void setPaddle(int paddleX, int paddleY, int state) {
  // draw the center of the paddle:
  point(paddleX, paddleY, state);
  // draw the top pixel of the paddle:
  if (paddleY < screenHeight) {
    point(paddleX, paddleY + 1, state);
  }
  // draw the bottom pixel of the paddle:
  if (paddleY > 0) {
    point(paddleX, paddleY - 1, state);
  }
}

void moveBall() {
  // check to see if the ball is in the horizontal range
  // of the paddles:
  // off screen right:
  if (ballX >= screenWidth - 1) {
    // if the ball's next Y position is within the span
    // of the right paddle, reverse its  X direction:
    if ((ballY + ballDirectionY >= rightPaddleY - 1)
        && (ballY + ballDirectionY <= rightPaddleY + 1)) {
      // reverse the ball horizontal direction:
      ballDirectionX = -ballDirectionX;
    }
  }

  // off sceeen left:
  if (ballX <= 1) {
    // if the ball's next Y position is within the span
    // of the left paddle, reverse its  X direction:
    if ((ballY + ballDirectionY >= leftPaddleY - 1)
        && (ballY + ballDirectionY <= rightPaddleY + 1)) {
      // reverse the ball horizontal direction:
      ballDirectionX = -ballDirectionX;
    }
  }
  // if the ball goes off the screen bottom,
  // reverse its Y direction:
  if (ballY == screenHeight) {
    ballDirectionY = -ballDirectionY;
  }
  // if the ball goes off the screen top,
  // reverse its X direction:
  if (ballY == 0) {
    ballDirectionY = -ballDirectionY;
  }

  // clear the ball's previous position:
  point(ballX, ballY, LOW);

  // if the ball goes off the screen left or right:
  if ((ballX == 0) || (ballX == screenWidth)) {
    // reset the ball:
    ballX = screenWidth / 2;
    ballY = screenHeight / 2;
    // pause  and note the time you paused:
    gamePaused = true;
    timeStamp = millis();
  }
  // increment the ball's position in both directions:
  ballX = ballX + ballDirectionX;
  ballY = ballY + ballDirectionY;

  // if the game isn't paused, set the ball
  // to its new position:
  if (!gamePaused) {
    // set the new position:
    point(ballX, ballY, HIGH);
  }
}