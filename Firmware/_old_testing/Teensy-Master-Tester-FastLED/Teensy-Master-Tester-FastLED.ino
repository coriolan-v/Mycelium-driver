// Use if you want to force the software SPI subsystem to be used for some reason (generally, you don't)
// #define FASTLED_FORCE_SOFTWARE_SPI
// Use if you want to force non-accelerated pin access (hint: you really don't, it breaks lots of things)
// #define FASTLED_FORCE_SOFTWARE_SPI
// #define FASTLED_FORCE_SOFTWARE_PINS
#include <FastLED.h>

///////////////////////////////////////////////////////////////////////////////////////////
//
// Move a white dot along the strip of leds.  This program simply shows how to configure the leds,
// and then how to turn a single pixel white and then off, moving down the line of pixels.
// 

// How many leds are in the strip?
#define NUM_LEDS 100



// This is an array of leds.  One item for each led in your strip.
CRGB leds[NUM_LEDS];
//CRGB leds[48][NUM_LEDS];

// This function sets up the ledsand tells the controller about them
void setup() {
	// sanity check delay - allows reprogramming if accidently blowing power w/leds
   	
 FastLED.addLeds<WS2811, 0, RGB>(leds, NUM_LEDS);
  FastLED.addLeds<WS2811, 1, RGB>(leds, NUM_LEDS);
   FastLED.addLeds<WS2811, 2, RGB>(leds, NUM_LEDS);
   FastLED.addLeds<WS2811, 3, RGB>(leds, NUM_LEDS);
   FastLED.addLeds<WS2811, 4, RGB>(leds, NUM_LEDS);
   FastLED.addLeds<WS2811, 5, RGB>(leds, NUM_LEDS);
   FastLED.addLeds<WS2811, 6, RGB>(leds, NUM_LEDS);
   FastLED.addLeds<WS2811, 7, RGB>(leds, NUM_LEDS);
   FastLED.addLeds<WS2811, 8, RGB>(leds, NUM_LEDS);
   FastLED.addLeds<WS2811, 9, RGB>(leds, NUM_LEDS);
   FastLED.addLeds<WS2811, 10, RGB>(leds, NUM_LEDS);
   FastLED.addLeds<WS2811, 11, RGB>(leds, NUM_LEDS);
   FastLED.addLeds<WS2811, 12, RGB>(leds, NUM_LEDS);
   //FastLED.addLeds<WS2811, 13, RGB>(leds, NUM_LEDS);
   FastLED.addLeds<WS2811, 14, RGB>(leds, NUM_LEDS);
   FastLED.addLeds<WS2811, 15, RGB>(leds, NUM_LEDS);
   FastLED.addLeds<WS2811, 16, RGB>(leds, NUM_LEDS);
   FastLED.addLeds<WS2811, 17, RGB>(leds, NUM_LEDS);
   FastLED.addLeds<WS2811, 18, RGB>(leds, NUM_LEDS);
   FastLED.addLeds<WS2811, 19, RGB>(leds, NUM_LEDS);
   FastLED.addLeds<WS2811, 20, RGB>(leds, NUM_LEDS);
   FastLED.addLeds<WS2811, 21, RGB>(leds, NUM_LEDS);
   //FastLED.addLeds<WS2811, 22, RGB>(leds, NUM_LEDS);
   //FastLED.addLeds<WS2811, 23, RGB>(leds, NUM_LEDS);
   FastLED.addLeds<WS2811, 24, RGB>(leds, NUM_LEDS);
   FastLED.addLeds<WS2811, 25, RGB>(leds, NUM_LEDS);
   FastLED.addLeds<WS2811, 26, RGB>(leds, NUM_LEDS);
   FastLED.addLeds<WS2811, 27, RGB>(leds, NUM_LEDS);
   FastLED.addLeds<WS2811, 28, RGB>(leds, NUM_LEDS);
   FastLED.addLeds<WS2811, 29, RGB>(leds, NUM_LEDS);
   FastLED.addLeds<WS2811, 30, RGB>(leds, NUM_LEDS);
   FastLED.addLeds<WS2811, 31, RGB>(leds, NUM_LEDS);
   FastLED.addLeds<WS2811, 32, RGB>(leds, NUM_LEDS);
   FastLED.addLeds<WS2811, 33, RGB>(leds, NUM_LEDS);
   FastLED.addLeds<WS2811, 34, RGB>(leds, NUM_LEDS);
   FastLED.addLeds<WS2811, 35, RGB>(leds, NUM_LEDS);
   FastLED.addLeds<WS2811, 36, RGB>(leds, NUM_LEDS);
   FastLED.addLeds<WS2811, 37, RGB>(leds, NUM_LEDS);
   FastLED.addLeds<WS2811, 38, RGB>(leds, NUM_LEDS);
   FastLED.addLeds<WS2811, 39, RGB>(leds, NUM_LEDS);
   //FastLED.addLeds<WS2811, 40, RGB>(leds, NUM_LEDS);
   // FastLED.addLeds<WS2811, 41, RGB>(leds, NUM_LEDS);
//    FastLED.addLeds<WS2811, 49, RGB>(leds, NUM_LEDS);
//    FastLED.addLeds<WS2811, 50, RGB>(leds, NUM_LEDS);
//    FastLED.addLeds<WS2811, 51, RGB>(leds, NUM_LEDS);
//    FastLED.addLeds<WS2811, 52, RGB>(leds, NUM_LEDS);
//    FastLED.addLeds<WS2811, 53, RGB>(leds, NUM_LEDS);
//  FastLED.addLeds<WS2811, 54, RGB>(leds, NUM_LEDS);

    // for(int i = 0; i<48; i++){
    //   FastLED.addLeds<WS2811, 1, RGB>(leds, NUM_LEDS);
    // }
}

// This function runs over and over, and is where you do the magic to light
// your leds.
void loop() {
   // Move a single white led 
   for(int whiteLed = 0; whiteLed < NUM_LEDS; whiteLed = whiteLed + 1) {
      // Turn our current led on to white, then show the leds
      leds[whiteLed] = CRGB::Red;

      // Show the leds (only one of which is set to white, from above)
      FastLED.show();

      // Wait a little bit
      delay(100);

      // Turn our current led back to black for the next loop around
      leds[whiteLed] = CRGB::Black;
   }
}
