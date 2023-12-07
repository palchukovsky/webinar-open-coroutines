#include "download_result.hpp"

#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>

#include <iostream>
#include <thread>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;
using namespace std::chrono;

namespace lib {

Result downloadAsync(const std::string_view host,
                     const std::string_view pagePath,
                     boost::asio::io_context &context,
                     boost::asio::yield_context yield) {
  std::cout << "Downloading page in thread " << std::this_thread::get_id()
            << "..." << std::endl;

  beast::error_code ec;

  tcp::resolver resolver{context};
  const auto resolved =
      resolver.async_resolve(host, "https", yield[ec]);  // was "resolve"
  if (ec) {
    throw std::runtime_error{"Failed to resolve host"};
  }

  beast::tcp_stream stream(context);
  stream.async_connect(resolved, yield[ec]);  // was "connect"
  if (ec) {
    throw std::runtime_error{"Failed to connect to host"};
  }

  const auto protoVer = 11;  // HTTP/1.1
  http::request<http::string_body> request{http::verb::get, pagePath, protoVer};
  request.set(http::field::host, host);
  request.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

  const auto startTime = high_resolution_clock::now();

  http::async_write(stream, request, yield[ec]);  // was "write"
  if (ec) {
    throw std::runtime_error{"Failed to write request"};
  }

  http::response<http::dynamic_body> response;
  {
    beast::flat_buffer buffer;
    http::async_read(stream, buffer, response, yield[ec]);  // was "read"
    if (ec) {
      throw std::runtime_error{"Failed to read response"};
    }
  }

  return {
      response.body().size(),
      duration_cast<milliseconds>(high_resolution_clock::now() - startTime)};
}

}  // namespace lib
