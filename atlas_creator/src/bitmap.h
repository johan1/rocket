#ifndef _ROCKET_BITMAP_H_
#define _ROCKET_BITMAP_H_

#include <cstdint>
#include <vector>
#include <rocket/TupleTyping.h>

DEFINE_TUPLE_WRAPPER(RgbaPixel, red, uint8_t, green, uint8_t, blue, uint8_t, alpha, uint8_t);
DEFINE_TUPLE_WRAPPER(RgbPixel, red, uint8_t, green, uint8_t, blue, uint8_t);

template <typename PixelType>
class Bitmap {
public:
	Bitmap() : data(), w(0), h(0) {}
	Bitmap(uint32_t w, uint32_t h) : data(w*h), w(w), h(h) {}

	PixelType& getPixel(uint32_t x, uint32_t y) {
		return data[y*w + x];
	}

	PixelType const& getPixel(uint32_t x, uint32_t y) const {
		return data[y*w + x];
	}

	uint32_t width() const { return w; }
	uint32_t height() const { return h; }

private:
	std::vector<PixelType> data;
	uint32_t w;
	uint32_t h;
};

#endif /* _ROCKET_BITMAP_H_ */
