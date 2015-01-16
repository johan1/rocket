#include "png_reader.h"

#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <png.h>
#include <cstdint>
#include <istream>

struct PngReadStructs {
	png_structp png_ptr;
	png_infop info_ptr;
	png_infop end_info;

	PngReadStructs() {
		png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, static_cast<png_voidp>(nullptr), nullptr, nullptr);
    	if (!png_ptr) {
        	throw std::runtime_error("Failed to create png read struct");
    	}

    	info_ptr = png_create_info_struct(png_ptr);
    	if (!info_ptr) {
        	throw std::runtime_error("Failed to create info struct");
    	}

    	end_info = png_create_info_struct(png_ptr);
    	if (!end_info) {
        	throw std::runtime_error("Failed to create end_info struct");
    	}
	}

	PngReadStructs(PngReadStructs const&) = delete;
	PngReadStructs& operator=(PngReadStructs const&) = delete;

	~PngReadStructs() {
    	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
	}
};

struct PngWriteStructs {
	png_structp png_ptr;
	png_infop info_ptr;

	PngWriteStructs() {
		png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, static_cast<png_voidp>(nullptr), nullptr, nullptr);
    	if (!png_ptr) {
        	throw std::runtime_error("Failed to create png read struct");
    	}

    	info_ptr = png_create_info_struct(png_ptr);
    	if (!info_ptr) {
        	throw std::runtime_error("Failed to create info struct");
    	}
	}

	PngWriteStructs(PngWriteStructs const&) = delete;
	PngWriteStructs& operator=(PngWriteStructs const&) = delete;

	~PngWriteStructs() {
		png_destroy_write_struct(&png_ptr, &info_ptr);
	}
};

struct PngImageData {
	png_bytep* image_row_pointers;
	unsigned long height;

	PngImageData(unsigned long row_bytes, unsigned long height) : height(height) {
		image_row_pointers = new png_bytep[height];
		for(uint32_t i = 0; i < height; ++i) {
			image_row_pointers[i] = new png_byte[row_bytes];
		}
	}

	PngImageData(PngImageData const&) = delete;

	PngImageData& operator=(PngImageData const&) = delete;

	PngImageData(PngImageData &&other) { 
		this->image_row_pointers = other.image_row_pointers;
		this->height = other.height;
		other.image_row_pointers = nullptr;
	}

	PngImageData& operator=(PngImageData &&other) {
		this->image_row_pointers = other.image_row_pointers;
		this->height = other.height;
		other.image_row_pointers = nullptr;

		return *this;
	}

	~PngImageData() {
		if (image_row_pointers != nullptr) {
			for(unsigned int i = 0; i < height; ++i) {
				delete[] image_row_pointers[i];
			}
			delete[] image_row_pointers;
		}
	}
};

static void read(png_structp png_ptr, png_bytep data, png_size_t length) {
    //Here we get our IO pointer back from the read struct.
    //This is the parameter we passed to the png_set_read_fn() function.
    //Our std::istream pointer.
    std::istream *is = reinterpret_cast<std::istream*>(png_get_io_ptr(png_ptr));

    //Cast the pointer to std::istream* and read 'length' bytes into 'data'
    is->read(reinterpret_cast<char*>(data), static_cast<long>(length));
}

static void write(png_structp png_ptr, png_bytep data, png_size_t length) {
    //Here we get our IO pointer back from the read struct.
    //This is the parameter we passed to the png_set_read_fn() function.
    //Our std::istream pointer.
    std::ostream *os = reinterpret_cast<std::ostream*>(png_get_io_ptr(png_ptr));

    //Cast the pointer to std::istream* and read 'length' bytes into 'data'
    os->write(reinterpret_cast<char*>(data), static_cast<long>(length));
}

static void flush(png_structp png_ptr) {
    //Here we get our IO pointer back from the read struct.
    //This is the parameter we passed to the png_set_read_fn() function.
    //Our std::istream pointer.
    std::ostream *os = reinterpret_cast<std::ostream*>(png_get_io_ptr(png_ptr));
	os->flush();
}

