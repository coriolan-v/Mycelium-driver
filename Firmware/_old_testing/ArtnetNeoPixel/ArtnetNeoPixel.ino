#include <Artnet.h>
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>
#include <SPI.h>
#include <Adafruit_NeoPixel.h>

int maxSyncUniverse = 20;

// Neopixel settings
const int numLeds = 100;                   // change for your setup
// const int numberOfChannels = numLeds * 3;  // Total number of channels you want to receive (1 led = 3 channels)
// const byte dataPin = 21;


byte dataPins[] = {
  23, 22, 21, 20, 19, 18, 17, 16,
  15, 14, 41, 40, 39, 38, 37, 36,
  0, 1, 2, 3, 4, 5, 6, 7,
  8, 9, 10, 11, 12, 24, 25, 26
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
Adafruit_NeoPixel leds26 = Adafruit_NeoPixel(numLeds, dataPins[25], NEO_GRB);
Adafruit_NeoPixel leds27 = Adafruit_NeoPixel(numLeds, dataPins[26], NEO_GRB);
Adafruit_NeoPixel leds28 = Adafruit_NeoPixel(numLeds, dataPins[27], NEO_GRB);
Adafruit_NeoPixel leds29 = Adafruit_NeoPixel(numLeds, dataPins[28], NEO_GRB);
Adafruit_NeoPixel leds30 = Adafruit_NeoPixel(numLeds, dataPins[29], NEO_GRB);
Adafruit_NeoPixel leds31 = Adafruit_NeoPixel(numLeds, dataPins[30], NEO_GRB);
Adafruit_NeoPixel leds32 = Adafruit_NeoPixel(numLeds, dataPins[31], NEO_GRB);

// Artnet settings
Artnet artnet;
//const int startUniverse = 1;  // CHANGE FOR YOUR SETUP most software this is 1, some software send out artnet first universe as 0.

// Check if we got all universes
// const int maxUniverses = numberOfChannels / 512 + ((numberOfChannels % 512) ? 1 : 0);
// bool universesReceived[maxUniverses];
// bool sendFrame = 1;
// int previousDataLength = 0;

// Change ip and mac address for your setup
byte ip[] = { 10, 0, 0, 41 };
byte mac[] = { 0x04, 0xE9, 0xE5, 0x00, 0x69, 0xEC };

void setup() {

  Serial.begin(115200);


  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);

  Serial.println("Testing LEDs...");

  beginLEDs();

  //allLEDsOn(0, 0, 25);
  //allLEDsOn(0, 0, 0);


  //Serial.begin(115200);
  artnet.begin(mac, ip);
  // this will be called for each packet received
  artnet.setArtDmxCallback(onDmxFrame);
  artnet.setArtSyncCallback(onSync);

  Serial.println("Artnet configured! Testing LEDs...");

  //allLEDsOn(0, 25, 0);
  //allLEDsOn(0, 0, 0);

  Serial.println("Ready!");
}

void onSync(IPAddress remoteIP) {
    //leds.show();
    showLEDs();
}

void loop() {
  // we call the read function inside the loop
  artnet.read();
}

const int startUniverse = 1;  // CHANGE FOR YOUR SETUP most software this is 1, some software send out artnet first universe as 0.
const int maxUniverses = 20;
//int maxUniverse = 4;
bool sendFrame = 1;
bool universesReceived[maxUniverses];

unsigned long prev = 0;
int fps = 0;

int checkSum = 0;

bool individualSync = false;

