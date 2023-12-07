#include "lib/download.hpp"
#include "lib/settings.hpp"
#include "lib/stat.hpp"

#include <chrono>
#include <coroutine>
#include <exception>
#include <future>
#include <iostream>
#include <string_view>
#include <vector>

namespace {

struct TaskPromise {
  std::promise<lib::Result> p;
  std::future<lib::Result> get_return_object() { return p.get_future(); }
  std::suspend_never initial_suspend() { return {}; }
  std::suspend_never final_suspend() noexcept { return {}; }
  void return_value(lib::Result value) { p.set_value(value); }
  void unhandled_exception() { p.set_exception(std::current_exception()); }
};

struct Task {
  using promise_type = TaskPromise;
  std::future<lib::Result> fut;

  Task(std::future<lib::Result> f) : fut(std::move(f)) {}

  lib::Result wait() { return fut.get(); }
};

Task download(const std::string_view host, const std::string_view page) {
  co_return lib::download(host, page);
}
}  // namespace

void execute() {
  const auto &host = lib::host;
  const auto &pages = lib::pages;

  std::vector<Task> results;
  results.reserve(pages.size());

  const auto startTime = std::chrono::high_resolution_clock::now();

  // As before - starting new thread for each page downloading...
  for (const auto &page : pages) {
    results.push_back(download(host, page));
  }

  lib::Stat stat;
  for (auto &r : results) {
    stat.add(r.wait());
  }

  const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::high_resolution_clock::now() - startTime);

  stat.print(std::cout);
  std::cout << "Duration: " << duration.count() << " ms" << std::endl;
}