void writeBitmapToPng(std::ostream &os, Bitmap<RgbaPixel> const& bitmap) {
	PngWriteStructs pws;

	// Read the image, TODO: We need to take special care for paletted png's and gray scale...
	if (setjmp(png_jmpbuf(pws.png_ptr))) {
		throw std::runtime_error("Failed to write png image");
	}

    // Set up custom write function.
    png_set_write_fn(pws.png_ptr, reinterpret_cast<png_voidp>(&os), &write, &flush);

	png_set_IHDR(pws.png_ptr, pws.info_ptr, bitmap.width(), bitmap.height(), 8, PNG_COLOR_TYPE_RGBA,
			PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	png_write_info(pws.png_ptr, pws.info_ptr);

	std::vector<uint8_t> rowData = std::vector<uint8_t>(bitmap.width() * 4 * sizeof(uint8_t));
	for (uint32_t y = 0; y < bitmap.height(); ++y) {
		for (uint32_t x = 0; x < bitmap.width(); ++x) {
			rowData[x * 4] = bitmap.getPixel(x, y).red();
			rowData[x * 4 +1] = bitmap.getPixel(x, y).green();
			rowData[x * 4 +2] = bitmap.getPixel(x, y).blue();
			rowData[x * 4 +3] = bitmap.getPixel(x, y).alpha();
		}

		png_write_row(pws.png_ptr, reinterpret_cast<png_bytep>(rowData.data()));
	}
}

boost::optional<Bitmap<RgbaPixel>> readBitmapFromPng(std::istream &is) {
    // Check if istream is a png file.
    uint8_t header[8];
    is.read(reinterpret_cast<char*>(header), 8);
    if (is.gcount() != 8) {
		return boost::optional<Bitmap<RgbaPixel>>();
    }
    bool is_png = !png_sig_cmp(header, 0, 8);
    if (!is_png) {
		return boost::optional<Bitmap<RgbaPixel>>();
    }

	PngReadStructs prs;

    // Set jmp, hopefully this is the correct way to hangle setjmp
    if (setjmp(png_jmpbuf(prs.png_ptr))) {
        throw std::runtime_error("Failed to read png info");    	
	}

    // Set up custom read function.
    png_set_read_fn(prs.png_ptr, reinterpret_cast<png_voidp>(&is), &read);

    //Set the amount of bytes we've already read (signature 8 bytes).
    png_set_sig_bytes(prs.png_ptr, 8);

    // Reading png info
    png_read_info(prs.png_ptr, prs.info_ptr);

	// Read the image, TODO: We need to take special care for paletted png's and gray scale...
	if (setjmp(png_jmpbuf(prs.png_ptr))) {
		throw std::runtime_error("Failed to read png image");
	}

	auto w = png_get_image_width(prs.png_ptr, prs.info_ptr);
	auto h = png_get_image_height(prs.png_ptr, prs.info_ptr);
	Bitmap<RgbaPixel> bitmap = Bitmap<RgbaPixel>(w, h);

	auto bd = png_get_bit_depth(prs.png_ptr, prs.info_ptr);
	auto ch = png_get_channels(prs.png_ptr, prs.info_ptr);
	auto ct = png_get_color_type(prs.png_ptr, prs.info_ptr);

	auto row_bytes = png_get_rowbytes(prs.png_ptr, prs.info_ptr);

	PngImageData imageData  = PngImageData(row_bytes, h);

	std::size_t pixelSize = ch*bd/8;
	uint8_t pixelData[4]; // Assuming that pixel data is at most 32 bits.
	png_read_image(prs.png_ptr, imageData.image_row_pointers);

	for (uint32_t y = 0; y < h; ++y) {
		uint32_t offset = 0;
		for (uint32_t x = 0; x < w; ++x) {
			std::memcpy(pixelData, imageData.image_row_pointers[y] + offset, pixelSize);
			
			if (ct == PNG_COLOR_TYPE_RGB || ct == PNG_COLOR_TYPE_RGBA) {
				RgbaPixel &pixel = bitmap.getPixel(x, y);
				pixel.red() = pixelData[0];
				pixel.green() = pixelData[1];
				pixel.blue() = pixelData[2];
				if(ct == PNG_COLOR_TYPE_RGBA) {
					pixel.alpha() = pixelData[3];
				} else {
					pixel.alpha() = 255;
				}

				offset += pixelSize;
			} else {
				std::cout << "Unknown color type!!!! " << static_cast<int>(ct) << " " <<
						PNG_COLOR_TYPE_RGB << " " << PNG_COLOR_TYPE_RGBA << std::endl;
			}
		}
	}

	return bitmap;
}

