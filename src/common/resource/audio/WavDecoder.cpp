#include "WavDecoder.h"

#include <cstring>
#include "../../util/Log.h"

using namespace rocket::util;

namespace rocket { namespace resource { namespace audio {

WavDecoder::RiffChunk::RiffChunk(std::istream &is) {
	char tagArr[5];
	is.read(tagArr, 4);
	tagArr[4] = '\0';

	tag = tagArr;

	is.read(reinterpret_cast<char*>(&size), 4);
}

static const char WAVE_CHUNK_ID[] = {'W', 'A', 'V', 'E'};

static const uint16_t WAVE_FORMAT_PCM = 1;

WavDecoder::WavFormat WavDecoder::parseWavFormat(std::istream &is) {
	WavFormat wf;
	is.read(reinterpret_cast<char*>(&wf.format), 2);
	is.read(reinterpret_cast<char*>(&wf.channels), 2);
	is.read(reinterpret_cast<char*>(&wf.samplesPerSec), 4);
	is.read(reinterpret_cast<char*>(&wf.avgBytesPerSec), 4);
	is.read(reinterpret_cast<char*>(&wf.blockSize), 2);

	return wf;
}

WavDecoder::WavDecoder(std::shared_ptr<std::istream> const& is) : is(is) {
	RiffChunk riffChunk(*is);
	if (riffChunk.tag != "RIFF") {
		throw std::runtime_error("Unexpected riff chunk id");	
	}

	// Parsing formtag
	char formTag[4];
	is->read(formTag, 4);
	if (std::memcmp(formTag, WAVE_CHUNK_ID, 4) != 0) {
		LOGE("Unexpected chunk id, expected WAVE id");
		throw std::runtime_error("Unexpected chunk id");
	}

	// Parse next chunk
	RiffChunk waveFormatChunk(*is);
	if (waveFormatChunk.tag != "fmt ") {
		throw std::runtime_error("Unexpected riff chunk id");	
	}

	WavFormat wf = parseWavFormat(*is);
	/*
	LOGD(boost::format("Wave format chunk is %d bytes") % waveFormatChunk.size);
	LOGD(boost::format("Wave format: format=%d, channels=%d, samples per sec=%d, avg bytes per sec=%d, block size=%d") 
			% wf.format 
			% wf.channels 
			% wf.samplesPerSec 
			% wf.avgBytesPerSec
			% wf.blockSize);
	*/

	if (wf.format == WAVE_FORMAT_PCM) {
		is->read(reinterpret_cast<char*>(&pcmInfo.bitsPerSample), 2);
	} else {
		throw std::runtime_error("Format type not supported");
	}
	pcmInfo.channels = wf.channels;
	pcmInfo.frequency = wf.samplesPerSec;

	bool dataChunkFound = false;

	RiffChunk dataChunk;
	while (!dataChunkFound) {
		RiffChunk chunk(*is);
		if (chunk.tag == "data") {
			dataChunk = chunk;
			dataChunkFound = true;
		} else {
			// Seek to next chunk
			is->seekg(chunk.size, std::ios_base::cur);
			if (is->eof()) {
				throw std::runtime_error("Reached end of stream before we could find a data chunk");
			}
		}
	}

	bytesLeftToRead = dataChunk.size;
}

bool WavDecoder::hasMorePacketsImpl() const {
	return bytesLeftToRead != 0;
}

void WavDecoder::readNextPacketImpl(std::vector<char> &data) {
	if (bytesLeftToRead > 0) {
		auto bytesToRead = bytesLeftToRead < 4096 ? bytesLeftToRead : 4096;
		is->read(readBuffer, bytesToRead);
		data.insert(data.end(), &(readBuffer[0]), &(readBuffer[bytesToRead]));
		bytesLeftToRead -= bytesToRead;
	}
}

PcmInfo const& WavDecoder::getPcmInfoImpl() const {
	return pcmInfo;
}

}}}
