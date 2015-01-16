#ifndef _ROCKET_PNG_READER_H_
#define _ROCKET_PNG_READER_H_

#include "bitmap.h"

#include <istream>
#include <cstdint>
#include <boost/optional.hpp>

#include <png.h>

void writeBitmapToPng(std::ostream &os, Bitmap<RgbaPixel> const& bitmap);

boost::optional<Bitmap<RgbaPixel>> readBitmapFromPng(std::istream &is);

#endif /* _ROCKET_PNG_READER_H_ */
