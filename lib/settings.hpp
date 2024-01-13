#include <array>
#include <string_view>

namespace lib {

const std::string_view host = "en.cppreference.com";
const std::array pages = {"/w",                       //
                          "/w/cpp/standard_library",  //
                          "/w/cpp/language",          //
                          "/w/cpp/memory",            //
                          "/w/cpp/concepts",          //
                          "/w/cpp/thread/thread",     //
                          "/w/cpp/thread/jthread",    //
                          "/w/cpp/thread/mutex",      //
                          "/w/cpp/thread/condition_variable"};

const auto port = 2009;

}  // namespace lib