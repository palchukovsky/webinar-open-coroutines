#pragma once

#include "download_result.hpp"

#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>

#include <string_view>

namespace lib {

Result downloadAsync(std::string_view host,
                     std::string_view pagePath,
                     boost::asio::io_context &context,
                     boost::asio::yield_context yield);

}  // namespace lib