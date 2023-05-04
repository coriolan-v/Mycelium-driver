// Receive multiple universes via Artnet and control a strip of ws2811 leds via OctoWS2811
//
// This example may be copied under the terms of the MIT license, see the LICENSE file for details
//  https://github.com/natcl/Artnet
// 
// http://forum.pjrc.com/threads/24688-Artnet-to-OctoWS2811?p=55589&viewfull=1#post55589

#include <Artnet.h>
// DON'T FORGET TO CHANGE ANY REFERENCES OF Ethernet.h and EthernetUdp.h in Arnet.h to NativeEthernet.h and NativeEthernetUdp.h
//C:\Users\Coriolan\AppData\Local\Arduino15\packages\teensy\hardware\avr\1.58.1\libraries\Artnet
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>
#include <SPI.h>
#include <OctoWS2811.h>

// Ideas for improving performance with WIZ820io / WIZ850io Ethernet:
// https://forum.pjrc.com/threads/45760-E1-31-sACN-Ethernet-DMX-Performance-help-6-Universe-Limit-improvements

// OctoWS2811 settings
const int ledsPerStrip = 100; // change for your setup
const byte numStrips= 32; // change for your setup
const int numLeds = ledsPerStrip * numStrips;
const int numberOfChannels = numLeds * 3; // Total number of channels you want to receive (1 led = 3 channels)
DMAMEM int displayMemory[ledsPerStrip*6];
int drawingMemory[ledsPerStrip*6];
const int config = WS2811_GBR | WS2811_800kHz;

// byte pinList[numStrips] = {
// 23, 22, 21, 20, 19};

// byte pinList[numStrips] = {
// 23, 22, 21, 20, 19, 
// 18, 17, 16, 15, 14,
//  12, 41, 40, 39, 38,
//  37, 36, 35, 34, 33};

 byte pinList[numStrips] = {
23, 22, 21, 20, 19, 18, 17, 16,
15, 14, 41, 40, 39, 38, 37, 36,
0, 1, 2, 3, 4, 5, 6, 7, 
8, 9, 10, 11, 12, 24, 25, 26
};

 //const int numPins = 48;
// byte pinList[numStrips] = {
//  0, 1,   2,  3,  4,  5,  6,  7,  8,  9,
// 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 
// 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 
// 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 
// 40, 41, 49, 50, 51, 52, 53, 54
// };
OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config,  numStrips, pinList);

// Artnet settings
Artnet artnet;
const int startUniverse = 0; // CHANGE FOR YOUR SETUP most software this is 1, some software send out artnet first universe as zero.
//const int numberOfChannels = ledsPerStrip * numStrips * 3; // Total number of channels you want to receive (1 led = 3 channels)
//byte channelBuffer[numberOfChannels]; // Combined universes into a single array

// Check if we got all universes
// const int maxUniverses = numberOfChannels / 512 + ((numberOfChannels % 512) ? 1 : 0);
// bool universesReceived[maxUniverses];

// const int maxUniverses = numStrips;
// bool universesReceived[maxUniverses];
// bool sendFrame = 1;

const int maxUniverses = numberOfChannels / 512 + ((numberOfChannels % 512) ? 1 : 0);
bool universesReceived[maxUniverses];
bool sendFrame = 1;
int previousDataLength = 0;

// Change ip and mac address for your setup
byte ip[] = {192, 168, 1, 3};
byte mac[] = {0x04, 0xE9, 0xE5, 0x00, 0x69, 0xEC};

void setup()
{
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  Serial.begin(115200);

  leds.begin();

  initRed();
  
  artnet.begin(mac, ip);

  artnet.setArtDmxCallback(onDmxFrame);

  initGreen();

  Serial.println("START_____");
  
  

  // this will be called for each packet received
  
}

void loop()
{
  // we call the read function inside the loop
  artnet.read();
}

void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data)
{
   sendFrame = 1;

  // Store which universe has got in
  if ((universe - startUniverse) < maxUniverses)
    universesReceived[universe - startUniverse] = 1;

  for (int i = 0 ; i < maxUniverses ; i++)
  {
    if (universesReceived[i] == 0)
    {
      sendFrame = 0;
      break;
    }
  }

  // read universe and put into the right part of the display buffer
  for (int i = 0; i < length / 3; i++)
  {
    int led = i + (universe - startUniverse) * (previousDataLength / 3);
    if (led < numLeds)
      leds.setPixel(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
  }
  previousDataLength = length;      
  
  if (sendFrame)
  {
    leds.show();
    // Reset universeReceived to 0
    memset(universesReceived, 0, maxUniverses);
  }
  // sendFrame = 1;
  // Serial.print(universe);
  // Serial.print("/");
  // Serial.print(length);
  // Serial.print("/");
  // Serial.print(sequence);
  // Serial.print("/");
  // Serial.print(*data);
  // Serial.println();

  // // Store which universe has got in
  // //if (universe < maxUniverses)
  // //  universesReceived[universe] = 1;

  // // for (int i = 0 ; i < maxUniverses ; i++)
  // // {
  // //   if (universesReceived[i] == 0)
  // //   {
  // //     //Serial.println("Broke");
  // //     sendFrame = 0;
  // //     break;
  // //   }
  // // }

  // // read universe and put into the right part of the display buffer
  // for (int i = 0 ; i < length ; i++)
  // {
  //   int bufferIndex = i + ((universe - startUniverse) * length);
  //   if (bufferIndex < numberOfChannels) // to verify
  //     channelBuffer[bufferIndex] = byte(data[i]);
  // }      

  // // send to leds
  // for (int i = 0; i < ledsPerStrip * numStrips; i++)
  // {
  //   leds.setPixel(i, channelBuffer[(i) * 3], channelBuffer[(i * 3) + 1], channelBuffer[(i * 3) + 2]);
  // }      
  
  // if (sendFrame)
  // {
  //   leds.show();
  //   // Reset universeReceived to 0
  //   memset(universesReceived, 0, maxUniverses);
  // }
}

void initRed()
{
  for (int i = 0 ; i < ledsPerStrip * numStrips ; i++)
    leds.setPixel(i, 50, 0, 0);
  leds.show();
  delay(500);

   for (int i = 0 ; i < ledsPerStrip * numStrips  ; i++)
    leds.setPixel(i, 0, 0, 0);
  leds.show();
   delay(500);
}

void initGreen()
{
  for (int i = 0 ; i < ledsPerStrip * numStrips  ; i++)
    leds.setPixel(i, 0, 50, 0);
  leds.show();
  delay(500);
   for (int i = 0 ; i < ledsPerStrip * numStrips  ; i++)
    leds.setPixel(i, 0, 0, 0);
  leds.show();
   delay(500);
}

void initTest()
{
  for (int i = 0 ; i < ledsPerStrip * numStrips ; i++)
    leds.setPixel(i, 50, 0, 0);
  leds.show();
  delay(500);
  for (int i = 0 ; i < ledsPerStrip * numStrips  ; i++)
    leds.setPixel(i, 0, 50, 0);
  leds.show();
  delay(500);
  for (int i = 0 ; i < ledsPerStrip * numStrips  ; i++)
    leds.setPixel(i, 0, 0, 50);
  leds.show();
  delay(500);
  for (int i = 0 ; i < ledsPerStrip * numStrips  ; i++)
    leds.setPixel(i, 0, 0, 0);
  leds.show();
   delay(500);
 
}
