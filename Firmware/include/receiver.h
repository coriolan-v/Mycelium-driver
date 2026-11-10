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
  uint32_t syncPackets = 0;
  uint32_t ignoredSync = 0;
  uint32_t replacedFrames = 0;
  uint32_t syncTimeouts = 0;
};

// Single-threaded: Ethernet callbacks and presentation both run in loop().
// Staging holds arriving data, drawing holds a frozen released frame, and the
// OctoWS2811 display buffer is read by DMA. New data cannot alter a synced frame.
class Receiver {
 public:
  explicit Receiver(uint8_t* drawing) : drawing_(drawing) {}

  bool receive(const uint8_t* packet, size_t size, uint32_t now,
               uint32_t sourceIP = 0) {
    static constexpr uint8_t id[] = {'A', 'r', 't', '-', 'N', 'e', 't', 0};
    if (size < 12 || size > 530 || std::memcmp(packet, id, sizeof(id)) != 0 ||
        (static_cast<uint16_t>(packet[10]) << 8 | packet[11]) < 14) {
      ++stats.rejected;
      return false;
    }

    const uint16_t opcode = packet[8] | (uint16_t(packet[9]) << 8);
    if (opcode == 0x5200) {
      if (size != 14) {
        ++stats.rejected;
        return false;
      }
      if (!haveSource_ || sourceIP != sourceIP_) {
        ++stats.ignoredSync;
        return false;
      }
      syncActive_ = true;
      lastSyncUs_ = now;
      ++stats.syncPackets;
      if (received_ != 0) capture();
      return true;
    }
    if (opcode != 0x5000 || size < 18 || (packet[15] & 0x80) != 0) {
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

    // This receiver does not merge sources. The latest valid data sender owns
    // sync; a sender change also resets that sender's sequence history.
    if (haveSource_ && sourceIP != sourceIP_) {
      std::memset(sequences_, 0, sizeof(sequences_));
      received_ = 0;
      if (pending_) ++stats.replacedFrames;
      pending_ = false;
      syncActive_ = false;
    }
    sourceIP_ = sourceIP;
    haveSource_ = true;

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

    const size_t port = index / kUniversesPerPin;
    const size_t channel = (index % kUniversesPerPin) * kChannelsPerUniverse;
    const size_t remaining = kBytesPerPort - channel;
    const size_t capacity = remaining < kChannelsPerUniverse ? remaining : kChannelsPerUniverse;
    // Each port occupies 1530 bytes: three full 510-channel RGB universes.
    // Ignore channels 511/512 if the sender pads a packet to 512 channels.
    size_t bytes = length < capacity ? length : capacity;
    bytes -= bytes % 3;
    uint8_t* dest = staging_ + port * kBytesPerPort + channel;
    const uint8_t* data = packet + 18;
    // TM1812 wire order is RGB. This exchanges R/G relative to the previous
    // GRB output. Copy directly; the DMA driver does not reorder these bytes.
    std::memcpy(dest, data, bytes);
    // Short universes clear their unused pixels rather than leaving stale data.
    if (bytes < capacity) {
      std::memset(dest + bytes, 0, capacity - bytes);
    }
    if (received_ == 0) firstPacketUs_ = now;
    received_ |= uint64_t(1) << index;
    ++stats.accepted;
    return true;
  }

  bool ready(uint32_t now) {
    if (syncActive_ && !synchronized(now)) {
      syncActive_ = false;
      ++stats.syncTimeouts;
    }
    if (!pending_ && !syncActive_ && received_ != 0 &&
        (received_ == allUniverses() ||
         uint32_t(now - firstPacketUs_) >= kPartialFrameTimeoutUs)) {
      capture();
    }
    return pending_;
  }

  bool synchronized(uint32_t now) const {
    return syncActive_ && uint32_t(now - lastSyncUs_) < kSyncTimeoutUs;
  }

  // Call only after show() has copied drawing data into its display buffer.
  void presented() {
    ++stats.frames;
    if (pendingPartial_) ++stats.partialFrames;
    pending_ = false;
  }

  volatile ReceiverStats stats;  // Keep debugger counters observable with LTO.

 private:
  void capture() {
    if (pending_) ++stats.replacedFrames;
    std::memcpy(drawing_, staging_, kFrameBytes);
    pendingPartial_ = received_ != allUniverses();
    pending_ = true;
    received_ = 0;
  }

  static constexpr uint64_t allUniverses() {
    return ~uint64_t(0) >> (64 - kUniverseCount);
  }

  uint8_t* const drawing_;
  alignas(32) uint8_t staging_[kFrameBytes] = {};
  uint64_t received_ = 0;
  uint32_t firstPacketUs_ = 0;
  uint32_t lastPacketUs_[kUniverseCount] = {};
  uint8_t sequences_[kUniverseCount] = {};
  bool pending_ = false;
  bool pendingPartial_ = false;
  bool haveSource_ = false;
  bool syncActive_ = false;
  uint32_t sourceIP_ = 0;
  uint32_t lastSyncUs_ = 0;
};

}  // namespace mushroom
