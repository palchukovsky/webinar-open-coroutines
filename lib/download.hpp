#pragma once

#include "download_result.hpp"

#include <string_view>

namespace lib {

Result download(std::string_view host, std::string_view pagePath);

}  // namespace lib