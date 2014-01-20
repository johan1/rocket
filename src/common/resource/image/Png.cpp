#include "Png.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>

#include <cstring>

#include <png.h>

namespace rocket { namespace resource { namespace image {

using namespace std;

Png::Png(shared_ptr<istream> is) :
		is(is), png_ptr{nullptr}, info_ptr{nullptr}, end_info{nullptr},
		image_row_pointers{nullptr}, image_row_count{0}, bitmap{nullptr} {

    // Check if istream is a png file.
    uint8_t header[8];
    is->read(reinterpret_cast<char*>(header), 8);
    if (is->gcount() != 8) {
        throw runtime_error("Failed to read png header");
    }
    bool is_png = !png_sig_cmp(header, 0, 8);
    if (!is_png) {
        throw runtime_error("Not a png file");
    }

	// Parse information structs
    try {
        png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp) NULL, NULL, NULL);
        if (!png_ptr) {
            throw runtime_error("Failed to create png read struct");
        }

        info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr) {
            throw runtime_error("Failed to create info struct");
        }

        end_info = png_create_info_struct(png_ptr);
        if (!end_info) {
            throw runtime_error("Failed to create end_info struct");
        }

        // Set jmp, hopefully this is the correct way to hangle setjmp
        if (setjmp(png_jmpbuf(png_ptr))) {
            throw runtime_error("Failed to read png info");
        }

        // Set up custom read function.
        png_set_read_fn(png_ptr, (png_voidp) is.get(), &read);

        //Set the amount of bytes we've already read (signature 8 bytes).
        png_set_sig_bytes(png_ptr, 8);

        // Reading png info
        png_read_info(png_ptr, info_ptr);

		// Read the image, TODO: We need to take special care for paletted png's and gray scale...
		if (setjmp(png_jmpbuf(png_ptr))) {
			throw runtime_error("Failed to read png image");
		}

		uint32_t w = width();
		uint32_t h = height();
		uint32_t bd = bitDepth();
		uint32_t ch = channels();
		ColorType ct = colorType();

		std::cout << "w: " << w << ", h: " << h << ", bd: " << bd << ", ch: " << ch << std::endl;

		bitmap = std::unique_ptr<Bitmap<RGBAPixel>>{ new Bitmap<RGBAPixel>(w, h) };
		
		// TODO: Leaks, if we have an out of memory error, yes...
		// Investigate what I mean by the above statement, I wrote it some time ago and forgot...
		int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
		image_row_pointers = new png_bytep[h];
		image_row_count = h;
		for(uint32_t i = 0; i < h; ++i) {
			image_row_pointers[i] = new png_byte[row_bytes];
		}

		int pixelSize = ch*bd/8;
		uint8_t pixelData[4]; // Assuming that pixel data is at most 32 bits.
		png_read_image(png_ptr, image_row_pointers);
		for (uint32_t y = 0; y < h; ++y) {
			uint32_t offset = 0;
			for (uint32_t x = 0; x < w; ++x) {
				std::memcpy(pixelData, image_row_pointers[y] + offset, pixelSize);
				
				if(ct == ColorType::Rgb || ct == ColorType::RgbAlpha) {
					RGBAPixel &pixel = bitmap->getPixel(x, y);
					pixel.red = pixelData[0];
					pixel.green = pixelData[1];
					pixel.blue = pixelData[2];
					if(ct == ColorType::RgbAlpha) {
						pixel.alpha = pixelData[3];
					} else {
						pixel.alpha = 255;
					}

					offset += pixelSize;
				}
			}
		}
    } catch (...) { // Catch, clean-up and rethrow
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        throw;
    }
}

Png::~Png() {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
	if (image_row_pointers != nullptr) {
		for(int i = 0; i < image_row_count; ++i) {
			delete[] image_row_pointers[i];
		}
		delete[] image_row_pointers;
	}
}

void Png::read(png_structp png_ptr, png_bytep data, png_size_t length) {
    //Here we get our IO pointer back from the read struct.
    //This is the parameter we passed to the png_set_read_fn() function.
    //Our std::istream pointer.
    std::istream *is = reinterpret_cast<std::istream*>(png_get_io_ptr(png_ptr));

    //Cast the pointer to std::istream* and read 'length' bytes into 'data'
    is->read(reinterpret_cast<char*>(data), length);

	// std::cout << "read chunk of " << is->gcount() << " bytes, good=: "  << is->good() << std::endl;
}

}}}
