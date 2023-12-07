#include "lib/client.hpp"
#include "lib/settings.hpp"

#include <boost/algorithm/string/trim_all.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/streambuf.hpp>

#include <iostream>
#include <thread>

namespace asio = boost::asio;
using tcp = asio::ip::tcp;

namespace {

asio::awaitable<void> workWithClient(tcp::socket socket) {
  asio::streambuf buffer;
  co_await asio::async_read_until(socket, buffer, "\n", asio::use_awaitable);

  std::string name{asio::buffer_cast<const char*>(buffer.data()),
                   buffer.size()};
  boost::trim(name);

  std::cout << std::this_thread::get_id() << " Client \"" << name
            << "\": Connected." << std::endl;

  lib::Client client("co", name);

  for (;;) {
    buffer.consume(buffer.size());

    try {
      co_await asio::async_read_until(socket, buffer, "\n",
                                      asio::use_awaitable);
    } catch (const boost::system::system_error& ex) {
      if (ex.code() != asio::error::eof) {
        throw;
      }
      break;
    }

    const std::string_view logRecord{
        asio::buffer_cast<const char*>(buffer.data()), buffer.size()};

    std::cout << std::this_thread::get_id() << " Client \"" << name
              << "\": New log record received." << std::endl;

    client.putRecord(logRecord);

    // Try this:
    //    std::this_thread::sleep_for(std::chrono::seconds(15));
    // ...then try this:
    //    asio::steady_timer timer{co_await asio::this_coro::executor};
    //    timer.expires_after(std::chrono::seconds(15));
    //    co_await timer.async_wait(asio::use_awaitable);
    // ... then run asio::io_context in more than 1 thread and repeat both.
  }

  std::cout << std::this_thread::get_id() << " Client \"" << name
            << "\": Disconnected." << std::endl;
}

asio::awaitable<void> listenNewClient() {
  const auto executor = co_await asio::this_coro::executor;

  std::cout << std::this_thread::get_id() << " Listing TCP v4 port "
            << lib::port << " for new log clients..." << std::endl;
  tcp::acceptor acceptor{executor, {tcp::v4(), lib::port}};

  for (;;) {
    auto socket = co_await acceptor.async_accept(asio::use_awaitable);
    std::cout << std::this_thread::get_id() << " Accepting new connection..."
              << std::endl;

    asio::co_spawn(executor, workWithClient(std::move(socket)), asio::detached);
  }
}

}  // namespace

void execute() {
  std::cout << std::this_thread::get_id() << " Running server..." << std::endl;

  asio::io_context ioContext;

  asio::signal_set signals{ioContext, SIGINT, SIGTERM};
  signals.async_wait([&](auto, auto) { ioContext.stop(); });

  asio::co_spawn(ioContext, listenNewClient, asio::detached);

  ioContext.run();

  std::cout << std::this_thread::get_id() << " Server stopped." << std::endl;
}
