#include "Zip.h"
#include "ResourceException.h"
#include "../util/Log.h"

#include <cstdint>
#include <istream>
#include <iostream>
#include <fstream>
#include <memory>
#include <cstring>
#include <stdexcept>
#include <unordered_map>
#include <zlib.h>

namespace rocket { namespace resource {

// TODO: Create zip file so we can check NONE

static constexpr unsigned int BUF_SIZE = 1024;

uint32_t const Zip::LocalFileHeader::SIGNATURE;

/* Zip Local File header.
 *  0 	4 	Local file header signature = 0x04034b50 (read as a little-endian number)
 *  4 	2 	Version needed to extract (minimum)
 *  6 	2 	General purpose bit flag
 *  8 	2 	Compression method
 * 10 	2 	File last modification time
 * 12 	2 	File last modification date
 * 14 	4 	CRC-32
 * 18 	4 	Compressed size
 * 22 	4 	Uncompressed size
 * 26 	2 	File name length (n)
 * 28 	2 	Extra field length (m)
 * 30 	n 	File name
 * 30+n 	m 	Extra field
 */
Zip::LocalFileHeader::LocalFileHeader(std::istream &inputStream) {
	inputStream.read(reinterpret_cast<char*>(&version), 2);
	inputStream.read(reinterpret_cast<char*>(&bitFlag), 2);
	inputStream.read(reinterpret_cast<char*>(&compressionMethod), 2);
	inputStream.read(reinterpret_cast<char*>(&modificationTime), 2);
	inputStream.read(reinterpret_cast<char*>(&modificationDate), 2);
	inputStream.read(reinterpret_cast<char*>(&crc), 4);
	inputStream.read(reinterpret_cast<char*>(&compressedSize), 4);
	inputStream.read(reinterpret_cast<char*>(&uncompressedSize), 4);
	inputStream.read(reinterpret_cast<char*>(&nameLength), 2);
	inputStream.read(reinterpret_cast<char*>(&extraFieldLength), 2);

	fileName = new char[nameLength + 1];
	inputStream.read(fileName, nameLength);
	fileName[nameLength] = '\0';

	extraField = new char[extraFieldLength + 1];
	inputStream.read(extraField, extraFieldLength);
	extraField[extraFieldLength] = '\0';

	dataStartPos = inputStream.tellg();
}

Zip::LocalFileHeader::~LocalFileHeader() {
	delete[] fileName;
	delete[] extraField;
}

std::ostream& operator<<(std::ostream& o, Zip::LocalFileHeader &fh) {
	if (fh.compressionMethod == Zip::LocalFileHeader::CompressionMethod::NONE) {
		o << "NONE";
	} else if (fh.compressionMethod == Zip::LocalFileHeader::CompressionMethod::DEFLATE) {
		o << "DEFLATE";
	} else {
		o << "UNKNOWN";
	}

	return o << " " << fh.compressedSize << " " << fh.uncompressedSize << " " << fh.fileName;
}

Zip::DeflateStreamBuffer::DeflateStreamBuffer(std::shared_ptr<std::istream> zipStream, std::shared_ptr<Zip::LocalFileHeader> fileHeader) : 
		zipStream{zipStream},
		fileHeader{fileHeader} {
	setg(buf+BUF_SIZE, buf+BUF_SIZE, buf+BUF_SIZE);
	zipStream->seekg(fileHeader->dataStartPos);

	/* allocate inflate state */
    zstr.zalloc = Z_NULL;
    zstr.zfree = Z_NULL;
    zstr.opaque = Z_NULL;
    zstr.avail_in = 0;
    zstr.next_in = Z_NULL;

    auto ret = inflateInit2(&zstr, -MAX_WBITS);
    if (ret != Z_OK) {
		throw ResourceException(ResourceException::Type::FAILED_TO_INIT_ZLIB, "Failed to initialize inflate");
	}
}

Zip::DeflateStreamBuffer::int_type Zip::DeflateStreamBuffer::underflow() {
	unsigned int contentLeft = fileHeader->dataStartPos + fileHeader->compressedSize -
			static_cast<unsigned int>(zipStream->tellg());

	if (contentLeft == 0 && zstr.avail_in <= 0) {
		return traits_type::eof();
	}

	// We need to fill buffer with content.
	if (gptr() >= egptr()) {
		fillBuffer(contentLeft);
		setg(buf, buf, &buf[BUF_SIZE] - zstr.avail_out);
	}

	return traits_type::to_int_type(*gptr());
}

void Zip::DeflateStreamBuffer::fillBuffer(unsigned int contentLeft) {
	zstr.avail_out = BUF_SIZE;
	zstr.next_out = reinterpret_cast<unsigned char*>(buf);

	do {
		// Fill zstr.next_in if empty
		if (zstr.avail_in <= 0) {
			if (contentLeft >= BUF_SIZE) {
				zipStream->read(zbuf, BUF_SIZE);
				zstr.avail_in = BUF_SIZE;
			} else if(contentLeft > 0) {
				zipStream->read(zbuf, contentLeft);
				zstr.avail_in += contentLeft;
			}
			zstr.next_in = reinterpret_cast<unsigned char*>(zbuf);
		}

		auto ret = inflate(&zstr, Z_SYNC_FLUSH);

		if(ret == Z_STREAM_END) {
			break;
		} else if(ret != Z_OK){
			std::cout << "Inflation error" << std::endl;
			throw ResourceException(ResourceException::Type::INFLATION_ERROR, "Inflation error");
		}
	} while(zstr.avail_out != 0); // Ok, we have more to read...
}

Zip::DeflateStreamBuffer::~DeflateStreamBuffer() {
	(void)inflateEnd(&zstr);
}

Zip::NoneStreamBuffer::NoneStreamBuffer(std::shared_ptr<std::istream> zipStream, std::shared_ptr<LocalFileHeader> fileHeader) : 
		zipStream{zipStream},
		fileHeader{fileHeader} {
	setg(buf+BUF_SIZE, buf+BUF_SIZE, buf+BUF_SIZE);
	zipStream->seekg(fileHeader->dataStartPos);
}

Zip::NoneStreamBuffer::int_type Zip::NoneStreamBuffer::underflow() {
	unsigned int contentLeft = fileHeader->dataStartPos + fileHeader->compressedSize -
			static_cast<unsigned int>(zipStream->tellg());

	if (contentLeft == 0) {
		return traits_type::eof();
	}

	// We need to fill buffer with content.
	if (gptr() >= egptr()) {
		if(contentLeft >= BUF_SIZE) {
			zipStream->read(buf, BUF_SIZE);
		} else {
			zipStream->read(buf, contentLeft);
		}

		setg(&buf[0], &buf[0], contentLeft >= BUF_SIZE ? &buf[BUF_SIZE] : &buf[contentLeft]);
	}

	return traits_type::to_int_type(*gptr());
}

Zip::Zip(std::string const& zipPath) : 
		zipPath {zipPath} {
	std::shared_ptr<std::istream> zipStream = std::make_shared<std::fstream>(zipPath,
			std::ios_base::in | std::ios_base::binary);
	if (!zipStream->good()) {
		throw ResourceException(ResourceException::Type::FAILED_TO_OPEN_PATH, 
				"Failed to open zip file.");
	}

	uint32_t packageSignature;
	while (!zipStream->eof()) {
		zipStream->read(reinterpret_cast<char*>(&packageSignature), 4);
		switch(packageSignature) {
		case LocalFileHeader::SIGNATURE: {
			std::shared_ptr<LocalFileHeader> lfh = std::make_shared<LocalFileHeader>(*zipStream);
			files[lfh->fileName] = lfh;

			// Seek to after the data.
			// DataDescriptor dd {*zipStream};
			zipStream->seekg(lfh->compressedSize, std::ios_base::cur);
		} break;
		case 0x02014b50: // Central directory file header
			break;
		case 0x06054b50: // End of directory record
			break;
		default:
			break;
			// Unexpected?
		}
	}
}

void Zip::list() const {
	for (auto file : files) {
		LOGD(boost::format("%s\n") % *(file.second));
//		std::cout << *(file.second) << std::endl;
	}
}

bool Zip::contains(std::string const& fileName) const {
	if (files.find(fileName) != files.end()) {
		return true;
	} 

	return false;
}

std::shared_ptr<std::istream> Zip::openFile(const char* fileName) {
	std::shared_ptr<std::istream> zipStream = std::make_shared<std::fstream>(zipPath,
			std::ios_base::in | std::ios_base::binary);
	if (!zipStream->good()) {
		throw ResourceException(ResourceException::Type::FAILED_TO_OPEN_PATH, 
				"Failed to open zip file.");
	}

	auto it = files.find(fileName);
	if (it == files.end()) {
		throw ResourceException(ResourceException::Type::FILE_NOT_FOUND, fileName);
	}

	auto lfr = it->second;
	zipStream->seekg(lfr->dataStartPos);
	if (lfr->compressionMethod == LocalFileHeader::CompressionMethod::NONE) {
		return createStreamBuffer<NoneStreamBuffer>(zipStream, lfr);
	} else if(lfr->compressionMethod == LocalFileHeader::CompressionMethod::DEFLATE) {
		return createStreamBuffer<DeflateStreamBuffer>(zipStream, lfr);
	} else {
		throw ResourceException(ResourceException::Type::UNSUPPORTED_COMPRESSION_METHOD,
				"Unsupported compression method");
	}
}

}}
