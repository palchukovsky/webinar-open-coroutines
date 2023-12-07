#include "lib/download.hpp"
#include "lib/settings.hpp"
#include "lib/stat.hpp"

#include <chrono>
#include <iostream>

void execute() {
  const auto &host = lib::host;
  const auto &pages = lib::pages;

  lib::Stat stat;

  const auto startTime = std::chrono::high_resolution_clock::now();

  for (const auto &page : pages) {
    stat.add(lib::download(host, page));
  }

  const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::high_resolution_clock::now() - startTime);

  stat.print(std::cout);
  std::cout << "Duration: " << duration.count() << " ms" << std::endl;
}
