/*
  Pong™ for the Arduino Uno R4 LED Matrix. With WiFi input.
  To play: 
  * open a POSIX command line interface (on MacOS, Terminal app. On Windows, use WSL) 
  * nc -u (the_board_ip_address) 8888
  * once connected, send: 
  *    w and s for left paddle up and down
  *    o and l for right paddle up and down
  *    hit return after each character

  Circuit:
  * Just the board

  Based on previous Pong™s:
  https://www.tigoe.com/pcomp/code/Processing/454/

 Make sure to add a file, arduino_secrets.h, with:
    #define SECRET_SSID ""    //  your network SSID (name)
    #define SECRET_PASS ""    // your network passwordx


  created 30 Jun 2021
  by Tom Igoe
*/
#include <WiFiS3.h>
#include "arduino_secrets.h"

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
int rightPaddleY = screenHeight / 2;
int leftPaddleY = screenHeight / 2;

// timing variables for the game play speed and state:
// last time the ball was moved, in ms:
long timeStamp = 0;
// interval between ball moves, in ms:
long interval = 120;
// game state:
bool gamePaused = false;

WiFiUDP Udp;            // instance of UDP library
const int port = 8888;  // port on which this client sends and receives


void setup() {
  // Serial is not really needed but helps for debugging:
  Serial.begin(9600);
  // Built in LED pin to indicate WiFi connection:
  pinMode(LED_BUILTIN, OUTPUT);
  if (!Serial) delay(3000);
  // try to connect to the network:
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Attempting to connect to: " + String(SECRET_SSID));
    //Connect to WPA / WPA2 network:
    WiFi.begin(SECRET_SSID, SECRET_PASS);
    delay(2000);
  }
  Serial.println("connected to: " + String(SECRET_SSID));
  IPAddress ip = WiFi.localIP();
  Serial.print(ip);
  Serial.print("  Signal Strength: ");
  Serial.println(WiFi.RSSI());
  // You're connected, turn on the LED:
  digitalWrite(LED_BUILTIN, HIGH);

  Udp.begin(port);

  // start the screen:
  matrix.begin();
  setPaddle(0, leftPaddleY, HIGH);
  setPaddle(screenWidth, leftPaddleY, HIGH);
}


void loop() {
  // monitor clients:
  checkUdp();
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

void checkUdp() {
  // if there's data available, read a packet

  int packetSize = Udp.parsePacket();
  if (packetSize) {

    Serial.print("Received from ");
    IPAddress remoteIp = Udp.remoteIP();
    Serial.print(remoteIp);
    Serial.print(", port ");
    Serial.println(Udp.remotePort());
    // read a char:
    char input = char(Udp.read());
    // see if it's a paddle movement key:
    switch (input) {
      case 'w':
        // left paddle up
        // erase the left paddle:
        setPaddle(0, leftPaddleY, LOW);
        // calculate new position:
        leftPaddleY = constrain(leftPaddleY--, 0, screenHeight);
        // re-draw the  left paddle:
        setPaddle(0, leftPaddleY, HIGH);
        break;
      case 's':
        // left paddle down    // erase the left paddle:
        setPaddle(0, leftPaddleY, LOW);
        // calculate new position:
        leftPaddleY = constrain(leftPaddleY++, 0, screenHeight);
        // re-draw the  left paddle:
        setPaddle(0, leftPaddleY, HIGH);
        break;
      case 'o':
        // right paddle up
        // erase the right paddle:
        setPaddle(screenWidth, rightPaddleY, LOW);
        rightPaddleY = constrain(rightPaddleY--, 0, screenHeight);
        // re-draw the  left paddle:
        setPaddle(screenWidth, rightPaddleY, HIGH);
        break;
      case 'l':
        // right paddle down
        // erase the right paddle:
        setPaddle(screenWidth, rightPaddleY, LOW);
        rightPaddleY = constrain(rightPaddleY++, 0, screenHeight);
        // re-draw the  left paddle:
        setPaddle(screenWidth, rightPaddleY, HIGH);
        break;
    }
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