#ifndef _BITMAP_H_
#define _BITMAP_H_

#include <algorithm>
#include <cassert>
#include <ostream>
#include <stdint.h>
#include <string>

namespace rocket { namespace resource { namespace image {

inline
uint32_t upperPow2Bound(uint32_t value) {
	uint32_t result;
	for (result = 1; result < value; result *= 2);
	return result;
}

struct RGBAPixel {
	RGBAPixel(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) :
			red {red}, green {green}, blue {blue}, alpha {alpha} {}

	RGBAPixel() : RGBAPixel(0, 0, 0, 0) {}

	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t alpha;
};

struct RGBPixel {
	RGBPixel(uint8_t red, uint8_t green, uint8_t blue) :
			red {red}, green {green}, blue {blue} {}

	RGBPixel() : RGBPixel(0, 0, 0) {}

	uint8_t red;
	uint8_t green;
	uint8_t blue;
};
  
struct APixel {
	APixel(uint8_t alpha) : alpha {alpha} {}
	APixel() : APixel(0) {}

	uint8_t alpha;
};

inline
std::ostream& operator<<(std::ostream &os, RGBPixel const& pixel) {
	return os << "(" << static_cast<int>(pixel.red) << ", " << static_cast<int>(pixel.green) <<
			", " << static_cast<int>(pixel.blue) << ")";
}

inline
std::ostream& operator<<(std::ostream &os, RGBAPixel const& pixel) {
	return os << "(" << static_cast<int>(pixel.red) << ", " << static_cast<int>(pixel.green) <<
			", " <<	static_cast<int>(pixel.blue) << ", " << static_cast<int>(pixel.alpha) << ")";
}

inline
std::ostream& operator<<(std::ostream &os, APixel const& pixel) {
	return os << static_cast<int>(pixel.alpha);
}

template <typename PixelType>
class Bitmap {
public:
	typedef PixelType Pixel;

	Bitmap(uint32_t contentWidth, uint32_t contentHeight); 

	Pixel& getPixel(unsigned int x, unsigned int y);
	Pixel const& getPixel(unsigned int x, unsigned int y) const;

	uint32_t getWidth() const;
	uint32_t getHeight() const;
	uint32_t getContentWidth() const;
	uint32_t getContentHeight() const;

	// Oh yes it's ugly however we need a handle to be able to read into textures.
	void *getRawData(); // TODO: Don't know if this is necessary. I don't want this.
	void const* getRawData() const;

private:
	uint32_t width;
	uint32_t height;
	uint32_t contentWidth;
	uint32_t contentHeight;

	std::vector<Pixel> data;
};

template <typename PixelType>
inline
std::ostream& operator<<(std::ostream &os, Bitmap<PixelType> const& bitmap) {
	for (uint32_t row = 0; row < bitmap.getContentHeight(); ++row) {
		for (uint32_t col = 0; col < bitmap.getContentWidth(); ++col) {
			os << bitmap.getPixel(col, row) << " ";
		}
		os << '\n';
	}

	return os;
};

template <typename PixelType>
inline
Bitmap<PixelType>::Bitmap(uint32_t contentWidth, uint32_t contentHeight) : contentWidth{contentWidth}, contentHeight{contentHeight} {
	width = upperPow2Bound(contentWidth);
	height = upperPow2Bound(contentHeight);

	data.assign(width * height, PixelType());
}

template <typename PixelType>
inline
PixelType& Bitmap<PixelType>::getPixel(unsigned int x, unsigned int y) {
	assert(y < contentHeight);
	assert(x < contentWidth);

	y += height-contentHeight;
	return data[(height-1-y)*width + x];
}

template <typename PixelType>
inline
PixelType const& Bitmap<PixelType>::getPixel(unsigned int x, unsigned int y) const {
	assert(y < contentHeight);
	assert(x < contentWidth);

	y += height-contentHeight;
	return data[(height-1-y)*width + x];
}

template <typename PixelType>
inline 
uint32_t Bitmap<PixelType>::getWidth() const {
	return width;
}

template <typename PixelType>
inline 
uint32_t Bitmap<PixelType>::getHeight() const {
	return height;
}

template <typename PixelType>
inline
uint32_t Bitmap<PixelType>::getContentWidth() const {
	return contentWidth;
}

template <typename PixelType>
inline
uint32_t Bitmap<PixelType>::getContentHeight() const {
	return contentHeight;
}

template <typename PixelType>
inline void *Bitmap<PixelType>::getRawData() {
	return static_cast<void*>(&(data[0])); // Should point to the first pixel
}

template <typename PixelType>
inline
void const* Bitmap<PixelType>::getRawData() const {
	return static_cast<void const*>(&(data[0]));
}

}}}

#endif // _BITMAP_H_
