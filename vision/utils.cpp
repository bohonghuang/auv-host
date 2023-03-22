#include "utils.h"
#include "block.h"

namespace auv::vision::utils {

std::vector<std::string> split(std::string_view str, std::string_view delim) {
  std::vector<std::string> results;
  size_t begin = 0;
  size_t pos = str.find(delim);
  while (pos < str.length()) {
    size_t length = pos - begin;
    results.emplace_back(str.substr(begin, length));
    begin = pos + delim.length();
    pos = str.find(delim, begin);
  }
  if (str.length() - begin)
    results.emplace_back(str.substr(begin, str.length() - begin));
  return results;
}

} // namespace auv::vision::utils
