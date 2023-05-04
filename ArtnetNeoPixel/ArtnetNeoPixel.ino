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
  15, 14, 41, 40, 39, 38, 37, 36,
  0, 1, 2, 3, 4, 5, 6, 7,
  8//, 9, 10, 11, 12, 24, 25, 26
};

Adafruit_NeoPixel leds = Adafruit_NeoPixel(numLeds, dataPins[0], NEO_GRB);
Adafruit_NeoPixel leds2 = Adafruit_NeoPixel(numLeds, dataPins[1], NEO_GRB);
Adafruit_NeoPixel leds3 = Adafruit_NeoPixel(numLeds, dataPins[2], NEO_GRB);
Adafruit_NeoPixel leds4 = Adafruit_NeoPixel(numLeds, dataPins[3], NEO_GRB);
Adafruit_NeoPixel leds5 = Adafruit_NeoPixel(numLeds, dataPins[4], NEO_GRB);
Adafruit_NeoPixel leds6 = Adafruit_NeoPixel(numLeds, dataPins[5], NEO_GRB);
Adafruit_NeoPixel leds7 = Adafruit_NeoPixel(numLeds, dataPins[6], NEO_GRB);
Adafruit_NeoPixel leds8 = Adafruit_NeoPixel(numLeds, dataPins[7], NEO_GRB);

Adafruit_NeoPixel leds9 = Adafruit_NeoPixel(numLeds, dataPins[8], NEO_GRB);
Adafruit_NeoPixel leds10 = Adafruit_NeoPixel(numLeds, dataPins[9], NEO_GRB);
Adafruit_NeoPixel leds11 = Adafruit_NeoPixel(numLeds, dataPins[10], NEO_GRB);
Adafruit_NeoPixel leds12 = Adafruit_NeoPixel(numLeds, dataPins[11], NEO_GRB);
Adafruit_NeoPixel leds13 = Adafruit_NeoPixel(numLeds, dataPins[12], NEO_GRB);
Adafruit_NeoPixel leds14 = Adafruit_NeoPixel(numLeds, dataPins[13], NEO_GRB);
Adafruit_NeoPixel leds15 = Adafruit_NeoPixel(numLeds, dataPins[14], NEO_GRB);
Adafruit_NeoPixel leds16 = Adafruit_NeoPixel(numLeds, dataPins[15], NEO_GRB);

Adafruit_NeoPixel leds17 = Adafruit_NeoPixel(numLeds, dataPins[16], NEO_GRB);
Adafruit_NeoPixel leds18 = Adafruit_NeoPixel(numLeds, dataPins[17], NEO_GRB);
Adafruit_NeoPixel leds19 = Adafruit_NeoPixel(numLeds, dataPins[18], NEO_GRB);
Adafruit_NeoPixel leds20 = Adafruit_NeoPixel(numLeds, dataPins[19], NEO_GRB);
Adafruit_NeoPixel leds21 = Adafruit_NeoPixel(numLeds, dataPins[20], NEO_GRB);
Adafruit_NeoPixel leds22 = Adafruit_NeoPixel(numLeds, dataPins[21], NEO_GRB);
Adafruit_NeoPixel leds23 = Adafruit_NeoPixel(numLeds, dataPins[22], NEO_GRB);
Adafruit_NeoPixel leds24 = Adafruit_NeoPixel(numLeds, dataPins[23], NEO_GRB);

Adafruit_NeoPixel leds25 = Adafruit_NeoPixel(numLeds, dataPins[24], NEO_GRB);




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

int maxUniverse = 4;

