#ifndef _ROCKET_WAV_DECODER_H_
#define _ROCKET_WAV_DECODER_H_

#include "PcmDecoder.h"

#include <memory>
#include <string>

namespace rocket { namespace resource { namespace audio {

class WavDecoder : public PcmDecoder {
public:
	WavDecoder(std::shared_ptr<std::istream> const& is);

private:
	struct RiffChunk {
		std::string tag;
		uint32_t size;

		RiffChunk() : tag(""), size(0) {}
		RiffChunk(std::istream &is);
	};

	struct WavFormat {
		uint16_t format;
		uint16_t channels;
		uint32_t samplesPerSec;
		uint32_t avgBytesPerSec;
		uint16_t blockSize;
	};

	std::shared_ptr<std::istream> is;
	PcmInfo pcmInfo;
	//RiffChunk riffData

	char readBuffer[4096];
	uint32_t bytesLeftToRead;

	virtual bool hasMorePacketsImpl() const;
	virtual void readNextPacketImpl(std::vector<char> &data);
	virtual PcmInfo const& getPcmInfoImpl() const;

	static WavFormat parseWavFormat(std::istream &is);

//	virtual PcmData loadPcmDataImpl(std::istream &is);
};

}}
using namespace resource::audio; // Collapse
}

#endif
