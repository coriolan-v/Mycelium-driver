#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>

#include "config.h"

namespace mushroom {

struct ReceiverStats {
  uint32_t accepted = 0;
  uint32_t rejected = 0;
  uint32_t outOfOrder = 0;
  uint32_t frames = 0;
  uint32_t partialFrames = 0;
};

// Single-threaded: Ethernet callbacks and presentation both run in loop().
// The drawing buffer is separate from the buffer read by the DMA interrupt.
class Receiver {
 public:
  explicit Receiver(uint8_t* drawing) : drawing_(drawing) {}

  bool receive(const uint8_t* packet, size_t size, uint32_t now) {
    static constexpr uint8_t id[] = {'A', 'r', 't', '-', 'N', 'e', 't', 0};
    if (size < 18 || size > 530 || std::memcmp(packet, id, sizeof(id)) != 0 ||
        packet[8] != 0x00 || packet[9] != 0x50 ||
        (static_cast<uint16_t>(packet[10]) << 8 | packet[11]) < 14 ||
        (packet[15] & 0x80) != 0) {
      ++stats.rejected;
      return false;
    }

    const uint16_t universe = packet[14] | (uint16_t(packet[15]) << 8);
    const uint16_t length = (uint16_t(packet[16]) << 8) | packet[17];
    const uint16_t index = uint16_t(universe - kStartUniverse);
    if (index >= kUniverseCount ||
        length < 2 || length > 512 || (length & 1) || size != size_t(18 + length)) {
      ++stats.rejected;
      return false;
    }

    const uint8_t sequence = packet[12];
    // Sequence 0 disables ordering. Nonzero sequences wrap from 255 to 1.
    // Track each universe independently: senders need not share a sequence.
    if (sequence != 0 && sequences_[index] != 0 &&
        uint32_t(now - lastPacketUs_[index]) < kSequenceResetUs) {
      const unsigned delta = (unsigned(sequence) + 255 - sequences_[index]) % 255;
      if (delta == 0 || delta > 127) {
        ++stats.outOfOrder;
        return false;
      }
    }
    sequences_[index] = sequence;
    lastPacketUs_[index] = now;

    // Each universe owns exactly 170 pixels; channels 511/512 are padding.
    // Convert RGB -> GRB once, directly into the OctoWS2811 drawing buffer.
    size_t bytes = length < kChannelsPerUniverse ? length : kChannelsPerUniverse;
    bytes -= bytes % 3;
    uint8_t* dest = drawing_ + index * kChannelsPerUniverse;
    const uint8_t* data = packet + 18;
    for (size_t i = 0; i < bytes; i += 3) {
      dest[i] = data[i + 1];
      dest[i + 1] = data[i];
      dest[i + 2] = data[i + 2];
    }
    // Short universes clear their unused pixels rather than leaving stale data.
    if (bytes < kChannelsPerUniverse) {
      std::memset(dest + bytes, 0, kChannelsPerUniverse - bytes);
    }
    if (received_ == 0) firstPacketUs_ = now;
    received_ |= uint64_t(1) << index;
    ++stats.accepted;
    return true;
  }

  bool ready(uint32_t now) const {
    return received_ != 0 &&
           (received_ == allUniverses() ||
            uint32_t(now - firstPacketUs_) >= kPartialFrameTimeoutUs);
  }

  // Call only after show() has copied drawing data into its display buffer.
  void presented() {
    ++stats.frames;
    if (received_ != allUniverses()) ++stats.partialFrames;
    received_ = 0;
  }

  volatile ReceiverStats stats;  // Keep debugger counters observable with LTO.

 private:
  static constexpr uint64_t allUniverses() {
    return ~uint64_t(0) >> (64 - kUniverseCount);
  }

  uint8_t* const drawing_;
  uint64_t received_ = 0;
  uint32_t firstPacketUs_ = 0;
  uint32_t lastPacketUs_[kUniverseCount] = {};
  uint8_t sequences_[kUniverseCount] = {};
};

}  // namespace mushroom
