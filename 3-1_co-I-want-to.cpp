#include "lib/download.hpp"
#include "lib/settings.hpp"
#include "lib/stat.hpp"

#include <chrono>
#include <coroutine>
#include <iostream>
#include <string_view>
#include <vector>

// This is just my humble wish, nothing more.

namespace {

// Adding asynchronous coroutine to call our synchronous download function.
auto download(const std::string_view host, const std::string_view page) {
  co_yield lib::download(lib::host, page);
  co_return;
}
}  // namespace

void execute() {
  const auto &host = lib::host;
  const auto &pages = lib::pages;

  std::vector<std::coroutine_handle<lib::Result>> results;
  results.reserve(pages.size());

  const auto startTime = std::chrono::high_resolution_clock::now();

  // Starting asynchronous download for each page and remember deferred result.
  for (const auto &page : pages) {
    results.push_back(download(lib::host, page));
  }

  lib::Stat stat;
  // `std::thread::join` and `std::future::get` replaced with co_wait.
  // Super! Compiler support!
  for (auto &r : results) {
    stat.add(co_await r);
  }

  const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::high_resolution_clock::now() - startTime);

  stat.print(std::cout);
  std::cout << "Duration: " << duration.count() << " ms" << std::endl;
}
