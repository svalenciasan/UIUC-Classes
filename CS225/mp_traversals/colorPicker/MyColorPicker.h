#pragma once

#include "ColorPicker.h"
#include "../cs225/HSLAPixel.h"
#include "../Point.h"

using namespace cs225;

/**
 * A color picker class using your own color picking algorithm
 */
class MyColorPicker : public ColorPicker {
public:
  MyColorPicker();
  HSLAPixel getColor(unsigned x, unsigned y);

private:
  double h_;
  double s_;
  double l_;
  double a_;
};
