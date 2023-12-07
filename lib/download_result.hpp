#pragma once

#include <chrono>

namespace lib {

struct Result {
  size_t sizeBytes;
  std::chrono::milliseconds timeSpent;
};

}  // namespace lib