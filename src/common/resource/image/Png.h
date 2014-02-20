#ifndef _PNG_H_
#define _PNG_H_

#include <iostream>
#include <memory>

#include <png.h>

#include "Bitmap.h"

namespace rocket { namespace resource { namespace image {

class Png {
public:
    enum class ColorType : uint8_t {
        Gray      = PNG_COLOR_TYPE_GRAY,
        Palette   = PNG_COLOR_TYPE_PALETTE,
        Rgb       = PNG_COLOR_TYPE_RGB,
        RgbAlpha  = PNG_COLOR_TYPE_RGB_ALPHA,
        GrayAlpha = PNG_COLOR_TYPE_GRAY_ALPHA
    };

    Png(std::shared_ptr<std::istream> is);
    ~Png();

    uint32_t width();
    uint32_t height();
    uint32_t bitDepth();
    uint32_t channels();
    ColorType colorType();

	// std::shared_ptr<Bitmap> getBitmap();
	Bitmap<RGBAPixel>& getBitmap();
private:
    // shared_ptr<istream> &is;
    Png(Png const&) = delete;
    Png& operator=(Png const&) = delete;

    static void read(png_structp png_ptr, png_bytep data, png_size_t length);

	
    std::shared_ptr<std::istream> is;

	// Png api structs
    png_structp png_ptr;
    png_infop info_ptr;
    png_infop end_info;

	// Image data
	png_bytep* image_row_pointers;
	int image_row_count;

	std::unique_ptr<Bitmap<RGBAPixel>> bitmap;
};

// Inline methods

inline uint32_t Png::width() {
    return png_get_image_width(png_ptr, info_ptr);
}

inline uint32_t Png::height() {
    return png_get_image_height(png_ptr, info_ptr);
}

inline uint32_t Png::bitDepth() {
    return png_get_bit_depth(png_ptr, info_ptr);
}

inline uint32_t Png::channels() {
    return png_get_channels(png_ptr, info_ptr);
}

inline Png::ColorType Png::colorType() {
    return static_cast<Png::ColorType>(png_get_color_type(png_ptr, info_ptr));
}

inline Bitmap<RGBAPixel>& Png::getBitmap() {
	return *(bitmap.get());
}

}}
using namespace resource::image; // Collapse
}

#endif // _PNG_TEST_H_

