#ifndef _ROCKET_PCM_DECODER_H_
#define _ROCKET_PCM_DECODER_H_

#include <istream>
#include <vector>

namespace rocket { namespace resource { namespace audio {

struct PcmInfo {
	uint16_t channels;
	uint32_t frequency;
	uint16_t bitsPerSample;
};

class PcmDecoder {
public:
	virtual ~PcmDecoder() {}

	bool hasMorePackets() const;
	void readNextPacket(std::vector<char> &data);
	PcmInfo const& getPcmInfo() const;

private:
	virtual bool hasMorePacketsImpl() const = 0;
	virtual void readNextPacketImpl(std::vector<char> &data) = 0;
	virtual PcmInfo const& getPcmInfoImpl() const = 0;
};

inline
bool PcmDecoder::hasMorePackets() const {
	return hasMorePacketsImpl();
}

inline
void PcmDecoder::readNextPacket(std::vector<char> &data) {
	readNextPacketImpl(data);
}

inline
PcmInfo const& PcmDecoder::getPcmInfo() const {
	return getPcmInfoImpl();
};

}}
using namespace resource::audio; // Collapse
}
#endif
