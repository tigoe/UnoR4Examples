# UnoR4Examples
This is a repository for testing out ideas on the Uno R4. Most of these are not finished. 

## Understanding the LED Matrix Library
In order to control the 12x8 LED matrix on the Uno R4 WiFi, you need a space in memory that's at least 96 bits in size. The library provides two ways to do this. 

The first is simply to make a two-dimensional array of bytes like so:
````
byte pixels[8][12] = {
  { 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0 },
  { 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0 },
  { 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0 },
  { 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
  { 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};
````

This option is simple to understand, because you can see the image in the pattern of the array. The ones in the array above form a heart, and that's the image you'd see on the screen. 

This method takes more memory than is needed, however. Even though each LED needs only a single bit to store its state, you're using eight bits (a byte). The more memory-efficient method to store a frame is to use an array of 32-bit integers. Here's the same heart in that form:

````
unsigned long pixels[] = {
  0x3184a444,
  0x42081100,
  0xa0040000
};
````
An unsigned long variable holds 32 bits, and 96/32 is 3, so an unsigned long array is an efficient way to hold all the bits you need for the LED matrix. 

But how do those hexadecimal values relate to the positions of the pixels? To find out, convert the hexadecimal values to binary values. Here's a code snippet that will do this:

````
for (int b = 0; b < 3; b++) {
    Serial.println(pixels[b], BIN);
  }
````

This will print out all the bit values of the array. The output will look like this:

````
110001100001001010010001000100
1000010000010000001000100000000
10100000000001000000000000000000
````

This method doesn't show you all the bits, though. Each array element should have 32 bits. If you add zeros to show all 32 bits of each element, you get:

````
00110001100001001010010001000100
01000010000010000001000100000000
10100000000001000000000000000000
````
Now divide it into groups of 12 bits and you've got the heart back:
````
001100011000
010010100100
010001000100
001000001000
000100010000
000010100000
000001000000
000000000000
````

Here's a code snipped that will print out the frame this way:

````
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
````

The Matrix drawing tool actually exports four 32-bit ints. The last int is the number of wait before the next frame in an animation sequence. This allows for animation sequences with varying rhythms (Thanks @facchinm for the explanation).
