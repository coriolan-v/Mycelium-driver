#include <array>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "receiver.h"
#include "fps.h"

using namespace mushroom;

namespace {
unsigned checks = 0;
void check(bool condition, const char* message) {
  ++checks;
  if (!condition) {
    std::fprintf(stderr, "FAIL: %s\n", message);
    std::exit(1);
  }
}

std::vector<uint8_t> dmx(uint16_t universe, uint8_t sequence = 0,
                         uint16_t length = 510) {
  std::vector<uint8_t> packet(18 + length, 0);
  std::memcpy(packet.data(), "Art-Net", 8);
  packet[9] = 0x50;
  packet[11] = 14;
  packet[12] = sequence;
  universe += kStartUniverse;
  packet[14] = universe & 255;
  packet[15] = universe >> 8;
  packet[16] = length >> 8;
  packet[17] = length & 255;
  for (size_t i = 18; i + 2 < packet.size(); i += 3) {
    packet[i] = 11;
    packet[i + 1] = 22;
    packet[i + 2] = 33;
  }
  return packet;
}

struct Fixture {
  std::array<uint8_t, kFrameBytes + 2> buffer{};
  Receiver receiver{buffer.data() + 1};
  Fixture() {
    buffer.front() = 0xAB;
    buffer.back() = 0xCD;
  }
  bool receive(const std::vector<uint8_t>& p, uint32_t now = 100) {
    return receiver.receive(p.data(), p.size(), now);
  }
  void guards() {
    check(buffer.front() == 0xAB && buffer.back() == 0xCD, "buffer guards intact");
  }
};

void mapping() {
  Fixture f;
  check(kPinCount == 16 && kUniverseCount == kActivePorts * 3 && kLedsPerPin == 510 &&
            kFrameBytes == kActivePorts * 1530, "three full RGB universes per output");
  check(kOutputCycleUs == 15600, "output cycle includes all 510 pixels and reset");
  check(sizeof(kIP) == 4 && kStartUniverse + kUniverseCount <= 32768,
        "independent controller address and valid universe range");
  check(!f.receiver.ready(0xFFFFFFFF), "empty receiver never ready");
  // A coprime stride exercises every universe out of arrival order.
  for (size_t n = 0; n < kUniverseCount; ++n) {
    const size_t index = (n * 17) % kUniverseCount;
    auto packet = dmx(index, 1, n % 2 ? 510 : 512);
    packet[18] = uint8_t(index);
    check(f.receive(packet), "accept mapped universe");
    check(f.receiver.ready(100) == (n == kUniverseCount - 1),
          "full frame ready exactly when all unique universes arrived");
    f.guards();
  }
  for (size_t index = 0; index < kUniverseCount; ++index) {
    const size_t channel = index % 3 * 510;
    const size_t capacity = 510;
    const auto* pixel = f.buffer.data() + 1 + index / 3 * 1530 + channel;
    check(pixel[0] == index && pixel[1] == 22 && pixel[2] == 33,
          "correct packed universe offset and requested TM1812 R/G swap");
    check(pixel[capacity - 3] == 11 && pixel[capacity - 2] == 22 &&
              pixel[capacity - 1] == 33, "last pixel mapped; padding ignored");
  }
  f.receiver.presented();
  check(!f.receiver.ready(100000), "presentation resets receipt mask");
  check(f.receiver.stats.frames == 1 && f.receiver.stats.partialFrames == 0,
        "full frame statistics");
}

void malformed() {
  Fixture f;
  auto good = dmx(0);
  for (size_t size = 0; size < 18; ++size) {
    check(!f.receiver.receive(good.data(), size, 100), "short headers rejected");
  }
  auto bad = good; bad[0] = 'X';
  check(!f.receive(bad), "invalid magic rejected");
  bad = good; bad[7] = 1;
  check(!f.receive(bad), "magic terminator required");
  bad = good; bad[8] = 1;
  check(!f.receive(bad), "unsupported opcode rejected");
  bad = good; bad[11] = 13;
  check(!f.receive(bad), "old protocol rejected");
  bad = good; bad[15] = 0x80;
  check(!f.receive(bad), "reserved universe bit rejected");
  check(!f.receive(dmx(kUniverseCount)), "universe beyond last active pin rejected");
  if (kStartUniverse != 0) {
    bad = good;
    bad[14] = (kStartUniverse - 1) & 255;
    bad[15] = (kStartUniverse - 1) >> 8;
    check(!f.receive(bad), "universe below configured range rejected");
  }
  check(!f.receive(dmx(32767)), "high unrelated universe rejected");
  check(!f.receive(dmx(0, 0, 0)), "zero DMX length rejected");
  check(!f.receive(dmx(0, 0, 1)), "one-byte DMX rejected");
  check(!f.receive(dmx(0, 0, 509)), "odd DMX length rejected");
  check(!f.receive(dmx(0, 0, 514)), "oversized DMX rejected");
  bad = good; bad.pop_back();
  check(!f.receive(bad), "truncated payload rejected");
  bad = good; bad.push_back(0);
  check(!f.receive(bad), "inconsistent payload length rejected");
  bad = good; bad[16] = 255; bad[17] = 254;
  check(!f.receive(bad), "bogus huge declared length rejected");
  check(f.receiver.stats.accepted == 0 && !f.receiver.ready(100000),
        "bad packets never alter frame readiness");
  for (size_t i = 1; i <= kFrameBytes; ++i) {
    check(f.buffer[i] == 0, "bad packets never alter pixel memory");
  }
  f.guards();
}

void sequencing() {
  Fixture f;
  check(f.receive(dmx(0, 254)), "first sequence accepted");
  check(!f.receive(dmx(0, 254)), "duplicate sequence rejected");
  check(!f.receive(dmx(0, 253)), "older sequence rejected");
  check(f.receive(dmx(1, 2)), "sequence is independent per universe");
  check(f.receive(dmx(0, 255)), "sequence 255 accepted");
  check(f.receive(dmx(0, 1)), "sequence wraps 255 to 1");
  check(!f.receive(dmx(0, 255)), "late packet across wrap rejected");
  check(f.receive(dmx(0, 0)), "zero disables sequencing");
  check(f.receive(dmx(0, 0)), "repeated zero sequence accepted");
  check(f.receive(dmx(0, 80)), "sequence resumes after zero");
  check(!f.receive(dmx(0, 1)), "sender reset initially rejected");
  check(f.receive(dmx(0, 1), 100 + kSequenceResetUs),
        "sender restart accepted after inactivity");
  check(f.receiver.stats.outOfOrder == 4, "out-of-order counter");
}

void shortAndMissing() {
  Fixture f;
  check(f.receive(dmx(0)), "seed full universe");
  check(f.receive(dmx(0, 0, 4)), "short even universe accepted");
  check(!f.receiver.ready(99 + kPartialFrameTimeoutUs),
        "partial frame waits until deadline");
  check(f.receiver.ready(100 + kPartialFrameTimeoutUs),
        "lost universe cannot freeze outputs");
  check(f.buffer[1] == 11 && f.buffer[2] == 22 && f.buffer[3] == 33,
        "short packet contains one complete pixel");
  for (size_t i = 4; i <= 510; ++i) {
    check(f.buffer[i] == 0, "short universe clears all remaining channels");
  }
  f.receiver.presented();
  check(f.receiver.stats.partialFrames == 1, "partial frame counted");
  check(f.buffer[1] == 11, "presentation preserves last pixel values");
  check(!f.receiver.ready(2000000), "no retransmission without new data");
  f.guards();
}

void clockWrapAndLatest() {
  Fixture f;
  const uint32_t start = 0xFFFFF000u;
  check(f.receive(dmx(0, 20), start), "receive before micros wrap");
  check(!f.receiver.ready(start + kPartialFrameTimeoutUs - 1),
        "deadline stays pending across clock wrap");
  check(f.receiver.ready(start + kPartialFrameTimeoutUs),
        "deadline expires across clock wrap");
  check(!f.receive(dmx(0, 19), start + 5000),
        "sequence timer works across clock wrap");
  check(f.receive(dmx(0, 1), start + kSequenceResetUs),
        "sequence inactivity reset works across clock wrap");
  auto latest = dmx(0, 2);
  latest[18] = 99;
  check(f.receive(latest, start + kSequenceResetUs + 1), "receive newer frame");
  check(f.buffer[1] == 11, "released frame remains frozen while waiting for DMA");
  f.receiver.presented();
  check(f.receiver.ready(start + kSequenceResetUs + kPartialFrameTimeoutUs + 2),
        "later data remains pending and releases at its own deadline");
  check(f.buffer[1] == 99, "latest staged data appears at next release");
  f.guards();
}

std::vector<uint8_t> syncPacket() {
  std::vector<uint8_t> packet(14, 0);
  std::memcpy(packet.data(), "Art-Net", 8);
  packet[9] = 0x52;
  packet[11] = 14;
  return packet;
}

void artSync() {
  Fixture f;
  const uint32_t sender = 0x0100000A;
  auto sync = syncPacket();
  check(!f.receiver.receive(sync.data(), sync.size(), 10, sender),
        "sync before any mapped data is ignored");
  auto packet = dmx(0, 1);
  check(f.receiver.receive(packet.data(), packet.size(), 100, sender), "source data accepted");
  check(!f.receiver.receive(sync.data(), sync.size(), 101, sender + 1),
        "sync from unrelated source ignored");
  check(!f.receiver.synchronized(101), "wrong source cannot enable sync mode");
  check(!f.receiver.receive(sync.data(), 13, 102, sender), "truncated sync rejected");
  auto oversized = sync; oversized.push_back(0);
  check(!f.receiver.receive(oversized.data(), oversized.size(), 102, sender),
        "oversized sync rejected");
  check(f.receiver.receive(sync.data(), sync.size(), 103, sender), "matching sync accepted");
  check(f.receiver.synchronized(103) && f.receiver.ready(103),
        "first matching sync releases buffered data and enables sync mode");
  check(f.buffer[1] == 11, "sync snapshot contains received data");

  packet = dmx(0, 2); packet[18] = 99;
  check(f.receiver.receive(packet.data(), packet.size(), 104, sender), "next frame staged");
  check(f.buffer[1] == 11, "next frame cannot overwrite released snapshot");
  f.receiver.presented();
  check(!f.receiver.ready(104 + kPartialFrameTimeoutUs),
        "normal partial deadline cannot bypass ArtSync");
  check(f.receiver.receive(sync.data(), sync.size(), 20000, sender), "second sync accepted");
  check(f.receiver.ready(20000) && f.buffer[1] == 99, "next sync releases next frame");

  packet = dmx(0, 3); packet[18] = 77;
  check(f.receiver.receive(packet.data(), packet.size(), 20001, sender), "newer pending frame");
  check(f.receiver.receive(sync.data(), sync.size(), 20002, sender), "newer sync while DMA busy");
  check(f.buffer[1] == 77 && f.receiver.stats.replacedFrames == 1,
        "newest synchronized snapshot replaces unsent snapshot without queue");
  f.receiver.presented();
  check(f.receiver.stats.partialFrames == 2, "partial counts refer to released snapshots");
  check(f.receiver.receive(sync.data(), sync.size(), 20003, sender), "sync without changes accepted");
  check(!f.receiver.ready(20003), "empty sync does not generate an artificial output frame");
  check(f.receiver.stats.syncPackets == 4 && f.receiver.stats.ignoredSync == 2,
        "sync packet and source filtering statistics");
  f.guards();
}

void syncTimeoutAndSourceChange() {
  Fixture f;
  auto sync = syncPacket();
  const uint32_t start = 0xFFFFF000;
  const uint32_t sender = 1;
  auto packet = dmx(0, 200);
  f.receiver.receive(packet.data(), packet.size(), start, sender);
  f.receiver.receive(sync.data(), sync.size(), start + 1, sender);
  f.receiver.presented();
  for (size_t i = 0; i < kUniverseCount; ++i) {
    packet = dmx(i, 201);
    check(f.receiver.receive(packet.data(), packet.size(), start + 2, sender), "stage complete sync frame");
  }
  check(!f.receiver.ready(start + kSyncTimeoutUs), "hold until exact four-second boundary");
  check(f.receiver.ready(start + 1 + kSyncTimeoutUs), "four-second timeout releases data across micros wrap");
  check(!f.receiver.synchronized(start + 1 + kSyncTimeoutUs) &&
            f.receiver.stats.syncTimeouts == 1, "fallback changes mode and is counted once");
  f.receiver.presented();
  f.receiver.ready(start + 2 + kSyncTimeoutUs);
  check(f.receiver.stats.syncTimeouts == 1, "timeout counted once");

  packet = dmx(0, 1);
  check(f.receiver.receive(packet.data(), packet.size(), 5000000, sender + 1),
        "new sender resets old sequence state");
  check(!f.receiver.receive(sync.data(), sync.size(), 5000001, sender),
        "old sender sync no longer controls output");
  check(f.receiver.receive(sync.data(), sync.size(), 5000002, sender + 1),
        "new sender can establish sync");
  check(f.receiver.ready(5000002), "new sender frame released");
  f.guards();
}

void fpsReporting() {
  FpsMeter meter;
  check(!meter.sample(999999, 60), "no report before interval");
  check(meter.sample(1000000, 60) && meter.fpsTenths == 600, "60 completed frames means 60 FPS");
  check(meter.sample(3000000, 120) && meter.fpsTenths == 300,
        "delayed report uses actual two-second duration");
  check(meter.sample(4000000, 120) && meter.fpsTenths == 0, "stopped output reports zero");
  FpsMeter wrapping(0xFFFFFFFFu - 500000, 0xFFFFFFFFu - 29);
  check(wrapping.sample(uint32_t(0xFFFFFFFFu - 500000 + 1000000), 30) &&
            wrapping.fpsTenths == 600, "clock and frame counter wrap do not corrupt FPS");
}

void finalUniverseClipping() {
  Fixture f;
  auto packet = dmx(2, 1, 510);
  packet[525] = 91; packet[526] = 92; packet[527] = 93;
  check(f.receive(packet), "third universe accepts all 510 channels");
  check(f.receiver.ready(100 + kPartialFrameTimeoutUs), "release full final universe");
  check(f.buffer[1021] == 11 && f.buffer[1528] == 91 &&
            f.buffer[1529] == 92 && f.buffer[1530] == 93,
        "third universe maps all 170 pixels including port pixel 510");
  check(f.buffer[1531] == 0, "third universe cannot touch the next output");
  f.receiver.presented();
  auto padded = dmx(2, 2, 512);
  padded[528] = 0xEF; padded[529] = 0xFE;
  check(f.receive(padded, 20000), "padded final universe accepted");
  check(f.receiver.ready(20000 + kPartialFrameTimeoutUs), "release padded final universe");
  check(f.buffer[1531] == 0 && f.buffer[1532] == 0,
        "channels 511/512 cannot spill into next port");
  f.receiver.presented();
  check(f.receive(dmx(2, 3, 60), 40000), "short third universe still accepted");
  check(f.receiver.ready(40000 + kPartialFrameTimeoutUs), "release short third universe");
  check(f.buffer[1080] == 33, "short packet preserves its first 20 pixels");
  for (size_t i = 1081; i <= 1530; ++i) {
    check(f.buffer[i] == 0, "short third universe clears its remaining pixels");
  }
  f.guards();
}

void tm1812ColourSwap() {
  Fixture f;
  auto packet = dmx(0, 1, 12);
  const uint8_t colours[] = {255, 0, 0, 0, 255, 0, 0, 0, 255, 10, 20, 30};
  std::memcpy(packet.data() + 18, colours, sizeof(colours));
  check(f.receive(packet), "TM1812 colour test packet accepted");
  check(f.receiver.ready(100 + kPartialFrameTimeoutUs), "release colour test");
  const uint8_t previousGRB[] = {0, 255, 0, 255, 0, 0, 0, 0, 255, 20, 10, 30};
  const auto* output = f.buffer.data() + 1;
  for (size_t i = 0; i < sizeof(colours); i += 3) {
    check(output[i] == previousGRB[i + 1] && output[i + 1] == previousGRB[i] &&
              output[i + 2] == previousGRB[i + 2],
          "TM1812 output swaps R/G relative to previous GRB; blue is unchanged");
  }
  f.guards();
}
}  // namespace

int main() {
  mapping();
  malformed();
  sequencing();
  shortAndMissing();
  clockWrapAndLatest();
  artSync();
  syncTimeoutAndSourceChange();
  fpsReporting();
  finalUniverseClipping();
  tm1812ColourSwap();
  std::printf("PASS: %u ports, start universe %u, %u checks (mapping, ArtSync, FPS, validation, wrap)\n",
              unsigned(kActivePorts), unsigned(kStartUniverse), checks);
}
