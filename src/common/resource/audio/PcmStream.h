#ifndef _ROCKET_PCM_STREAM_H_
#define _ROCKET_PCM_STREAM_H_

#include "PcmDecoder.h"
#include <memory>
#include <boost/atomic.hpp>
#include <boost/thread.hpp>
#include <boost/optional.hpp>

namespace rocket { namespace resource { namespace audio {

class PcmStream {
public:
	PcmStream(std::unique_ptr<PcmDecoder> &&decoder);
	~PcmStream();

	PcmStream(PcmStream &&) = default;
	PcmStream& operator=(PcmStream &&) = default;

	PcmInfo const& getPcmInfo() const;

	boost::optional<std::vector<char> const&> getPacket(uint32_t index);

private:
	PcmInfo pcmInfo;

	std::unique_ptr<PcmDecoder> decoder;

	boost::thread decoderThread;
	boost::mutex packetsMutex;

	std::vector<std::vector<char>> packets;
	bool allPacketsRead;
	boost::atomic<bool> shuttingDown;
};

}}} 

#endif
