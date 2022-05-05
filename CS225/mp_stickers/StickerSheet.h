/**
 * @file StickerSheet.h
 * Contains your declaration of the interface for the StickerSheet class.
 */
#pragma once
#include <iostream>
#include <vector>
#include <utility>
#include "Image.h"

using std::vector;
using std::pair;
//using Image::Image;

class StickerSheet {
    public:
        StickerSheet (const Image &picture, unsigned max);
        StickerSheet (const StickerSheet &other);
        ~StickerSheet ();
        const StickerSheet & operator= (const StickerSheet &other);
        void changeMaxStickers (unsigned max);
        int addSticker (Image &sticker, unsigned x, unsigned y);
        bool translate (unsigned index, unsigned x, unsigned y);
        void removeSticker (unsigned index);
        Image * getSticker (unsigned index);
        Image render () const;
    private:
        Image base_;
        vector<Image*> image_ptrs_;
        vector<pair<unsigned, unsigned>> coordinates_;
        unsigned max_;

        void copy(const StickerSheet & other);
        void destroy();
};
