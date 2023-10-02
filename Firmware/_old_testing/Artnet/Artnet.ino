// Receive multiple universes via Artnet and control a strip of ws2811 leds via OctoWS2811
//
// This example may be copied under the terms of the MIT license, see the LICENSE file for details
//  https://github.com/natcl/Artnet
//
// http://forum.pjrc.com/threads/24688-Artnet-to-OctoWS2811?p=55589&viewfull=1#post55589

#include <Artnet.h>
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>
#include <SPI.h>
#include <OctoWS2811.h>

// Ideas for improving performance with WIZ820io / WIZ850io Ethernet:
// https://forum.pjrc.com/threads/45760-E1-31-sACN-Ethernet-DMX-Performance-help-6-Universe-Limit-improvements

const int numPins = 20;
const byte numStrips = 20; // change for your setup
byte pinList[numPins] = {
  23, 22, 21, 20, 19, 18, 17, 16,
  15, 14, 41, 40, 39, 38, 37, 36,
  0, 1, 2, 3//, 4, 5, 6, 7,
  //8, 9, 10, 11, 12, 24, 25, 26
};

// OctoWS2811 settings
const int ledsPerStrip = 100; // change for your setup
const int bytesPerLED = 3;  // change to 4 if using RGBW
DMAMEM int displayMemory[ledsPerStrip * numPins * bytesPerLED / 4];
int drawingMemory[ledsPerStrip * numPins * bytesPerLED / 4];
const int config = WS2811_GRB | WS2811_800kHz;
OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config, numPins, pinList);

// Artnet settings
Artnet artnet;
const int startUniverse = 1; // CHANGE FOR YOUR SETUP most software this is 1, some software send out artnet first universe as zero.
const int numberOfChannels = ledsPerStrip * numStrips * 3; // Total number of channels you want to receive (1 led = 3 channels)
byte channelBuffer[numberOfChannels]; // Combined universes into a single array

// Check if we got all universes
//const int maxUniverses = numberOfChannels / 512 + ((numberOfChannels % 512) ? 1 : 0);
const int maxUniverses = 20;
bool universesReceived[maxUniverses];
bool sendFrame = 1;

// Change ip and mac address for your setup
byte ip[] = {10, 0, 0, 65};
byte mac[] = {0x04, 0xE9, 0xE5, 0x00, 0x69, 0xEC};

void setup()
{
  Serial.begin(115200);
  leds.begin();
  artnet.begin(mac, ip);

  initTest();

  // this will be called for each packet received
  artnet.setArtDmxCallback(onDmxFrame);

  Serial.print("maxUniver ");
  Serial.print(maxUniverses);
}

void loop()
{
  // we call the read function inside the loop
  artnet.read();
}

//if (artnet.read() == ART_DMX)
// {
// print out our data
//    Serial.print("universe number = ");
//    Serial.print(artnet.getUniverse());
//    Serial.print("\tdata length = ");
//    Serial.print(artnet.getLength());
//    Serial.print("\tsequence n0. = ");
//    Serial.println(artnet.getSequence());
//    //    Serial.print("DMX data: ");
//    //    for (unsigned int i = 0 ; i < artnet.getLength() ; i++)
//    //    {
//    //      Serial.print(artnet.getDmxFrame()[i]);
//    //      Serial.print("  ");
//    //    }
//    //    Serial.println();
//    Serial.println();

//    if (artnet.getUniverse() == 1) {
//      for (unsigned int i = 0 ; i < artnet.getLength() ; i++) {
//        leds.setPixel(i, artnet.getDmxFrame()[(i) * 3], artnet.getDmxFrame()[(i * 3) + 1], artnet.getDmxFrame()[(i * 3) + 2]);
//      }
//    }
//
//    if (artnet.getUniverse() == 2) {
//      for (unsigned int i = 100 ; i < 200 ; i++) {
//        leds.setPixel(i, artnet.getDmxFrame()[(i) * 3], artnet.getDmxFrame()[(i * 3) + 1], artnet.getDmxFrame()[(i * 3) + 2]);
//      }
//    }
//
//    if (artnet.getUniverse() == 20)
//    {
//      leds.show();
//    }

//    if (artnet.getUniverse() == 1)
//    {
//      //Serial.print("DMX data: ");
//      for (unsigned int i = 0 ; i < artnet.getLength() ; i++)
//      {
//        leds.setPixel(i, artnet.getDmxFrame()[(i) * 3], artnet.getDmxFrame()[(i * 3) + 1], artnet.getDmxFrame()[(i * 3) + 2]);
//
//        //Serial.print(artnet.getDmxFrame()[i]);
//       // Serial.print("  ");
//      }
//      //leds.show();
//      //Serial.println();
//
//      //    for (int i = 0; i < 100; i++)
//      //      {
//      //        leds.setPixel(i, channelBuffer[(i) * 3], channelBuffer[(i * 3) + 1], channelBuffer[(i * 3) + 2]);
//      //        leds.setPixel(
//      //    //    Serial.print(i);
//      //    //    Serial.print("/");
//      //    //    Serial.print(channelBuffer[i]);
//      //    //    Serial.println();
//      //
//      //      }
//
//
//    }
//
//    if(
//
//  }

// }

