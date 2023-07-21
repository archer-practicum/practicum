#pragma once
#include "common.h"

class Texture {
public:
    explicit Texture(Image image)
        : _image(std::move(image)) {
    }

    Size GetSize() const {        
        return GetImageSize(_image);
    }

    char GetPixelColor(Point p) const {
        Size size = GetSize();
        if (p.x < size.width && p.y < size.height) {
            return _image.at(p.y).at(p.x);
        } else {
            return '.';
        }        
    }

private:
    Image _image;
    Size _size;
};