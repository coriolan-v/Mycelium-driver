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
const int numLeds = 240; // change for your setup
const int channelsPerLed = 3;
const int numberOfChannels = numLeds * channelsPerLed; // Total number of channels you want to receive (1 led = 3 channels)
const byte dataPin = 23;

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
const int startUniverse = 0; // CHANGE FOR YOUR SETUP most software this is 1, some software send out artnet first universe as 0.
int previousDataLength = 0;

// Change ip and mac address for your setup
byte ip[] = {10, 0, 0, 199};
byte mac[] = {0x04, 0xE9, 0xE5, 0x00, 0x69, 0xEC};
byte broadcast[] = {10, 0, 0, 255};
void setup()
{
  Serial.begin(115200);
  artnet.begin(mac, ip);
  //leds.begin();
  beginLEDs();
  artnet.setBroadcast(broadcast);
  initTest();

  // this will be called for each packet received
  artnet.setArtDmxCallback(onDmxFrame);
  artnet.setArtSyncCallback(onSync);
}

void loop()
{
  // we call the read function inside the loop
  artnet.read();
}

void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data, IPAddress remoteIP)
{
  // read universe and put into the right part of the display buffer
  for (int i = 0; i < length / channelsPerLed; i++)
  {
    int led = i + (universe - startUniverse) * (previousDataLength / channelsPerLed);
    if (led < numLeds) {
      if (channelsPerLed == 4)
        leds.setPixelColor(led, data[i * channelsPerLed], data[i * channelsPerLed + 1], data[i * channelsPerLed + 2], data[i * channelsPerLed + 3]);
      if (channelsPerLed == 3)
        leds.setPixelColor(led, data[i * channelsPerLed], data[i * channelsPerLed + 1], data[i * channelsPerLed + 2]);
    }
  }
  previousDataLength = length;
}

void onSync(IPAddress remoteIP) {
  //leds.show();

  showLEDs();
}

void initTest()
{
  for (int i = 0 ; i < numLeds ; i++)
    leds.setPixelColor(i, 127, 0, 0);
  leds.show();
  delay(500);
  for (int i = 0 ; i < numLeds ; i++)
    leds.setPixelColor(i, 0, 127, 0);
  leds.show();
  delay(500);
  for (int i = 0 ; i < numLeds ; i++)
    leds.setPixelColor(i, 0, 0, 127);
  leds.show();
  delay(500);
  for (int i = 0 ; i < numLeds ; i++)
    leds.setPixelColor(i, 0, 0, 0);
  leds.show();
}
