#include "PcmStream.h"
#include "../../util/Log.h"

using namespace boost;

namespace rocket { namespace resource { namespace audio {

PcmStream::PcmStream(std::unique_ptr<PcmDecoder> &&decoder) : decoder(std::move(decoder)), allPacketsRead(false), shuttingDown(false) {
	pcmInfo = this->decoder->getPcmInfo();

	decoderThread = thread([this]() {
		int packetCount = 0; 
		while (this->decoder->hasMorePackets() && !this->shuttingDown) {
			std::vector<char> packet;
			this->decoder->readNextPacket(packet);

			// TAKE lock and insert packet into packet vector
			{
				++packetCount;
				lock_guard<mutex> lock(packetsMutex);
				this->packets.push_back(std::move(packet));
			}
		}

		{
			lock_guard<mutex> lock(packetsMutex);
			allPacketsRead = true;
		}
		LOGD(boost::format("Finished decoding, %d packets decoded") % packetCount);
		this->decoder.reset();
	});
}

PcmStream::~PcmStream() {
	try {
		if (decoder) {
			LOGD("Shutting down decoder");
			shuttingDown = true;
			decoderThread.join();
		}
	} catch (...) {
		LOGE("Failed to join decoderThread in PcmStream destructor. ABORT!");
		std::abort();
	}
}

PcmInfo const& PcmStream::getPcmInfo() const {
	return pcmInfo;
}

boost::optional<std::vector<char> const&> PcmStream::getPacket(uint32_t index) {
	while (true) {
		{
			lock_guard<mutex> lock(packetsMutex);
			if (index < packets.size()) {
				return packets[index];
			} else if (allPacketsRead) {
				return boost::optional<std::vector<char> const&>(); // Already read last packet.
			}
		}

		this_thread::yield();
	}
	
}

}}} 
