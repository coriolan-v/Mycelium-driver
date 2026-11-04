#include <array>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "receiver.h"

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
  check(kPinCount == 16 && kUniverseCount == 48 && kLedsPerPin == 510 &&
            kFrameBytes == 24480, "16 outputs / 3 universes / 510 pixels");
  check(!f.receiver.ready(0xFFFFFFFF), "empty receiver never ready");
  // A coprime stride exercises every universe out of arrival order.
  for (size_t n = 0; n < kUniverseCount; ++n) {
    const size_t index = (n * 17) % kUniverseCount;
    auto packet = dmx(kStartUniverse + index, 1, n % 2 ? 510 : 512);
    packet[18] = uint8_t(index);
    check(f.receive(packet), "accept mapped universe");
    const auto* pixel = f.buffer.data() + 1 + index * 510;
    check(pixel[0] == 22 && pixel[1] == index && pixel[2] == 33,
          "correct universe offset and RGB to GRB order");
    check(pixel[507] == 22 && pixel[508] == 11 && pixel[509] == 33,
          "last pixel mapped; 512-channel padding ignored");
    check(f.receiver.ready(100) == (n == kUniverseCount - 1),
          "full frame ready exactly when all unique universes arrived");
    f.guards();
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
  check(!f.receive(dmx(48)), "universe beyond last pin rejected");
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
  check(f.buffer[1] == 22 && f.buffer[2] == 11 && f.buffer[3] == 33,
        "short packet contains one complete pixel");
  for (size_t i = 4; i <= 510; ++i) {
    check(f.buffer[i] == 0, "short universe clears all remaining channels");
  }
  check(!f.receiver.ready(99 + kPartialFrameTimeoutUs),
        "partial frame waits until deadline");
  check(f.receiver.ready(100 + kPartialFrameTimeoutUs),
        "lost universe cannot freeze outputs");
  f.receiver.presented();
  check(f.receiver.stats.partialFrames == 1, "partial frame counted");
  check(f.buffer[1] == 22, "presentation preserves last pixel values");
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
  check(f.buffer[2] == 99, "newer data replaces pending data without queue");
  f.guards();
}
}  // namespace

int main() {
  mapping();
  malformed();
  sequencing();
  shortAndMissing();
  clockWrapAndLatest();
  std::printf("PASS: %u checks (mapping, validation, sequencing, deadlines, wrap)\n", checks);
}
