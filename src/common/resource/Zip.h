#ifndef _ZIP_H_
#define _ZIP_H_

#include <cstdint>
#include <exception>
#include <istream>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include <zlib.h>

namespace rocket { namespace resource {
class Zip {
public:
	class LocalFileHeader {
	public:
		static uint32_t const SIGNATURE { 0x04034b50 };
		enum class CompressionMethod : uint16_t {
			NONE = 0,
			DEFLATE = 8
		};

		uint16_t version;
		uint16_t bitFlag;
		CompressionMethod compressionMethod;
		uint16_t modificationTime;
		uint16_t modificationDate;
		uint32_t crc;
		uint32_t compressedSize;
		uint32_t uncompressedSize;
		uint16_t nameLength;
		uint16_t extraFieldLength;
		char 	*fileName;
		char	*extraField;

		int		dataStartPos;

		LocalFileHeader(std::istream &inputStream);
		LocalFileHeader(LocalFileHeader const& other) = delete;
		LocalFileHeader& operator=(LocalFileHeader const& other) = delete;
		~LocalFileHeader();
	};

	class DeflateStreamBuffer : public std::streambuf {
	public:
		DeflateStreamBuffer(std::shared_ptr<std::istream> zipStream, std::shared_ptr<LocalFileHeader> fileHeader);
		~DeflateStreamBuffer();
	private:
		DeflateStreamBuffer(DeflateStreamBuffer const&) = delete;
		DeflateStreamBuffer(DeflateStreamBuffer&&) = delete;
		DeflateStreamBuffer& operator=(DeflateStreamBuffer const&) = delete;
		DeflateStreamBuffer& operator=(DeflateStreamBuffer const&&) = delete;

		std::shared_ptr<std::istream> zipStream;
		std::shared_ptr<LocalFileHeader> fileHeader;
		char_type buf[1024];
		char_type zbuf[1024];
		z_stream zstr;

		virtual int_type underflow();

		void fillBuffer(unsigned int contentLeft);
	};

	class NoneStreamBuffer : public std::streambuf {
	public:
		NoneStreamBuffer(std::shared_ptr<std::istream> zipStream, std::shared_ptr<LocalFileHeader> fileHeader);
	private:
		std::shared_ptr<std::istream> zipStream;
		std::shared_ptr<LocalFileHeader> fileHeader;
		char_type buf[1024];

		virtual int_type underflow();
	};

	// Zip(std::shared_ptr<std::istream> zipStream);
	explicit Zip(std::string const& zipPath);

	void list() const;
	bool contains(std::string const& fileName) const;
	std::shared_ptr<std::istream> openFile(char const* fileName);

private:
	std::string zipPath;
	// std::shared_ptr<std::istream> zipStream;
	// std::istream::pos_type startOffset;
	std::unordered_map<std::string, std::shared_ptr<LocalFileHeader> > files;

	template<typename StreamBuffer>
	std::shared_ptr<std::istream> createStreamBuffer(std::shared_ptr<std::istream> const& zipStream,
			std::shared_ptr<LocalFileHeader> lfr);
};

std::ostream& operator<<(std::ostream& o, Zip::LocalFileHeader &fh);

// Template implementation
template<typename StreamBuffer>
std::shared_ptr<std::istream> Zip::createStreamBuffer(std::shared_ptr<std::istream> const& zipStream,
		std::shared_ptr<LocalFileHeader> lfr) {
	auto *sb = new StreamBuffer{zipStream, lfr};
	std::istream *stream = new std::istream{sb};

	// We need to delete dsb when shared ptr is destroyed.
	auto del = [sb](std::istream *stream) {
 		delete stream;
		delete sb;
	};

	return std::shared_ptr<std::istream> {stream, del};
}

}}

#endif // _ZIP_H_
