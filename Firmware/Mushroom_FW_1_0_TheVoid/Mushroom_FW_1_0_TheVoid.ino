// TEENSY 4.1 //

#include <Artnet.h>
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>
#include <OctoWS2811.h>

const int numPins = 39;
byte pinList[] = {
  23, 22, 21, 20, 19, 18, 17, 16,
  15, 14, 41, 40, 39, 38, 37, //36, //0,  //36,
  36, 35, 34, 33, //1, 2, 3, 4, 5, 6, 7, 8,
  30, 29, 28, 27,
  26, 25, 24, 12, 11, 10, 9, 8,
  7,  6,  5,  4,  3,  2,  1,  0

  
  //9, 10, 11, 12, 24, 25, 26, 27,
  //30, 35, 34, 33,
  //27, 28, 29, 36
};


// OctoWS2811 settings
const int ledsPerStrip = 100;  // change for your setup
const byte numStrips = 39;     // change for your setup
const int numLeds = ledsPerStrip * numStrips;
// 6*36 = 216
// 100 * 8 = 800

//mushroom: 26*100 = 2600

//const int numLeds = 1016;
const int numberOfChannels = numLeds * 3;  // Total number of channels you want to receive (1 led = 3 channels)
const int bytesPerLED = 3;                 // change to 4 if using RGBW
DMAMEM int displayMemory[ledsPerStrip * numPins * bytesPerLED / 4];
int drawingMemory[ledsPerStrip * numPins * bytesPerLED / 4];
const int config = WS2811_GRB | WS2811_800kHz;
OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config, numPins, pinList);
// Artnet settings
Artnet artnet;
const int startUniverse = 0;  // CHANGE FOR YOUR SETUP most software this is 1, some software send out artnet first universe as 0.

// Check if we got all universes
//const int maxUniverses = numberOfChannels / 512 + ((numberOfChannels % 512) ? 1 : 0);
const int maxUniverses = 39;
bool universesReceived[maxUniverses];
bool sendFrame = 1;
int previousDataLength = 0;

// Change ip and mac address for your setup
byte ip[] = { 10, 0, 0, 9 };
byte mac[] = { 0x04, 0xE9, 0xE5, 0x00, 0x69, 0xEC };

void setup() {
  Serial.begin(115200);

  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);

  leds.begin();
  initLED(0, 0, 25);

  artnet.begin(mac, ip);
  // this will be called for each packet received
  artnet.setArtDmxCallback(onDmxFrame);

  initLED(0, 25, 0);

  initLED(25, 0, 0);

  Serial.println("READY");
}

void loop() {
  artnet.read();
}


int led = 0;
void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data) {
  sendFrame = 1;

  // Serial.print(universe);
  // Serial.print(",");
  // for(int i = 0; i<100; i++){
  //   Serial.print(data[i]);
  //   Serial.print("/");
  // }
  // Serial.println();

  // Store which universe has got in
  if ((universe - startUniverse) < maxUniverses) {
    universesReceived[universe - startUniverse] = 1;
  }

  for (int i = 0; i < maxUniverses; i++) {
    if (universesReceived[i] == 0) {
      sendFrame = 0;
      break;
    }
  }

  led = universe * ledsPerStrip;
  for(int i = 0; i<ledsPerStrip; i++)
  {
    leds.setPixel(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    led++;
  }


  if (sendFrame) {
    leds.show();
    // Reset universeReceived to 0
    memset(universesReceived, 0, maxUniverses);
  }
}


void initLED(int r, int g, int b) {
  for (int i = 0; i < numLeds; i++) {
    leds.setPixel(i, r, g, b);
    leds.show();
  }
  delay(500);
  for (int i = 0; i < numLeds; i++)
    leds.setPixel(i, 0, 0, 0);
  leds.show();
}
