#pragma once

#include "download_result.hpp"

#include <chrono>
#include <iostream>

namespace lib {

struct Stat {
  void add(const Result &result) {
    ++m_numberOfPages;
    m_numberOfBytes += result.sizeBytes;
    m_totalTimeSpent += result.timeSpent;
  }

  void print(std::ostream &os) const {
    os << "Stat:\n"
       << "  Pages: " << m_numberOfPages << "\n"
       << "  Bytes: " << m_numberOfBytes << "\n"
       << "  Total time spent: " << m_totalTimeSpent.count() << " ms\n"
       << "  Average time per page: "
       << m_totalTimeSpent.count() / m_numberOfPages << " ms\n"
       << "  Average page size: " << m_numberOfBytes / m_totalTimeSpent.count()
       << std::endl;
  }

 private:
  size_t m_numberOfPages{0};
  size_t m_numberOfBytes{0};
  std::chrono::milliseconds m_totalTimeSpent{0};
};

}  // namespace lib
