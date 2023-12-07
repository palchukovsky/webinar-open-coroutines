#include "download_result.hpp"

#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>

#include <iostream>
#include <mutex>
#include <thread>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;
using namespace std::chrono;

namespace lib {

Result download(const std::string_view host, const std::string_view pagePath) {
  {
    static std::mutex mutex;
    const std::lock_guard<std::mutex> lock(mutex);
    std::cout << "Downloading page in thread " << std::this_thread::get_id()
              << "..." << std::endl;
  }

  net::io_context context;

  beast::tcp_stream stream(context);
  stream.connect(tcp::resolver{context}.resolve(host, "https"));

  const auto protoVer = 11;  // HTTP/1.1
  http::request<http::string_body> request{http::verb::get, pagePath, protoVer};
  request.set(http::field::host, host);
  request.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

  const auto startTime = high_resolution_clock::now();

  http::write(stream, request);

  http::response<http::dynamic_body> response;
  {
    beast::flat_buffer buffer;
    http::read(stream, buffer, response);
  }

  return {
      response.body().size(),
      duration_cast<milliseconds>(high_resolution_clock::now() - startTime)};
}

}  // namespace lib
