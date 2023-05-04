#include <Artnet.h>
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>
#include <SPI.h>

#include "FastLED.h"
#define NUM_STRIPS 3
#define NUM_LEDS_PER_STRIP 100
CRGB leds[NUM_STRIPS][NUM_LEDS_PER_STRIP];

byte dataPins[] = {
  23, 22, 21, 20, 19, 18, 17, 16,
  15, 14, 41, 40, 39, 38, 37, 36,
  0, 1, 2, 3, 4, 5, 6, 7,
  8  //, 9, 10, 11, 12, 24, 25, 26
};

Artnet artnet;
const int startUniverse = 1;                          // CHANGE FOR YOUR SETUP most software this is 1, some software send out artnet first universe as 0.
const int numberOfChannels = NUM_LEDS_PER_STRIP * 3;  // Total number of channels you want to receive (1 led = 3 channels)
const int maxUniverses = numberOfChannels / 512 + ((numberOfChannels % 512) ? 1 : 0);
bool universesReceived[maxUniverses];
//bool sendFrame = 1;
//int previousDataLength = 0;

// Change ip and mac address for your setup
byte ip[] = { 192, 168, 1, 3 };
byte mac[] = { 0x04, 0xE9, 0xE5, 0x00, 0x69, 0xEC };

void setup() {
  // put your setup code here, to run once:
  artnet.begin(mac, ip);

  //for (int i = 0; i < NUM_STRIPS; i++) {
    FastLED.addLeds<NEOPIXEL, 23>(leds[0], NUM_LEDS_PER_STRIP);
    FastLED.addLeds<NEOPIXEL, 22>(leds[1], NUM_LEDS_PER_STRIP);
    FastLED.addLeds<NEOPIXEL, 21>(leds[2], NUM_LEDS_PER_STRIP);
 // }



initTest();

// this will be called for each packet received
artnet.setArtDmxCallback(onDmxFrame);
}

void loop() {
  // put your main code here, to run repeatedly:
  // we call the read function inside the loop
  artnet.read();
}

void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data) {


   Serial.print(universe);
  Serial.print("/");
  Serial.print(length);
  Serial.print("/");
  Serial.print(sequence);
  Serial.print("/");
  Serial.print(data[0]);
  Serial.println();

  //if (universe == 1) {
    for (unsigned int i = 0; i < length / 3; i++) {
      int led = i + 1;
      //if (led < numLeds)
      //leds.setPixelColor(leds, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
      leds[universe-1][i].setRGB( data[i * 3 + 0], data[i * 3 + 1], data[i * 3 + 2]);
      //leds2.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
      //leds3.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
    //leds.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);

     
    //leds2.show();
   // leds3.show();
    // Reset universeReceived to 0
    //memset(universesReceived, 0, maxUniverses);
 // }

 if(universe == 3)
 {
    FastLED.show();
 }

}

void initTest() {
   for (int i = 0; i < NUM_STRIPS; i++) {
      for(int p = 0; p < NUM_LEDS_PER_STRIP; p = p + 1) {
      // Turn our current led on to white, then show the leds
      leds[i][p] = CRGB::White;

      // Show the leds (only one of which is set to white, from above)
      FastLED.show();

      // Wait a little bit
      delay(1);

      // Turn our current led back to black for the next loop around
      leds[i][p] = CRGB::Black;
   }
  }
}
