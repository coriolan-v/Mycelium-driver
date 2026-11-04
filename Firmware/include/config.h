#pragma once

#include <cstddef>
#include <cstdint>

namespace mushroom {

constexpr char kFirmwareVersion[] = "3.0.0";

// EDIT THESE SETTINGS FOR EACH INDEPENDENT CONTROLLER, THEN BUILD AND UPLOAD.
// Give the second controller a different IP, e.g. {10, 0, 0, 10}.
constexpr uint8_t kIP[] = {192, 168, 1, 201};
constexpr uint8_t kNetmask[] = {255, 0, 0, 0};
constexpr uint8_t kGateway[] = {0, 0, 0, 0};
constexpr uint16_t kStartUniverse = 0;
constexpr size_t kActivePorts = 16;

// Output order follows the first 16 entries of _dev/Mushroom_FW_2.
constexpr uint8_t kPins[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 24, 25, 26};
constexpr size_t kPinCount = sizeof(kPins) / sizeof(kPins[0]);
constexpr size_t kPixelsPerRod = 36;
constexpr size_t kRodsPerPort = 10;
constexpr size_t kUniversesPerPin = 3;
constexpr size_t kChannelsPerUniverse = 510;
constexpr size_t kUniverseCount = kActivePorts * kUniversesPerPin;
constexpr size_t kLedsPerPin = kPixelsPerRod * kRodsPerPort;
constexpr size_t kBytesPerPort = kLedsPerPin * 3;
constexpr size_t kFrameBytes = kActivePorts * kBytesPerPort;
constexpr uint16_t kArtNetPort = 6454;

// 24 bits at 800 kHz per pixel, plus the driver's 300 us reset allowance.
constexpr uint32_t kOutputCycleUs = kLedsPerPin * 30 + 300;
// A lost universe must not freeze the entire installation. This is a deadline,
// checked while doing other work, never a delay or a busy wait.
constexpr uint32_t kPartialFrameTimeoutUs = kOutputCycleUs;
constexpr uint32_t kSequenceResetUs = 1000000;
constexpr uint32_t kSyncTimeoutUs = 4000000;
constexpr uint32_t kFpsReportIntervalUs = 1000000;

// QNEthernet uses the board's unique factory MAC address.

constexpr bool uniquePins() {
  for (size_t i = 0; i < kPinCount; ++i) {
    for (size_t j = i + 1; j < kPinCount; ++j) {
      if (kPins[i] == kPins[j]) return false;
    }
  }
  return true;
}

static_assert(kPinCount == 16, "This firmware is configured for 16 outputs");
static_assert(kActivePorts > 0 && kActivePorts <= kPinCount,
              "Active outputs must fit the available pins");
static_assert((kBytesPerPort + kChannelsPerUniverse - 1) / kChannelsPerUniverse ==
                  kUniversesPerPin, "Universe count must match the strip length");
static_assert(uniquePins(), "Output pins must be unique");
static_assert(kChannelsPerUniverse % 3 == 0 && kChannelsPerUniverse <= 512,
              "Each universe must contain whole RGB pixels");
static_assert(kUniverseCount <= 64, "Receipt mask supports up to 64 universes");
static_assert(kStartUniverse + kUniverseCount <= 32768,
              "Universes must fit the Art-Net 15-bit port address");

}  // namespace mushroom
