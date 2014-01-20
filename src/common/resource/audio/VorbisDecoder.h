#ifndef _ROCKET_VORBIS_DECODER_
#define _ROCKET_VORBIS_DECODER_

#include "PcmDecoder.h"
#include <vorbis/vorbisfile.h>
#include <memory>

namespace rocket { namespace resource { namespace audio {

class VorbisDecoder : public PcmDecoder {
public:
	VorbisDecoder(std::shared_ptr<std::istream> const& is);
	virtual ~VorbisDecoder() noexcept(true);

private:
	std::shared_ptr<std::istream> is;
	OggVorbis_File vf;
	PcmInfo pcmInfo;

	VorbisDecoder(VorbisDecoder const&) = delete;
	VorbisDecoder& operator=(VorbisDecoder const&) = delete;

	char readBuffer[4096];
	virtual bool hasMorePacketsImpl() const;
	virtual void readNextPacketImpl(std::vector<char> &data);
	virtual PcmInfo const& getPcmInfoImpl() const;
};

}}}

#endif
