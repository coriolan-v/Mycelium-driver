/*
This example will receive multiple universes via Artnet and control a strip of ws2811 leds via 
Adafruit's NeoPixel library: https://github.com/adafruit/Adafruit_NeoPixel
This example may be copied under the terms of the MIT license, see the LICENSE file for details
*/

#include <Artnet.h>
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>
#include <SPI.h>
#include <Adafruit_NeoPixel.h>

// Neopixel settings
const int numLeds = 100;                   // change for your setup
const int numberOfChannels = numLeds * 3;  // Total number of channels you want to receive (1 led = 3 channels)
const byte dataPin = 21;


byte dataPins[] = {
  23, 22, 21, 20, 19, 18, 17, 16,
  15, 14, 41, 40, 39, 38, 37, 36  //,
  //0, 1, 2, 3, 4, 5, 6, 7//,
  //8, 9, 10, 11, 12, 24, 25, 26
};

Adafruit_NeoPixel leds = Adafruit_NeoPixel(numLeds, dataPins[0], NEO_GRB);
Adafruit_NeoPixel leds2 = Adafruit_NeoPixel(numLeds, dataPins[1], NEO_GRB);
Adafruit_NeoPixel leds3 = Adafruit_NeoPixel(numLeds, dataPins[2], NEO_GRB);



// Artnet settings
Artnet artnet;
const int startUniverse = 1;  // CHANGE FOR YOUR SETUP most software this is 1, some software send out artnet first universe as 0.

// Check if we got all universes
const int maxUniverses = numberOfChannels / 512 + ((numberOfChannels % 512) ? 1 : 0);
bool universesReceived[maxUniverses];
bool sendFrame = 1;
int previousDataLength = 0;

// Change ip and mac address for your setup
byte ip[] = { 192, 168, 1, 3 };
byte mac[] = { 0x04, 0xE9, 0xE5, 0x00, 0x69, 0xEC };

void setup() {
  //Serial.begin(115200);
  artnet.begin(mac, ip);
  leds.begin();
  leds2.begin();
  leds3.begin();
  initTest();

  // this will be called for each packet received
  artnet.setArtDmxCallback(onDmxFrame);
}

void loop() {
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
  Serial.print(*data);
  Serial.println();

  if (universe == 1) {
    for (unsigned int i = 0; i < length / 3; i++) {
      int led = i + 1;
      //if (led < numLeds)
      leds.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
      //leds2.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
      //leds3.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
    //leds.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);

      leds.show();
    //leds2.show();
   // leds3.show();
    // Reset universeReceived to 0
    //memset(universesReceived, 0, maxUniverses);
  }

   if (universe == 2) {
    for (unsigned int i = 0; i < length / 3; i++) {
      int led = i + 1;
      //if (led < numLeds)
      leds2.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
      //leds2.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
      //leds3.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
    //leds.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);

      leds2.show();
    //leds2.show();
   // leds3.show();
    // Reset universeReceived to 0
   // memset(universesReceived, 0, maxUniverses);
  }

  if (universe == 3) {
    for (unsigned int i = 0; i < length / 3; i++) {
      int led = i + 1;
      //if (led < numLeds)
      leds3.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
      //leds2.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
      //leds3.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
    //leds.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);

      leds3.show();
    //leds2.show();
   // leds3.show();
    // Reset universeReceived to 0
    //memset(universesReceived, 0, maxUniverses);
  }


  // sendFrame = 1;
  // // set brightness of the whole strip
  // if (universe == 15) {
  //   leds.setBrightness(data[0]);
  //   leds.show();
  //   leds2.setBrightness(data[0]);
  //   leds2.show();
  //   leds3.setBrightness(data[0]);
  //   leds3.show();
  // }

  // // Store which universe has got in
  // if ((universe - startUniverse) < maxUniverses)
  //   universesReceived[universe - startUniverse] = 1;

  // for (int i = 0; i < maxUniverses; i++) {
  //   if (universesReceived[i] == 0) {
  //     //Serial.println("Broke");
  //     sendFrame = 0;
  //     break;
  //   }
  // }

  // // read universe and put into the right part of the display buffer
  // for (unsigned int i = 0; i < length / 3; i++) {
  //   int led = i + (universe - startUniverse) * (previousDataLength / 3);
  //   if (led < numLeds)
  //     leds.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
  //   leds2.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
  //   leds3.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
  // }
  // previousDataLength = length;

  // if (sendFrame) {
  //   leds.show();
  //   leds2.show();
  //   leds3.show();
  //   // Reset universeReceived to 0
  //   memset(universesReceived, 0, maxUniverses);
  // }
}

void initTest() {
  for (int i = 0; i < numLeds; i++) {
    leds.setPixelColor(i, 20, 0, 0);
    leds.show();
    leds2.setPixelColor(i, 20, 0, 0);
    leds2.show();
    leds3.setPixelColor(i, 20, 0, 0);
    leds3.show();
  }

  delay(500);

  for (int i = 0; i < numLeds; i++)
    leds.setPixelColor(i, 0, 127, 0);
  leds.show();
  delay(500);
  for (int i = 0; i < numLeds; i++)
    leds.setPixelColor(i, 0, 0, 127);
  leds.show();
  delay(500);
  for (int i = 0; i < numLeds; i++)
  {

    leds.setPixelColor(i, 0, 0, 0);
    leds2.setPixelColor(i, 0, 0, 0);
    leds3.setPixelColor(i, 0, 0, 0);
 
}
  leds.show();
}
