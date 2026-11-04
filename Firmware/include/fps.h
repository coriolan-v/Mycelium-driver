#pragma once

#include <cstdint>
#include "config.h"

namespace mushroom {

// Count a frame only after DMA and the LED reset interval have finished.
// Sample over actual elapsed time, rather than assuming loop/report cadence.
class FpsMeter {
 public:
  explicit FpsMeter(uint32_t now = 0, uint32_t completed = 0)
      : lastUs_(now), lastCompleted_(completed) {}

  bool sample(uint32_t now, uint32_t completed) {
    const uint32_t elapsed = now - lastUs_;
    if (elapsed < kFpsReportIntervalUs) return false;
    fpsTenths = uint64_t(uint32_t(completed - lastCompleted_)) * 10000000 / elapsed;
    lastUs_ = now;
    lastCompleted_ = completed;
    return true;
  }

  uint32_t fpsTenths = 0;

 private:
  uint32_t lastUs_;
  uint32_t lastCompleted_;
};

}  // namespace mushroom
