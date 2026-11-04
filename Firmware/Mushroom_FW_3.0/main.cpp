// Mushroom firmware v3.0 - Teensy 4.1, 16 outputs / 48 universes.
#include <Arduino.h>
#include <OctoWS2811.h>
#include <QNEthernet.h>
#include <lwip/pbuf.h>
#include <lwip/udp.h>

#include "config.h"
#include "receiver.h"

namespace {
using namespace mushroom;
namespace qn = qindesign::network;

// The ISR reads displayMemory; receive callbacks write only drawingMemory.
// 24,480 bytes per buffer, for 8,160 RGB LEDs across 16 parallel outputs.
DMAMEM uint8_t displayMemory[kFrameBytes] __attribute__((aligned(32)));
alignas(32) uint8_t drawingMemory[kFrameBytes];
OctoWS2811 leds(kLedsPerPin, displayMemory, drawingMemory,
               WS2811_GRB | WS2811_800kHz, kPinCount, kPins);
Receiver receiver(drawingMemory);
udp_pcb* artnetSocket = nullptr;
bool ethernetStarted = false;
bool clearOnStartup = true;
uint32_t lastStartAttemptMs = 0;

IPAddress ip(const uint8_t (&address)[4]) {
  return IPAddress(address[0], address[1], address[2], address[3]);
}

// lwIP invokes this on the main thread from Ethernet.loop(), not from an ISR.
// Consume a contiguous packet in place; only chained pbufs need a scratch copy.
void receivePacket(void*, udp_pcb*, pbuf* packet, const ip_addr_t*, u16_t) {
  if (packet == nullptr) return;
  if (packet->tot_len >= 18 && packet->tot_len <= 530) {
    const auto* data = static_cast<const uint8_t*>(packet->payload);
    if (packet->len == packet->tot_len) {
      receiver.receive(data, packet->tot_len, micros());
    } else {
      uint8_t scratch[530];
      const auto copied = pbuf_copy_partial(packet, scratch, packet->tot_len, 0);
      if (copied == packet->tot_len) receiver.receive(scratch, copied, micros());
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
  if (udp_bind(artnetSocket, IP_ADDR_ANY, kArtNetPort) != ERR_OK) {
    udp_remove(artnetSocket);
    artnetSocket = nullptr;
    return;
  }
  udp_recv(artnetSocket, receivePacket, nullptr);
}
}  // namespace

void setup() {
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
  if (leds.busy()) return;
  const bool frameReady = receiver.ready(micros());
  // If input beat the startup clear, preserve it until a frame is ready.
  if (clearOnStartup && receiver.stats.accepted != 0) clearOnStartup = false;
  if (frameReady || clearOnStartup) {
    leds.show();
    if (frameReady) receiver.presented();
    clearOnStartup = false;
  }
}
