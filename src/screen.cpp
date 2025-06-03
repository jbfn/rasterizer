#include <fstream>
#include <iostream>
#include <string>

#include "screen.h"
#include "structs.h"

void WriteScreenToFile(const Screen &screen, const std::string &name) {
  const std::string filePath = name + ".bmp";
  std::ofstream out(filePath, std::ios::binary);
  if (!out)
    return;

  uint32_t width = screen.getWidth();
  uint32_t height = screen.getHeight();

  uint32_t fileHeaderSize = 14;
  uint32_t infoHeaderSize = 40;
  uint32_t dataSize = width * height * 4; // 4 bytes per pixel (RGB + padding)
  uint32_t fileSize = fileHeaderSize + infoHeaderSize + dataSize;

  // ----- FILE HEADER -----
  out.put('B');
  out.put('M');
  out.write(reinterpret_cast<char *>(&fileSize), 4);
  uint32_t reserved = 0;
  out.write(reinterpret_cast<char *>(&reserved), 4);
  uint32_t dataOffset = fileHeaderSize + infoHeaderSize;
  out.write(reinterpret_cast<char *>(&dataOffset), 4);

  // ----- INFO HEADER -----
  out.write(reinterpret_cast<char *>(&infoHeaderSize), 4);
  out.write(reinterpret_cast<char *>(&width), 4);
  out.write(reinterpret_cast<char *>(&height), 4);
  uint16_t planes = 1;
  out.write(reinterpret_cast<char *>(&planes), 2);
  uint16_t bitsPerPixel = 32; // 4 bytes = 32 bits
  out.write(reinterpret_cast<char *>(&bitsPerPixel), 2);
  uint32_t compression = 0;
  out.write(reinterpret_cast<char *>(&compression), 4);
  out.write(reinterpret_cast<char *>(&dataSize), 4);
  uint32_t misc[4] = {0, 0, 0, 0}; // resolution, palette
  out.write(reinterpret_cast<char *>(misc), 16);

  // ----- DATA -----
  for (int y = height - 1; y >= 0; --y) { // BMP writes bottom to top
    for (uint32_t x = 0; x < width; ++x) {
      Vector3 pixel = screen(x, y);
      uint8_t r = static_cast<uint8_t>(pixel[0] * 255);
      uint8_t g = static_cast<uint8_t>(pixel[1] * 255);
      uint8_t b = static_cast<uint8_t>(pixel[2] * 255);
      uint8_t padding = 0;
      out.put(b); // BMP uses BGR order
      out.put(g);
      out.put(r);
      out.put(padding); // alignment byte
    }
  }

  out.close();
}
