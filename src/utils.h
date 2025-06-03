#include <iomanip>
#include <sstream>
#include <string>

// Convert a number to a string and front pad it.
inline std::string ToPaddedString(const int number, const int length,
                                  char pad_char) {
  std::ostringstream oss;
  oss << std::setw(length) << std::setfill(pad_char) << number;
  return oss.str();
}
