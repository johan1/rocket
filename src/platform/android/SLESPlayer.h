#ifndef _ROCKET_SLES_PLAYER_H_
#define _ROCKET_SLES_PLAYER_H_

#include "../../common/resource/audio/PlatformAudioPlayer.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

#include <boost/thread/mutex.hpp>

#include <unordered_map>
#include <memory>

namespace rocket {
namespace resource { namespace audio {

class SLESPlayer : public PlatformAudioPlayer {
public:
	SLESPlayer();
	virtual ~SLESPlayer();	 

private:
	struct PcmBuffer { // TODO: Rename to PcmSource
		std::unique_ptr<PcmStream> pcmStream;
		SLDataFormat_PCM format;
		std::vector<uint32_t> playIds;
	};

	struct PcmPlayer {
		SLObjectItf player;
		SLPlayItf playItf;

		SLAndroidSimpleBufferQueueItf bufferQueueItf;

		bool loop;
		PcmBuffer* bufferPtr;
		uint32_t bufferId;
		uint32_t packetId;
		SLmillibel maxVolumeLevel;
	};

	struct SLESPlayerContext {
		SLESPlayer *slesPlayer;
		uint32_t playerId;
	};

	SLObjectItf engineObject;
	SLEngineItf engineEngine;
    SLObjectItf outputMix;
        
	uint32_t bufferCount;
	uint32_t playerCount;
	std::unordered_map<uint32_t, std::unique_ptr<PcmBuffer>> buffers;
	std::unordered_map<uint32_t, std::unique_ptr<PcmPlayer>> players;

	std::vector<uint32_t> finishedPlayIds; 

	virtual uint32_t loadAudioImpl(std::unique_ptr<PcmStream> &&pcmStream);
	virtual void unloadAudioImpl(uint32_t audioId);
	virtual uint32_t playAudioImpl(uint32_t playId, bool loop);
	virtual void pauseAudioImpl(uint32_t playId);
	virtual void stopAudioImpl(uint32_t playId);
	virtual bool isPlayingImpl(uint32_t playId) const;

	static boost::recursive_mutex slesMutex; // <-- TODO: This might need to be global.
	static void bqCallback(SLAndroidSimpleBufferQueueItf bqItf, void *context);
	static std::unordered_map<SLAndroidSimpleBufferQueueItf, SLESPlayerContext> slesPlayerContexts;
};

}
using namespace resource::audio; // And collapse

}}

#endif 
