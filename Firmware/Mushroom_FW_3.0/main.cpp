// Mushroom firmware v3.0 - Teensy 4.1, ArtSync and completed-output FPS.
#include <Arduino.h>
#include <cstdio>
#include <OctoWS2811.h>
#include <QNEthernet.h>
#include <lwip/pbuf.h>
#include <lwip/udp.h>

#include "config.h"
#include "fps.h"
#include "receiver.h"

namespace {
using namespace mushroom;
namespace qn = qindesign::network;

// The ISR reads displayMemory; receive callbacks write only drawingMemory.
// 17,280 bytes per buffer by default: 16 active ports of 360 RGB LEDs.
DMAMEM uint8_t displayMemory[kFrameBytes] __attribute__((aligned(32)));
alignas(32) uint8_t drawingMemory[kFrameBytes];
OctoWS2811 leds(kLedsPerPin, displayMemory, drawingMemory,
               WS2811_RGB | WS2811_800kHz, kActivePorts, kPins);
Receiver receiver(drawingMemory);
FpsMeter fps;
volatile uint32_t completedFrames = 0;
bool frameInFlight = false;
udp_pcb* artnetSocket = nullptr;
bool ethernetStarted = false;
bool clearOnStartup = true;
uint32_t lastStartAttemptMs = 0;

IPAddress ip(const uint8_t (&address)[4]) {
  return IPAddress(address[0], address[1], address[2], address[3]);
}

// lwIP invokes this on the main thread from Ethernet.loop(), not from an ISR.
// Consume a contiguous packet in place; only chained pbufs need a scratch copy.
void receivePacket(void*, udp_pcb*, pbuf* packet, const ip_addr_t* source, u16_t) {
  if (packet == nullptr) return;
  const uint32_t sourceIP = ip4_addr_get_u32(ip_2_ip4(source));
  if (packet->tot_len >= 12 && packet->tot_len <= 530) {
    const auto* data = static_cast<const uint8_t*>(packet->payload);
    if (packet->len == packet->tot_len) {
      receiver.receive(data, packet->tot_len, micros(), sourceIP);
    } else {
      uint8_t scratch[530];
      const auto copied = pbuf_copy_partial(packet, scratch, packet->tot_len, 0);
      if (copied == packet->tot_len) receiver.receive(scratch, copied, micros(), sourceIP);
    }
  } else {
    ++receiver.stats.rejected;
  }
  pbuf_free(packet);
}

void startNetwork() {
  lastStartAttemptMs = millis();
  if (!ethernetStarted) {
    ethernetStarted = qn::Ethernet.begin(ip(kIP), ip(kNetmask), ip(kGateway));
    if (!ethernetStarted) return;
  }
  artnetSocket = udp_new_ip_type(IPADDR_TYPE_V4);
  if (artnetSocket == nullptr) return;
  ip_set_option(artnetSocket, SOF_BROADCAST);
  if (udp_bind(artnetSocket, IP_ADDR_ANY, kArtNetPort) != ERR_OK) {
    udp_remove(artnetSocket);
    artnetSocket = nullptr;
    return;
  }
  udp_recv(artnetSocket, receivePacket, nullptr);
}

void reportFps(uint32_t now) {
  if (!fps.sample(now, completedFrames) || !Serial) return;
  char line[224];
  const int length = std::snprintf(
      line, sizeof(line),
      "fps=%lu.%lu mode=%s frames=%lu partial=%lu sync=%lu replaced=%lu bad=%lu old=%lu\n",
      static_cast<unsigned long>(fps.fpsTenths / 10),
      static_cast<unsigned long>(fps.fpsTenths % 10),
      receiver.synchronized(now) ? "SYNC" : "ASYNC",
      static_cast<unsigned long>(completedFrames),
      static_cast<unsigned long>(receiver.stats.partialFrames),
      static_cast<unsigned long>(receiver.stats.syncPackets),
      static_cast<unsigned long>(receiver.stats.replacedFrames),
      static_cast<unsigned long>(receiver.stats.rejected),
      static_cast<unsigned long>(receiver.stats.outOfOrder));
  // Never wait for a monitor to connect or for room in its transmit queue.
  // Drop this report if the host cannot keep up; don't queue stale telemetry.
  if (length > 0 && size_t(length) < sizeof(line) && Serial.availableForWrite() >= length) {
    Serial.write(reinterpret_cast<const uint8_t*>(line), size_t(length));
  }
}
}  // namespace

void setup() {
  Serial.begin(115200);
  leds.begin();
  // Static addressing: no DHCP, link wait, USB wait, or startup light show.
  startNetwork();
}

void loop() {
  qn::Ethernet.loop();
  if (artnetSocket == nullptr && uint32_t(millis() - lastStartAttemptMs) >= 1000) {
    startNetwork();
  }

  // busy() covers both active DMA and the LED reset interval. Never enter
  // show() while it would wait. Its buffer copy/initial DMA preparation still
  // uses a short amount of CPU time; transmission then runs in the background.
  const bool outputBusy = leds.busy();
  if (frameInFlight && !outputBusy) {
    ++completedFrames;
    frameInFlight = false;
  }
  const uint32_t now = micros();
  reportFps(now);
  if (outputBusy) return;
  const bool frameReady = receiver.ready(now);
  // If input beat the startup clear, preserve it until a frame is ready.
  if (clearOnStartup && receiver.stats.accepted != 0) clearOnStartup = false;
  if (frameReady || clearOnStartup) {
    leds.show();
    if (frameReady) {
      receiver.presented();
      frameInFlight = true;
    }
    clearOnStartup = false;
  }
}
