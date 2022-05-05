#include <cmath>
#include <iterator>
#include <iostream>
#include <vector>

#include "../cs225/HSLAPixel.h"
#include "../cs225/PNG.h"
#include "../Point.h"

#include "ImageTraversal.h"

/**
 * Calculates a metric for the difference between two pixels, used to
 * calculate if a pixel is within a tolerance.
 *
 * @param p1 First pixel
 * @param p2 Second pixel
 * @return the difference between two HSLAPixels
 */
double ImageTraversal::calculateDelta(const HSLAPixel & p1, const HSLAPixel & p2) {
  double h = fabs(p1.h - p2.h);
  double s = p1.s - p2.s;
  double l = p1.l - p2.l;

  // Handle the case where we found the bigger angle between two hues:
  if (h > 180) { h = 360 - h; }
  h /= 360;

  return sqrt( (h*h) + (s*s) + (l*l) );
}

bool ImageTraversal::Iterator::isInBounds(Point point) const{
  return (point.x < png_.width() && point.y < png_.height());
}

bool ImageTraversal::Iterator::isInTolerance(Point point) const{
  return calculateDelta(png_.getPixel(start_.x, start_.y), png_.getPixel(point.x, point.y)) <= tolerance_;
}

bool ImageTraversal::Iterator::isVisited(Point point) const{
  //return visited_.count(point) > 0;
  //return visited_[point];
  return visited_[point.y][point.x];
}

bool ImageTraversal::Iterator::isValid(Point point) const{
  if (isInBounds(point)) {
    return isInTolerance(point) * (!isVisited(point));
  }
  return false;
}

/**
 * Default iterator constructor.
 */
ImageTraversal::Iterator::Iterator() {
  /** @todo [Part 1] */
  traversal_ = nullptr;
}

ImageTraversal::Iterator::Iterator(PNG png, Point start, double tolerance, ImageTraversal* traversal) {
  png_ = png;
  start_ = start;
  curr_ = start;
  tolerance_ = tolerance;
  traversal_ = traversal;
  //visited_[curr_] = true;
  visited_.resize(png_.height(), vector<bool>(png_.width()));
  visited_[curr_.y][curr_.x] = true;

  // for (unsigned int i = 0; i < visited_.size(); ++i)
  // {
  //   for (unsigned int j = 0; j < visited_[i].size(); ++j)
  //   {
  //     std::cout << visited_[i][j];
  //   }
  //   std::cout << std::endl;
  // }
  // std::cout << "END" << std::endl;
}

/**
 * Iterator increment opreator.
 *
 * Advances the traversal of the image.
 */
ImageTraversal::Iterator & ImageTraversal::Iterator::operator++() {
  /** @todo [Part 1] */
  Point right(curr_.x + 1, curr_.y);
  if (isValid(right)) {
    traversal_->add(right);
  }
  Point below(curr_.x, curr_.y + 1);
  if (isValid(below)) {
    traversal_->add(below);
  }
  Point left(curr_.x - 1, curr_.y);
  if (isValid(left)) {
    traversal_->add(left);
  }
  Point above(curr_.x, curr_.y - 1);
  if (isValid(above)) {
    traversal_->add(above);
  }
 
  
  while(!traversal_->empty() && !isValid(traversal_->peek())) {
    traversal_->pop();
  }

  if (!traversal_->empty()) {
    curr_ = traversal_->pop();
    visited_[curr_.y][curr_.x] = true;
  }
  return *this;
}

/**
 * Iterator accessor opreator.
 *
 * Accesses the current Point in the ImageTraversal.
 */
Point ImageTraversal::Iterator::operator*() {
  /** @todo [Part 1] */
  return curr_;
}

/**
 * Iterator inequality operator.
 *
 * Determines if two iterators are not equal.
 */
bool ImageTraversal::Iterator::operator!=(const ImageTraversal::Iterator &other) {
  /** @todo [Part 1] */
  return !(traversal_->empty() && other.traversal_ == nullptr);
}

