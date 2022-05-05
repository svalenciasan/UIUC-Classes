#include "../cs225/HSLAPixel.h"
#include "../Point.h"

#include "ColorPicker.h"
#include "MyColorPicker.h"

using namespace cs225;

MyColorPicker::MyColorPicker() { 
  h_ = 0;
  s_ = 0;
  l_ = 0;
  a_ = 0;
}

/**
 * Picks the color for pixel (x, y).
 * Using your own algorithm
 */
HSLAPixel MyColorPicker::getColor(unsigned x, unsigned y) {
  /* @todo [Part 3] */
  HSLAPixel pixel(h_, s_, l_, a_);
  h_ += 0.5;
  s_ += 0.2;
  l_ += 0.2;
  a_ += 0.2;

  if (h_ >= 360) {
    h_ -= 1;
  }
  if (s_ >= 1) {
    s_ -= 1;
  }
  if (l_ >= 1) {
    l_ -= 1;
  }
  if (a_ >= 1) {
    a_ -= 1;
  }

  return pixel;
}