void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data)
{

  //    if (universe == 1) {
  //      for (unsigned int i = 0 ; i < 100 ; i++) {
  //        leds.setPixel(i, data[i], data
  //      }
  //    }
  //
  //    if (universe == 2) {
  //      for (unsigned int i = 0 ; i < 100 ; i++) {
  //        leds.setPixel(i, artnet.getDmxFrame()[(data[i]) * 3], artnet.getDmxFrame()[(data[i]) * 3], artnet.getDmxFrame()[data[i] * 3 + 2]);
  //      }
  //    }
  //
  //    if (universe == 3) {
  //      for (unsigned int i = 200 ; i < 300 ; i++) {
  //        leds.setPixel(i, artnet.getDmxFrame()[(i) * 3], artnet.getDmxFrame()[(i * 3) + 1], artnet.getDmxFrame()[(i * 3) + 2]);
  //      }
  //    }
  //
  //    if (universe == 4) {
  //      for (unsigned int i = 300 ; i < 400 ; i++) {
  //        leds.setPixel(i, artnet.getDmxFrame()[(i) * 3], artnet.getDmxFrame()[(i * 3) + 1], artnet.getDmxFrame()[(i * 3) + 2]);
  //      }
  //    }
  //
  //    if (universe == 5) {
  //      for (unsigned int i = 400 ; i < 500 ; i++) {
  //        leds.setPixel(i, artnet.getDmxFrame()[(i) * 3], artnet.getDmxFrame()[(i * 3) + 1], artnet.getDmxFrame()[(i * 3) + 2]);
  //      }
  //    }
  //
  //    if (universe == 6) {
  //      for (unsigned int i = 500 ; i < 600 ; i++) {
  //        leds.setPixel(i, artnet.getDmxFrame()[(i) * 3], artnet.getDmxFrame()[(i * 3) + 1], artnet.getDmxFrame()[(i * 3) + 2]);
  //      }
  //    }
  //
  //    if (universe == 7) {
  //      for (unsigned int i = 600 ; i < 700 ; i++) {
  //        leds.setPixel(i, artnet.getDmxFrame()[(i) * 3], artnet.getDmxFrame()[(i * 3) + 1], artnet.getDmxFrame()[(i * 3) + 2]);
  //      }
  //    }
  //
  //    if (universe == 8) {
  //      for (unsigned int i = 700 ; i < 800 ; i++) {
  //        leds.setPixel(i, artnet.getDmxFrame()[(i) * 3], artnet.getDmxFrame()[(i * 3) + 1], artnet.getDmxFrame()[(i * 3) + 2]);
  //      }
  //    }
  //
  //    if (universe == 9) {
  //      for (unsigned int i = 800 ; i < 900 ; i++) {
  //        leds.setPixel(i, artnet.getDmxFrame()[(i) * 3], artnet.getDmxFrame()[(i * 3) + 1], artnet.getDmxFrame()[(i * 3) + 2]);
  //      }
  //    }
  //
  //    if (universe == 10) {
  //      for (unsigned int i = 900 ; i < 1000 ; i++) {
  //        leds.setPixel(i, artnet.getDmxFrame()[(i) * 3], artnet.getDmxFrame()[(i * 3) + 1], artnet.getDmxFrame()[(i * 3) + 2]);
  //      }
  //    }



  //
  //    if (universe == 20)
  //    {
  //      leds.show();
  //    }

  //}
  sendFrame = 1;

  Serial.print(universe);
  Serial.print("/");
  Serial.print(length);
  Serial.print("/");
  Serial.print(sequence);
  Serial.print("/");
  Serial.print(*data);
  Serial.println();


  // Store which universe has got in
  if (universe < maxUniverses)
  {
     universesReceived[universe] = 1;
   //  Serial.print(universe)
  }
   

  for (int i = 0 ; i < maxUniverses ; i++)
  {
    if (universesReceived[i] == 0)
    {
      //Serial.println("Broke");
      sendFrame = 0;
      break;
    }
  }

  // Serial.print(millis());
  // Serial.println(" here");

  // read universe and put into the right part of the display buffer
  for (int i = 0 ; i < 100 ; i++)
  {
    int bufferIndex = i + ((universe - startUniverse) * length);

    // Serial.println(bufferIndex);
    if (bufferIndex < numberOfChannels) {
      // to verify
      channelBuffer[bufferIndex] = byte(data[i]);

      //      Serial.print(universe);
      //      Serial.print("/");
      //      Serial.print(bufferIndex);
      //      Serial.print("/");
      //      Serial.print(length);
      //      Serial.print("/");
      //      Serial.print(channelBuffer[bufferIndex]);
      //      Serial.println();
    }

  }

  // send to leds
  for (int i = 0; i < ledsPerStrip * numStrips; i++)
  {
    leds.setPixel(i, channelBuffer[(i) * 3], channelBuffer[(i * 3) + 1], channelBuffer[(i * 3) + 2]);
    //    Serial.print(i);
    //    Serial.print("/");
    //    Serial.print(channelBuffer[i]);
    //    Serial.println();

  }

  if (sendFrame)
  {
    Serial.println("__SEND__");
    leds.show();
    // Reset universeReceived to 0
    memset(universesReceived, 0, maxUniverses);
  }

}
// }

void initTest()
{
  for (int i = 0 ; i < ledsPerStrip * numStrips ; i++)
    leds.setPixel(i, 10, 0, 0);
  leds.show();
  delay(500);
  for (int i = 0 ; i < ledsPerStrip * numStrips  ; i++)
    leds.setPixel(i, 0, 10, 0);
  leds.show();
  delay(500);
  for (int i = 0 ; i < ledsPerStrip * numStrips  ; i++)
    leds.setPixel(i, 0, 0, 10);
  leds.show();
  delay(500);
  for (int i = 0 ; i < ledsPerStrip * numStrips  ; i++)
    leds.setPixel(i, 0, 0, 0);
  leds.show();
}
