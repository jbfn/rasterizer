#include <iomanip>
#include <sstream>
#include <string>

// Convert a number to a string and front pad it.
inline std::string ToPaddedString(const uint16_t number, const uint8_t length,
                                  char pad_char) {
  std::ostringstream oss;
  oss << std::setw(length) << std::setfill(pad_char) << number;
  return oss.str();
}
