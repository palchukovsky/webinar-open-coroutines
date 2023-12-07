#include "client.hpp"

#include <boost/algorithm/string/trim_all.hpp>

#include <cassert>

namespace lib {

//------------------------------------------------------------------------------

namespace {
auto openFile(const std::string_view prefix, const std::string_view clientName)
    -> std::ofstream {
  const auto fileName =
      std::string{prefix} + "_" + std::string{clientName} + ".log";

  std::ofstream result{fileName, std::ios::app | std::ios::out};
  if (!result) {
    throw std::runtime_error{"Filed to open file"};
  }

  return result;
}

}  // namespace

//------------------------------------------------------------------------------

template <typename Char, typename Traits>
std::basic_ostream<Char, Traits>& operator<<(
    std::basic_ostream<Char, Traits>& os, Client::Sequence& sequence) {
  using namespace std::chrono;

  const auto now = system_clock::now();
  auto tp = now.time_since_epoch();
  const auto s = duration_cast<seconds>(tp);

  tp -= s;
  auto ms = duration_cast<milliseconds>(tp);

  const auto tt = system_clock::to_time_t(now);
  const std::tm& tm = *std::localtime(&tt);

  os << std::setw(5) << std::setfill(' ') << sequence.m_sequenceNo << ". ";

  os << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
  os << '.' << std::setw(3) << std::setfill('0') << ms.count();
  os << " ";

  ++sequence.m_sequenceNo;

  return os;
}

//------------------------------------------------------------------------------

Client::Client(const std::string_view prefix, const std::string_view name)
    : m_file{openFile(prefix, name)} {
  m_file << m_sequence << "---=== Client \"" << name << "\" stated ===---"
         << std::endl;
}

Client::~Client() {
  try {
    m_file << m_sequence << "^^^ ^^^ ^^^ Client stopped ^^^ ^^^ ^^^"
           << std::endl;
  } catch (...) {
    assert(false);
  }
}

void Client::putRecord(std::string_view record) {
  m_file << m_sequence << boost::algorithm::trim_copy(record) << std::endl;
}

//------------------------------------------------------------------------------

}  // namespace lib