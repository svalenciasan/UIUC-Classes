#include "cs225/PNG.h"
#include "cs225/HSLAPixel.h"

#include <string>

using cs225::PNG;
using cs225::HSLAPixel;

void rotate(std::string inputFile, std::string outputFile) {
  PNG image;
  image.readFromFile(inputFile);
  PNG rotated_image(image.width(), image.height());

  unsigned rotated_x = image.width() - 1;
  unsigned rotated_y = image.height() - 1;

  for (unsigned x = 0; x < image.width(); x++) {
    for (unsigned y = 0; y < image.height(); y++) {
      HSLAPixel & pixel = image.getPixel(x, y);
      HSLAPixel & rotated_pixel = rotated_image.getPixel(rotated_x, rotated_y);

      rotated_pixel.h = pixel.h;
      rotated_pixel.s = pixel.s;
      rotated_pixel.l = pixel.l;
      rotated_pixel.a = pixel.a;

      rotated_y--;
    }
    rotated_y = image.height() - 1;
    rotated_x--;
  }
  rotated_image.writeToFile(outputFile);
}

cs225::PNG myArt(unsigned int width, unsigned int height) {
  cs225::PNG png(width, height);

  unsigned length = png.width();
  double h_value = 360;
  double s_value = 1;
  double l_value = .5;
  double a_value = 1;

  while (length > 0) {
    
    for (unsigned x = 0; x < length; x++) {
      for (unsigned y = 0; y < length; y++) {
        HSLAPixel & pixel = png.getPixel(x, y);

        pixel.h = h_value;
        pixel.s = s_value;
        pixel.l = l_value;
        pixel.a = a_value;
      }
    }

    length = length / 1.05;
    h_value -= 45;
    s_value -= .05;
    //l_value = .5;
    a_value -= .05;
  }

  return png;
}
