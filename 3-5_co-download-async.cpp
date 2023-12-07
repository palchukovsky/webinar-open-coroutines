#include "lib/download.hpp"
#include "lib/download_async.hpp"
#include "lib/settings.hpp"
#include "lib/stat.hpp"

#include <chrono>
#include <exception>
#include <future>
#include <iostream>
#include <string_view>
#include <thread>
#include <vector>

namespace {

std::future<lib::Result> download(boost::asio::io_context &context,
                                  const std::string_view host,
                                  const std::string_view page) {
  std::packaged_task task(
      [&context, host, page](boost::asio::yield_context yield) {
        return lib::downloadAsync(host, page, context, yield);
      });

  auto result = task.get_future();

  boost::asio::spawn(  // <-- Spawn doesn't mean "start right now".
      context, std::move(task));

  return result;
}

}  // namespace

void execute() {
  const auto &host = lib::host;
  const auto &pages = lib::pages;

  boost::asio::io_context context;

  std::vector<std::future<lib::Result>> results;
  results.reserve(pages.size());

  const auto startTime = std::chrono::high_resolution_clock::now();

  for (const auto &page : pages) {
    results.push_back(download(context, host, page));
  }

  context.run();  // <--- Here we start all the tasks.

  lib::Stat stat;
  for (auto &r : results) {
    stat.add(r.get());
  }

  const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::high_resolution_clock::now() - startTime);

  stat.print(std::cout);
  std::cout << "Duration: " << duration.count() << " ms" << std::endl;
}
