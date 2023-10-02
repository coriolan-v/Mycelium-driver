#include <OctoWS2811.h>
#include <FastLED.h>
#include <Arduino.h>
  
  const int numPins = 32;
  //byte pinList[numPins] = {33, 34, 35, 36, 37, 38, 39, 40};

  byte pinList[numPins] = {
  23, 22, 21, 20, 19, 18, 17, 16,
  15, 14, 41, 40, 39, 38, 37, 36,
  0, 1, 2, 3, 4, 5, 6, 7,
  8, 9, 10, 11, 12, 24, 25, 26
};

  const int ledsPerStrip = 100;
  CRGB rgbarray[numPins * ledsPerStrip];

  
//#include <Util.h>

template <EOrder RGB_ORDER = RGB,
          uint8_t CHIP = WS2811_800kHz>
class CTeensy4Controller : public CPixelLEDController<RGB_ORDER, 8, 0xFF>
{
    OctoWS2811 *pocto;

public:
    CTeensy4Controller(OctoWS2811 *_pocto)
        : pocto(_pocto){};

    virtual void init() {}
    virtual void showPixels(PixelController<RGB_ORDER, 8, 0xFF> &pixels)
    {

        uint32_t i = 0;
        while (pixels.has(1))
        {
            uint8_t r = pixels.loadAndScale0();
            uint8_t g = pixels.loadAndScale1();
            uint8_t b = pixels.loadAndScale2();
            pocto->setPixel(i++, r, g, b);
            pixels.stepDithering();
            pixels.advanceData();
        }

        pocto->show();
    }
};

  // These buffers need to be large enough for all the pixels.
  // The total number of pixels is "ledsPerStrip * numPins".
  // Each pixel needs 3 bytes, so multiply by 3.  An "int" is
  // 4 bytes, so divide by 4.  The array is created using "int"
  // so the compiler will align it to 32 bit memory.
  DMAMEM int displayMemory[ledsPerStrip * numPins * 3 / 4];
  int drawingMemory[ledsPerStrip * numPins * 3 / 4];
  OctoWS2811 octo(ledsPerStrip, displayMemory, drawingMemory, WS2811_RGB | WS2811_800kHz, numPins, pinList);

CTeensy4Controller<RGB, WS2811_800kHz> *pcontroller;

void setup() {
  // put your setup code here, to run once:
    octo.begin();
    pcontroller = new CTeensy4Controller<RGB, WS2811_800kHz>(&octo);

    FastLED.setBrightness(25);
    FastLED.addLeds(pcontroller, rgbarray, numPins * ledsPerStrip);

 for (int i=0; i < 200; i++) {
    octo.setPixel(i, 0x160000);
    octo.show();
    //delayMicroseconds(20);
  }
      
       // Move a single white led 
//   for(int whiteLed = 0; whiteLed < 100; whiteLed = whiteLed + 1) {
//      // Turn our current led on to white, then show the leds
//      rgbarray[whiteLed] = CRGB::White;
//
//      // Show the leds (only one of which is set to white, from above)
//      FastLED.show();
//      //pcontroller->showPixels();
//
//   
//  
////octo.show();
//      fill_solid(rgbarray, 100, CRGB::Blue);
//       
//      // Wait a little bit
//      delay(100);
//
//      // Turn our current led back to black for the next loop around
//      rgbarray[whiteLed] = CRGB::Black;
//   }
}

void loop() {
  // put your main code here, to run repeatedly:
 for (int i=0; i < 2000; i++) {
    octo.setPixel(i, 0x160000);
    octo.show();
    //delayMicroseconds(20);
  }

  //delay(1000);

   for (int i=0; i < 2000; i++) {
    octo.setPixel(i, 0x000000);
    octo.show();
    //delayMicroseconds(20);
  }

  //delay(1000);
}
