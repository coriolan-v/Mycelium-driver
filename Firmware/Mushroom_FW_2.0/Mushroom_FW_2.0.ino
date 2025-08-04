#include <Artnet.h>
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>
#include <OctoWS2811.h>

// Configuration
const int ledsPerStrip = 720;
const int numPins = 2;
const int numLeds = ledsPerStrip * numPins;
const int bytesPerLED = 3;
const int totalChannels = numLeds * bytesPerLED;  // 4320 bytes total
const int startUniverse = 0;
const int channelsPerUniverse = 510;  // ENTTEC LED Mapper sends 510
const int maxUniverses = (totalChannels + channelsPerUniverse - 1) / channelsPerUniverse;  // 9 universes

DMAMEM int displayMemory[numLeds * bytesPerLED / 4];
int drawingMemory[numLeds * bytesPerLED / 4];
byte pinList[numPins] = {23, 22};  // <- use pins 23 and 22

const int config = WS2811_GRB | WS2811_800kHz;
OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config, numPins, pinList);

// Artnet setup
Artnet artnet;
byte ip[] = {10, 0, 0, 9};
byte mac[] = {0x04, 0xE9, 0xE5, 0x00, 0x69, 0xEC};

uint8_t ledData[totalChannels];
bool universesReceived[maxUniverses];

void setup() {
  Serial.begin(115200);
  leds.begin();
  leds.show();

  initLED(0, 0, 25);  // blue test
  artnet.begin(mac, ip);
  artnet.setArtDmxCallback(onDmxFrame);
  initLED(0, 25, 0);  // green test

  Serial.println("ArtNet Receiver Ready");
}

void loop() {
  artnet.read();
}

void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data) {
  Serial.println("Frame received");

  uint16_t u = universe - startUniverse;
  if (u >= maxUniverses) return;

  uint16_t offset = u * channelsPerUniverse;

  Serial.print("Universe ");
  Serial.print(universe);
  Serial.print(" | Length: ");
  Serial.print(length);
  Serial.print(" | Offset: ");
  Serial.print(offset);
  Serial.print(" | First bytes: ");
  for (int i = 0; i < 6 && i < length; i++) {
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  uint16_t copyLen = min(length, totalChannels - offset);
  memcpy(&ledData[offset], data, copyLen);
  universesReceived[u] = true;

  // Wait until all universes received
  for (int i = 0; i < maxUniverses; i++) {
    if (!universesReceived[i]) return;
  }

  // Write pixel data to all strips
  for (int i = 0; i < ledsPerStrip * numPins; i++) {
    int base = i * 3;
    int r = ledData[base + 0];
    int g = ledData[base + 1];
    int b = ledData[base + 2];
    leds.setPixel(i, r, g, b);
  }

  leds.show();
  memset(universesReceived, 0, sizeof(universesReceived));
}

void initLED(int r, int g, int b) {
  for (int i = 0; i < numLeds; i++) {
    leds.setPixel(i, r, g, b);
    delay(1);
  }
  leds.show();
  delay(500);
  for (int i = 0; i < numLeds; i++) {
    leds.setPixel(i, 0, 0, 0);
     delay(1);
  }
  leds.show();
}
