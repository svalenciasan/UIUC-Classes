#include "StickerSheet.h"

    StickerSheet::StickerSheet(const Image & picture, unsigned max) {
        base_ = picture;
        max_ = max;
        image_ptrs_.resize(max_, nullptr);
        coordinates_.resize(max_);
    }

    StickerSheet::StickerSheet(const StickerSheet &other) {
        copy(other);
    }

    StickerSheet::~StickerSheet () {
        destroy();
    }

    const StickerSheet & StickerSheet::operator=(const StickerSheet &other) {
        destroy();
        copy(other);
        return *this;
    }

    void StickerSheet::changeMaxStickers(unsigned max) {
        if (max < max_) {
            for (unsigned i = max_ - 1; i >= max; i--) {
                removeSticker(i);
                if (i == 0) {
                    break;
                }
            }
        }
        max_ = max;
        image_ptrs_.resize(max_, nullptr);
        coordinates_.resize(max_);
    }

    int StickerSheet::addSticker(Image &sticker, unsigned x, unsigned y) {
        for (unsigned i = 0; i < max_; i++) {
            if (image_ptrs_[i] == nullptr) {
                //Image* image_ptr = &sticker;
                Image* image_ptr = new Image(sticker);
                image_ptrs_[i] = image_ptr;

                pair<unsigned, unsigned> coordinate_pair(x, y);
                coordinates_[i] = coordinate_pair;

                return i;
            }
        }
        return -1;
    }

    bool StickerSheet::translate(unsigned index, unsigned x, unsigned y) {
        if (index >= max_ || image_ptrs_[index] == nullptr) {
            return false;
        }
        coordinates_[index].first = x;
        coordinates_[index].second = y;
        return true;
    }

    void StickerSheet::removeSticker(unsigned index) {
        if (index >= max_) {
            return;
        }
        delete image_ptrs_[index];
        image_ptrs_[index] = nullptr;
    }

    Image * StickerSheet::getSticker(unsigned index) {
        if (index >= max_) {
            return nullptr;
        }
        return image_ptrs_[index];
    }

    Image StickerSheet::render() const {
        Image output_image(base_);
        for (unsigned sticker_index = 0; sticker_index < max_; sticker_index++) {
            if (image_ptrs_[sticker_index] == nullptr) {
                continue;
            }
            //Iterating through sticker image pixels and copying them to the output image
            for (unsigned int x = 0; x < image_ptrs_[sticker_index]->width(); x++) {
                for (unsigned int y = 0; y < image_ptrs_[sticker_index]->height(); y++) {
                    HSLAPixel & sticker_pixel = image_ptrs_[sticker_index]->getPixel(x,y);
                    if ((coordinates_[sticker_index].first + x) >= output_image.width()) {
                        output_image.resize(output_image.width() + (coordinates_[sticker_index].first + x - output_image.width() + 1), output_image.height());
                    }
                    if ((coordinates_[sticker_index].second + y) >= output_image.height()) {
                        output_image.resize(output_image.width(), output_image.height() + (coordinates_[sticker_index].second + y - output_image.height() + 1));
                    }

                    HSLAPixel & output_pixel = output_image.getPixel(coordinates_[sticker_index].first + x, coordinates_[sticker_index].second + y);
                    if (sticker_pixel.a == 0) {
                        continue;
                    }
                    output_pixel.h = sticker_pixel.h;
                    output_pixel.s = sticker_pixel.s;
                    output_pixel.l = sticker_pixel.l;
                    output_pixel.a = sticker_pixel.a;
                }
            }
        }
        return output_image;
    }

    void StickerSheet::copy(const StickerSheet & other) {
        base_ = other.base_;
        max_ = other.max_;
        
        image_ptrs_.resize(max_, nullptr);
        coordinates_.resize(max_);

        for (unsigned i = 0; i < other.max_; i++) {
            if (other.image_ptrs_[i] == nullptr) {
                image_ptrs_[i] = nullptr;
                continue;
            }
            Image* new_image_ptr = new Image(*other.image_ptrs_[i]);
            image_ptrs_[i] = new_image_ptr;

            pair<unsigned, unsigned> new_coordinate(other.coordinates_[i]);
            coordinates_[i] = new_coordinate;
        }
    }

    void StickerSheet::destroy() {
        for (unsigned i = 0; i < max_; i++) {
            delete image_ptrs_[i];
            //image_ptrs_[i] = nullptr;
        }
        image_ptrs_.clear();
        coordinates_.clear();
    }