#include "lib/download.hpp"
#include "lib/settings.hpp"
#include "lib/stat.hpp"

#include <chrono>
#include <future>
#include <iostream>
#include <string_view>
#include <vector>

void execute() {
  const auto &host = lib::host;
  const auto &pages = lib::pages;

  std::vector<std::future<lib::Result>> results;
  results.reserve(pages.size());

  const auto startTime = std::chrono::high_resolution_clock::now();

  // As before - starting new thread for each page downloading...
  for (const auto &page : pages) {
    results.push_back(std::async(&lib::download, host, page));
  }

  lib::Stat stat;
  // `std::thread::join` replaced with result waiting and getting.
  for (auto &r : results) {
    stat.add(r.get());
  }

  const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::high_resolution_clock::now() - startTime);

  stat.print(std::cout);
  std::cout << "Duration: " << duration.count() << " ms" << std::endl;
}
