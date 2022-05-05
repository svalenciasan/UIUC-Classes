#include "Image.h"

    void Image::lighten() {
        for (unsigned int x = 0; x < width(); x++) {
            for (unsigned int y = 0; y < height(); y++) {
                HSLAPixel & pixel = getPixel(x,y);
                pixel.l += 0.1;
                if (pixel.l > 1) {
                    pixel.l = 1;
                }
            }
        }
    }
    void Image::lighten(double amount) {
        for (unsigned int x = 0; x < width(); x++) {
            for (unsigned int y = 0; y < height(); y++) {
                HSLAPixel & pixel = getPixel(x,y);
                pixel.l += amount;
                if (pixel.l > 1) {
                    pixel.l = 1;
                }
            }
        }
    }
    void Image::darken() {
        for (unsigned int x = 0; x < width(); x++) {
            for (unsigned int y = 0; y < height(); y++) {
                HSLAPixel & pixel = getPixel(x,y);
                pixel.l -= 0.1;
                if (pixel.l < 0) {
                    pixel.l = 0;
                }
            }
        }
    }
    void Image::darken(double amount) {
        for (unsigned int x = 0; x < width(); x++) {
            for (unsigned int y = 0; y < height(); y++) {
                HSLAPixel & pixel = getPixel(x,y);
                pixel.l -= amount;
                if (pixel.l < 0) {
                    pixel.l = 0;
                }
            }
        }
    }
    void Image::saturate() {
        for (unsigned int x = 0; x < width(); x++) {
            for (unsigned int y = 0; y < height(); y++) {
                HSLAPixel & pixel = getPixel(x,y);
                pixel.s += 0.1;
                if (pixel.s > 1) {
                    pixel.s = 1;
                }
            }
        }
    }
    void Image::saturate(double amount) {
        for (unsigned int x = 0; x < width(); x++) {
            for (unsigned int y = 0; y < height(); y++) {
                HSLAPixel & pixel = getPixel(x,y);
                pixel.s += amount;
                if (pixel.s > 1) {
                    pixel.s = 1;
                }
            }
        }
    }
    void Image::desaturate() {
        for (unsigned int x = 0; x < width(); x++) {
            for (unsigned int y = 0; y < height(); y++) {
                HSLAPixel & pixel = getPixel(x,y);
                pixel.s -= 0.1;
                if (pixel.s < 0) {
                    pixel.s = 0;
                }
            }
        }
    }
    void Image::desaturate(double amount) {
        for (unsigned int x = 0; x < width(); x++) {
            for (unsigned int y = 0; y < height(); y++) {
                HSLAPixel & pixel = getPixel(x,y);
                pixel.s -= amount;
                if (pixel.s < 0) {
                    pixel.s = 0;
                }
            }
        }
    }
    void Image::grayscale() {
        for (unsigned x = 0; x < width(); x++) {
            for (unsigned y = 0; y < height(); y++) {
                HSLAPixel & pixel = getPixel(x, y);
                pixel.s = 0;
            }
        }
    }
    void Image::rotateColor(double degrees) {
        for (unsigned int x = 0; x < width(); x++) {
            for (unsigned int y = 0; y < height(); y++) {
                HSLAPixel & pixel = getPixel(x,y);
                pixel.h += degrees;
                //pixel.h = std::abs(pixel.h);
                while (pixel.h < 0) {
                    pixel.h = 360 + pixel.h;
                }
                if (pixel.h > 360) {
                    int factor = pixel.h / 360;
                    pixel.h = pixel.h - (factor * 360);
                    if (pixel.h == 0) {
                        pixel.h = 360;
                    }
                }
            }
        }
    }
    void Image::illinify() {
        double orange_hue = 11;
        double blue_hue = 216;

        for (unsigned x = 0; x < width(); x++) {
            for (unsigned y = 0; y < height(); y++) {
                HSLAPixel & pixel = getPixel(x, y);

                double distance_to_orange = std::abs(pixel.h - orange_hue);
                if (distance_to_orange > 180) {
                    distance_to_orange = std::abs((360 - pixel.h) - orange_hue);
                }

                double distance_to_blue = std::abs(pixel.h - blue_hue);
                if (distance_to_blue > 180) {
                    distance_to_blue = std::abs((360 - pixel.h) - blue_hue);
                }

                if (distance_to_orange < distance_to_blue) {
                    pixel.h = orange_hue;
                } else {
                    pixel.h = blue_hue;
                }
            }
        }
    }
    void Image::scale(double factor) {
        int new_width = factor * width();
        int new_height = factor * height();

        HSLAPixel * temp = new HSLAPixel[new_width * new_height];

        double x_ratio = width()/(double)new_width ;
        double y_ratio = height()/(double)new_height ;

        double px, py ; 
        for (int i = 0; i < new_height; i++) {
            for (int j = 0; j < new_width; j++) {
                px = floor(j * x_ratio);
                py = floor(i * y_ratio);
                temp[(i*new_width)+j] = getPixel((int)px, (int)py);
            }
        }

        resize(new_width, new_height);
        for (unsigned x = 0; x < width(); x++) {
            for (unsigned y = 0; y < height(); y++) {
                HSLAPixel & pixel = getPixel(x,y);
                pixel = temp[x + (y * width())];
            }
        }
        delete[] temp;
    }
    void Image::scale(unsigned w, unsigned h) {
        double factor_width = (double)w / width();
        double factor_height = (double)h / height();
        //If scaling down
        // if (w < width() || h < height()) {
        //     if (factor_width > factor_height) {
        //         scale(factor_width);
        //     } else {
        //         scale(factor_height);
        //     }
        //     return;
        // }
        //If scaling up
        if (factor_width > factor_height) {
           scale(factor_height);
        } else {
           scale(factor_width);
        }
    }