void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data) {

   Serial.print(universe);
  Serial.print("/");
  Serial.print(length);
  Serial.print("/");
  Serial.print(sequence);
  Serial.print("/");
  Serial.print(*data);
  Serial.println();

    length = 300;

  if (universe == 1) {
    for (unsigned int i = 0; i < length / 3; i++) {
      int led = i + 1;
      leds.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
        Serial.print(i);
        Serial.print(":");
      Serial.print(data[i]);
       Serial.print("-");
      Serial.print(data[i * 3 + 1]);
       Serial.print("-");
      Serial.print(data[i * 3 + 2]);
      Serial.print("/");
    }
    Serial.println();
     leds.show();
  }

   if (universe == 2) {
    for (unsigned int i = 0; i < length / 3; i++) {
      int led = i + 1;
      leds2.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
      leds2.show();
  }

   if (universe == 3) {
    for (unsigned int i = 0; i < length / 3; i++) {
      int led = i + 1;
      leds3.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
     leds3.show();
  }

   if (universe == 4) {
    for (unsigned int i = 0; i < length / 3; i++) {
      int led = i + 1;
      leds4.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
      leds4.show();
  }

   if (universe == 5) {
    for (unsigned int i = 0; i < length / 3; i++) {
      int led = i + 1;
      leds5.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
      leds5.show();
  }

   if (universe == 6) {
    for (unsigned int i = 0; i < length / 3; i++) {
      int led = i + 1;
      leds6.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
      leds6.show();
  }

   if (universe == 7) {
    for (unsigned int i = 0; i < length / 3; i++) {
      int led = i + 1;
      leds7.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
      leds7.show();
  }

   if (universe == 8) {
    for (unsigned int i = 0; i < length / 3; i++) {
      int led = i + 1;
      leds8.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
      leds8.show();
  }

   if (universe == 9) {
    for (unsigned int i = 0; i < length / 3; i++) {
      int led = i + 1;
      leds9.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
      leds9.show();
  }

   if (universe == 10) {
    for (unsigned int i = 0; i < length / 3; i++) {
      int led = i + 1;
      leds10.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
      leds10.show();
  }

   if (universe == 511) {
    for (unsigned int i = 0; i < length / 3; i++) {
      int led = i + 1;
      leds11.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
      leds11.show();
  }

   if (universe == 12) {
    for (unsigned int i = 0; i < length / 3; i++) {
      int led = i + 1;
      leds12.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
      leds12.show();
  }

   if (universe == 13) {
    for (unsigned int i = 0; i < length / 3; i++) {
      int led = i + 1;
      leds13.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
      leds13.show();
  }

   if (universe == 14) {
    for (unsigned int i = 0; i < length / 3; i++) {
      int led = i + 1;
      leds14.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
      leds14.show();
  }

   if (universe == 15) {
    for (unsigned int i = 0; i < length / 3; i++) {
      int led = i + 1;
      leds15.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
      leds15.show();
  }

   if (universe == 16) {
    for (unsigned int i = 0; i < length / 3; i++) {
      int led = i + 1;
      leds16.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
      leds16.show();
  }

   if (universe == 17) {
    for (unsigned int i = 0; i < length / 3; i++) {
      int led = i + 1;
      leds17.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
      leds17.show();
  }

   if (universe == 18) {
    for (unsigned int i = 0; i < length / 3; i++) {
      int led = i + 1;
      leds18.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
      leds18.show();
  }

   if (universe == 19) {
    for (unsigned int i = 0; i < length / 3; i++) {
      int led = i + 1;
      leds19.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
      leds19.show();
  }

   if (universe == 20) {
    for (unsigned int i = 0; i < length / 3; i++) {
      int led = i + 1;
      leds20.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
      leds20.show();
  }

   if (universe == 21) {
    for (unsigned int i = 0; i < length / 3; i++) {
      int led = i + 1;
      leds21.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
      leds21.show();
  }

   if (universe == 22) {
    for (unsigned int i = 0; i < length / 3; i++) {
      int led = i + 1;
      leds22.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
      leds22.show();
  }

   if (universe == 23) {
    for (unsigned int i = 0; i < length / 3; i++) {
      int led = i + 1;
      leds23.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
      leds23.show();
  }

   if (universe == 24) {
    for (unsigned int i = 0; i < length / 3; i++) {
      int led = i + 1;
      leds24.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
      leds24.show();
  }

   if (universe == 25) {
    for (unsigned int i = 0; i < length / 3; i++) {
      int led = i + 1;
      leds25.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
      leds25.show();
  }

//   if(universe == maxUniverse)
//   {
//  leds.show();
//   leds2.show();
//    leds3.show();
//     leds4.show();
//      leds5.show();
//   }




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
    leds4.setPixelColor(i, 0, 0, 0);
    leds5.setPixelColor(i, 0, 0, 0);
    leds6.setPixelColor(i, 0, 0, 0);
    leds7.setPixelColor(i, 0, 0, 0);
    leds8.setPixelColor(i, 0, 0, 0);
    leds9.setPixelColor(i, 0, 0, 0);
    leds10.setPixelColor(i, 0, 0, 0);
    leds11.setPixelColor(i, 0, 0, 0);
    leds12.setPixelColor(i, 0, 0, 0);
    leds13.setPixelColor(i, 0, 0, 0);
    leds14.setPixelColor(i, 0, 0, 0);
    leds15.setPixelColor(i, 0, 0, 0);
    leds16.setPixelColor(i, 0, 0, 0);
    leds17.setPixelColor(i, 0, 0, 0);
    leds18.setPixelColor(i, 0, 0, 0);
    leds19.setPixelColor(i, 0, 0, 0);
    leds20.setPixelColor(i, 0, 0, 0);
    leds21.setPixelColor(i, 0, 0, 0);
       leds22.setPixelColor(i, 0, 0, 0);
          leds23.setPixelColor(i, 0, 0, 0);
             leds23.setPixelColor(i, 0, 0, 0);
    leds25.setPixelColor(i, 0, 0, 0);
}
  leds.show();
}