void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data) {

//  Serial.print(universe);
//  Serial.print("/");
//  Serial.print(length);
//  Serial.print("/");
//  Serial.print(sequence);
//  Serial.print("/");
//  Serial.print(*data);
//  Serial.println();

  // sendFrame = 1;
  //
  //  // Serial.print(
  //
  //    //Serial.print("stored: ");
  //   // Serial.println(universe);
  //
  //  // Store which universe has got in
//  if ((universe - startUniverse) < maxUniverses) {
//
//    if (universesReceived[universe - startUniverse] == 0)
//    {
//      universesReceived[universe - startUniverse] = 1;
//
//      checkSum++;
//
//      Serial.print("stored FIRST TIME: ");
//      Serial.print(universe);
//      Serial.print(" checksum: ");
//      Serial.println(checkSum);
//    }
//
//     Serial.print("/ stored: ");
//     Serial.println(universe);
//  }

//  if (universe == maxUniverses)
//    {
//      sendFrame = 1;
//      Serial.print(millis());
//      Serial.println(" SEND______");
//
//      checkSum = 0;
//
//      for (int i = 0; i < maxUniverses; i++) {
//        universesReceived[i] = 0;
//      }
//    }


    
  //
  //

//  for (int i = 0; i < maxUniverses; i++) {
//
//    if (universesReceived[i] == 1) {
//      checkSum++;
//      Serial.print(i);
//    }
//
//    
//  }


//  for (int i = 0; i < maxUniverses; i++) {
//    if (universesReceived[i] == 0) {
//      sendFrame = 0;
//      //Serial.print(i);
//      //Serial.print(": ");
//      //Serial.println(("0"));
//      break;
//    }
//  }
//  //
//  //
//  //
//  //  length = 300;
//  //
//  if (sendFrame) {
//
//     if(!individualSync) showLEDs();
//  }



  //    //leds.show();
  //    // Reset universeReceived to 0
  //    // memset(universesReceived, 0, maxUniverses);
  //
  //    static unsigned long lastMillis;
  //    static unsigned long frameCount;
  //    static unsigned int framesPerSecond;
  //
  //    //Serial.print(millis());
  //    //Serial.print(" SEND ");
  //    //Serial.println(frameCount);
  //    // It is best if we declare millis() only once
  //    unsigned long now = millis();
  //    frameCount ++;
  //    if (now - lastMillis >= 1 * 1000) {
  //      framesPerSecond = frameCount / 1;
  //      Serial.print(millis()); Serial.print("ms uptime, FPS: "); Serial.println(framesPerSecond);
  //      frameCount = 0;
  //      lastMillis = now;
  //    }
  //
  //    //    if(millis() - prev >= 100)
  //    //    {
  //    //      prev = millis();
  //    //      Serial.print(millis());
  //    //      Serial.println(" SEND ");
  //    //      Serial.println(fps);
  //    //      fps = 0;
  //    //    } else {
  //    //      fps++;
  //    //    }
  //
  //    showLEDs();
  //  }

  if (universe == 1) {
    for (unsigned int i = 0; i < length / 3; i++) {
     // int led = i + 1;
      leds.setPixelColor(i + 1, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
    if(individualSync) leds.show();
  }

  if (universe == 2) {
    for (unsigned int i = 0; i < length / 3; i++) {
      //int led = i + 1;
      leds2.setPixelColor(i + 1, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
    if(individualSync) leds2.show();
  }

  if (universe == 3) {
    for (unsigned int i = 0; i < length / 3; i++) {
     // int led = i + 1;
      leds3.setPixelColor(i + 1, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
    if(individualSync) leds3.show();
  }

  if (universe == 4) {
    for (unsigned int i = 0; i < length / 3; i++) {
      //int led = i + 1;
      leds4.setPixelColor(i + 1, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
    if(individualSync) leds4.show();
  }

  if (universe == 5) {
    for (unsigned int i = 0; i < length / 3; i++) {
      //int led = i + 1;
      leds5.setPixelColor(i + 1, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
    if(individualSync) leds5.show();
  }

  if (universe == 6) {
    for (unsigned int i = 0; i < length / 3; i++) {
      //int led = i + 1;
      leds6.setPixelColor(i + 1, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
    if(individualSync) leds6.show();
  }

  if (universe == 7) {
    for (unsigned int i = 0; i < length / 3; i++) {
      //int led = i + 1;
      leds7.setPixelColor(i + 1, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
    if(individualSync) leds7.show();
  }

  if (universe == 8) {
    for (unsigned int i = 0; i < length / 3; i++) {
     // int led = i + 1;
      leds8.setPixelColor(i + 1, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
    if(individualSync) leds8.show();
  }

  if (universe == 9) {
    for (unsigned int i = 0; i < length / 3; i++) {
      //int led = i + 1;
      leds9.setPixelColor(i + 1, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
    if(individualSync) leds9.show();
  }

  if (universe == 10) {
    for (unsigned int i = 0; i < length / 3; i++) {
      //int led = i + 1;
      leds10.setPixelColor(i + 1, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
    if(individualSync) leds10.show();
  }

  if (universe == 11) {
    for (unsigned int i = 0; i < length / 3; i++) {
      //int led = i + 1;
      leds11.setPixelColor(i + 1, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
    if(individualSync) leds11.show();
  }

  if (universe == 12) {
    for (unsigned int i = 0; i < length / 3; i++) {
     // int led = i + 1;
      leds12.setPixelColor(i + 1, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
    if(individualSync) leds12.show();
  }

  if (universe == 13) {
    for (unsigned int i = 0; i < length / 3; i++) {
     // int led = i + 1;
      leds13.setPixelColor(i + 1, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
    if(individualSync) leds13.show();
  }

  if (universe == 14) {
    for (unsigned int i = 0; i < length / 3; i++) {
     // int led = i + 1;
      leds14.setPixelColor(i + 1, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
    if(individualSync) leds14.show();
  }

  if (universe == 15) {
    for (unsigned int i = 0; i < length / 3; i++) {
     // int led = i + 1;
      leds15.setPixelColor(i + 1, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
    if(individualSync) leds15.show();
  }

  if (universe == 16) {
    for (unsigned int i = 0; i < length / 3; i++) {
     // int led = i + 1;
      leds16.setPixelColor(i + 1, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
    if(individualSync) leds16.show();
  }

  if (universe == 17) {
    for (unsigned int i = 0; i < length / 3; i++) {
      //int led = i + 1;
      leds17.setPixelColor(i + 1, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
    if(individualSync) leds17.show();
  }

  if (universe == 18) {
    for (unsigned int i = 0; i < length / 3; i++) {
     // int led = i + 1;
      leds18.setPixelColor(i + 1, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
    if(individualSync) leds18.show();
  }

  if (universe == 19) {
    for (unsigned int i = 0; i < length / 3; i++) {
     // int led = i + 1;
      leds19.setPixelColor(i + 1, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
    if(individualSync) leds19.show();
  }

  if (universe == 20) {
    for (unsigned int i = 0; i < length / 3; i++) {
     // int led = i + 1;
      leds20.setPixelColor(i + 1, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
    if(individualSync) leds20.show();
  }

  if (universe == 21) {
    for (unsigned int i = 0; i < length / 3; i++) {
    //  int led = i + 1;
      leds21.setPixelColor(i + 1, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
    if(individualSync) leds21.show();
  }

  if (universe == 22) {
    for (unsigned int i = 0; i < length / 3; i++) {
      //int led = i + 1;
      leds22.setPixelColor(i + 1, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
    if(individualSync) leds22.show();
  }

  if (universe == 23) {
    for (unsigned int i = 0; i < length / 3; i++) {
     // int led = i + 1;
      leds23.setPixelColor(i + 1, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
    if(individualSync) leds23.show();
  }

  if (universe == 24) {
    for (unsigned int i = 0; i < length / 3; i++) {
    //  int led = i + 1;
      leds24.setPixelColor(i + 1, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
    if(individualSync) leds24.show();
  }

  if (universe == 25) {
    for (unsigned int i = 0; i < length / 3; i++) {
    //  int led = i + 1;
      leds25.setPixelColor(i + 1, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
    if(individualSync) leds25.show();
  }

  //  if (universe == 26) {
  //    for (unsigned int i = 0; i < length / 3; i++) {
  //      int led = i + 1;
  //      leds26.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
  //    }
  //    //leds25.show();
  //  }
  //
  //  if (universe == 27) {
  //    for (unsigned int i = 0; i < length / 3; i++) {
  //      int led = i + 1;
  //      leds27.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
  //    }
  //    //leds25.show();
  //  }
  //
  //  if (universe == 28) {
  //    for (unsigned int i = 0; i < length / 3; i++) {
  //      int led = i + 1;
  //      leds28.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
  //    }
  //    //leds25.show();
  //  }
  //
  //  if (universe == 29) {
  //    for (unsigned int i = 0; i < length / 3; i++) {
  //      int led = i + 1;
  //      leds29.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
  //    }
  //    //leds25.show();
  //  }
  //
  //  if (universe == 30) {
  //    for (unsigned int i = 0; i < length / 3; i++) {
  //      int led = i + 1;
  //      leds30.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
  //    }
  //    //leds25.show();
  //  }
  //
  //  if (universe == 31) {
  //    for (unsigned int i = 0; i < length / 3; i++) {
  //      int led = i + 1;
  //      leds31.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
  //    }
  //    //leds25.show();
  //  }
  //
  //  if (universe == 32) {
  //    for (unsigned int i = 0; i < length / 3; i++) {
  //      int led = i + 1;
  //      leds32.setPixelColor(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
  //    }
  //leds25.show();
  // }
  //

  //    if (universe == maxSyncUniverse)
  //    {
  //      showLEDs();
  //    }

}

// void initTest() {
//   for (int i = 0; i < numLeds; i++) {
//     leds.setPixelColor(i, 20, 0, 0);
//     leds.show();
//     leds2.setPixelColor(i, 20, 0, 0);
//     leds2.show();
//     leds3.setPixelColor(i, 20, 0, 0);
//     leds3.show();
//   }

//   delay(500);

//   for (int i = 0; i < numLeds; i++)
//     leds.setPixelColor(i, 0, 127, 0);
//   leds.show();
//   delay(500);
//   for (int i = 0; i < numLeds; i++)
//     leds.setPixelColor(i, 0, 0, 127);
//   leds.show();
//   delay(500);
//   for (int i = 0; i < numLeds; i++) {

//     leds.setPixelColor(i, 0, 0, 0);
//     leds2.setPixelColor(i, 0, 0, 0);
//     leds3.setPixelColor(i, 0, 0, 0);
//     leds4.setPixelColor(i, 0, 0, 0);
//     leds5.setPixelColor(i, 0, 0, 0);
//     leds6.setPixelColor(i, 0, 0, 0);
//     leds7.setPixelColor(i, 0, 0, 0);
//     leds8.setPixelColor(i, 0, 0, 0);
//     leds9.setPixelColor(i, 0, 0, 0);
//     leds10.setPixelColor(i, 0, 0, 0);
//     leds11.setPixelColor(i, 0, 0, 0);
//     leds12.setPixelColor(i, 0, 0, 0);
//     leds13.setPixelColor(i, 0, 0, 0);
//     leds14.setPixelColor(i, 0, 0, 0);
//     leds15.setPixelColor(i, 0, 0, 0);
//     leds16.setPixelColor(i, 0, 0, 0);
//     leds17.setPixelColor(i, 0, 0, 0);
//     leds18.setPixelColor(i, 0, 0, 0);
//     leds19.setPixelColor(i, 0, 0, 0);
//     leds20.setPixelColor(i, 0, 0, 0);
//     leds21.setPixelColor(i, 0, 0, 0);
//     leds22.setPixelColor(i, 0, 0, 0);
//     leds23.setPixelColor(i, 0, 0, 0);
//     leds23.setPixelColor(i, 0, 0, 0);
//     leds25.setPixelColor(i, 0, 0, 0);
//   }
//   leds.show();
// }

void allLEDsOn(int redVal, int greenVal, int blueVal) {
  for (int i = 0; i < numLeds; i++) {
    leds.setPixelColor(i, redVal, greenVal, blueVal);
    leds2.setPixelColor(i, redVal, greenVal, blueVal);
    leds3.setPixelColor(i, redVal, greenVal, blueVal);
    leds4.setPixelColor(i, redVal, greenVal, blueVal);
    leds5.setPixelColor(i, redVal, greenVal, blueVal);
    leds6.setPixelColor(i, redVal, greenVal, blueVal);
    leds7.setPixelColor(i, redVal, greenVal, blueVal);
    leds8.setPixelColor(i, redVal, greenVal, blueVal);
    leds9.setPixelColor(i, redVal, greenVal, blueVal);
    leds10.setPixelColor(i, redVal, greenVal, blueVal);
    leds11.setPixelColor(i, redVal, greenVal, blueVal);
    leds12.setPixelColor(i, redVal, greenVal, blueVal);
    leds13.setPixelColor(i, redVal, greenVal, blueVal);
    leds14.setPixelColor(i, redVal, greenVal, blueVal);
    leds15.setPixelColor(i, redVal, greenVal, blueVal);
    leds16.setPixelColor(i, redVal, greenVal, blueVal);
    leds17.setPixelColor(i, redVal, greenVal, blueVal);
    leds18.setPixelColor(i, redVal, greenVal, blueVal);
    leds19.setPixelColor(i, redVal, greenVal, blueVal);
    leds20.setPixelColor(i, redVal, greenVal, blueVal);
    leds21.setPixelColor(i, redVal, greenVal, blueVal);
    leds22.setPixelColor(i, redVal, greenVal, blueVal);
    leds23.setPixelColor(i, redVal, greenVal, blueVal);
    leds23.setPixelColor(i, redVal, greenVal, blueVal);
    leds25.setPixelColor(i, redVal, greenVal, blueVal);
    leds26.setPixelColor(i, redVal, greenVal, blueVal);
    leds27.setPixelColor(i, redVal, greenVal, blueVal);
    leds28.setPixelColor(i, redVal, greenVal, blueVal);
    leds29.setPixelColor(i, redVal, greenVal, blueVal);
    leds30.setPixelColor(i, redVal, greenVal, blueVal);
    leds31.setPixelColor(i, redVal, greenVal, blueVal);
    leds32.setPixelColor(i, redVal, greenVal, blueVal);

    showLEDs();
  }
}
