/**
 * @file HSLAPixel.cpp
 * Implementation of the HSLAPixel class for use in with the PNG library.
 *
 * @author CS 225: Data Structures
 */

#include "HSLAPixel.h"

namespace cs225 {
    HSLAPixel::HSLAPixel() {
        this->h = 0;
        this->s = 0;
        this->l = 1;
        this->a = 1;
    }

    HSLAPixel::HSLAPixel(double hue, double saturation, double luminance) {
        this->h = hue;
        this->s = saturation;
        this->l = luminance;
        this->a = 1;
    }

    HSLAPixel::HSLAPixel(double hue, double saturation, double luminance, double alpha) {
        this->h = hue;
        this->s = saturation;
        this->l = luminance;
        this->a = alpha;
    }
}//namespace cs225