#include "lib/download.hpp"
#include "lib/settings.hpp"
#include "lib/stat.hpp"

#include <chrono>
#include <iostream>
#include <mutex>
#include <string_view>
#include <thread>
#include <vector>

namespace {

// New extra-entity - to collect, sync and return results of downloading.
class Downloader {
 public:
  Downloader(const std::string_view host) : m_host{host} {}

 public:
  void downloadPage(const std::string_view target) {
    const auto result = lib::download(m_host, target);

    const std::lock_guard<std::mutex> lock{m_mutex};
    m_stat.add(result);
  }

  const lib::Stat &stat() const {
    // Has it to be thread safe?
    return m_stat;
  }

 private:
  std::string m_host;

  mutable std::mutex m_mutex;
  lib::Stat m_stat;
};

}  // namespace

void execute() {
  const auto &host = lib::host;
  const auto &pages = lib::pages;

  Downloader downloader{host};

  // New code - added list of threads...
  std::vector<std::thread> threads;
  threads.reserve(pages.size());

  const auto startTime = std::chrono::high_resolution_clock::now();

  // Starting new thread for each page downloading...
  for (const auto &page : pages) {
    threads.emplace_back(&Downloader::downloadPage, &downloader, page);
  }
  // New code - added waiting procedure. Useless for business logic :-(
  for (auto &thread : threads) {
    thread.join();
  }

  const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::high_resolution_clock::now() - startTime);

  downloader.stat().print(std::cout);
  std::cout << "Duration: " << duration.count() << " ms" << std::endl;
}
