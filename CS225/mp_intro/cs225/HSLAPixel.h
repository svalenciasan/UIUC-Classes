/**
 * @file HSLAPixel.h
 *
 * @author CS 225: Data Structures
 */

#pragma once

namespace cs225 {
class HSLAPixel {
  public:
    HSLAPixel();
    HSLAPixel(double hue, double saturation, double luminance);
    HSLAPixel(double hue, double saturation, double luminance, double alpha);
    double h, s, l, a;
};
}//namespace cs225