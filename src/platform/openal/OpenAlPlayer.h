#ifndef _ROCKET_OPENAL_PLAYER_H_
#define _ROCKET_OPENAL_PLAYER_H_

#include <string>
#include <vector>
#include <unordered_map>

#include <AL/al.h>
#include <AL/alc.h>

#include <cstdint>

#include <boost/atomic.hpp>
#include <boost/thread.hpp>

#include "../../common/resource/audio/PcmStream.h"
#include "../../common/resource/audio/PlatformAudioPlayer.h"

namespace rocket { namespace resource { namespace audio {

class OpenAlPlayer : public PlatformAudioPlayer {
public:
	OpenAlPlayer();
	OpenAlPlayer(std::string const& devicePath);
	virtual ~OpenAlPlayer();

private:
	class BufferLoader {
	public:
		BufferLoader(std::unique_ptr<PcmStream> &&pcmStream);
		~BufferLoader();
		bool loadCompleted();

		//! Generates and loads buffer for stream.
		boost::optional<ALuint> getBufferId(uint32_t packetIndex);

	private:
		PcmInfo pcmInfo;
		ALenum format;
		std::unique_ptr<PcmStream> pcmStream;
		std::vector<ALuint> bufferIds;
		bool finishedLoading;

		boost::mutex bufferAccessMutex;
	};

	class AudioStreamer {
	public:
		AudioStreamer(std::shared_ptr<BufferLoader> const& bufferLoader, bool loop);
		void pause();
		bool isPlaying() const;
		~AudioStreamer();

	private:
		ALuint sourceId;
		std::shared_ptr<BufferLoader> bufferLoader;
		boost::thread streamThread;
		boost::atomic<bool> isTearingDown;
	};

	ALCdevice *devicePtr;
	ALCcontext *contextPtr;

	uint32_t audioIdCount;
	uint32_t streamerCount;
	std::unordered_map<uint32_t, std::shared_ptr<BufferLoader>> bufferLoaders;
	std::unordered_map<uint32_t, std::unique_ptr<AudioStreamer>> audioStreams;

	OpenAlPlayer(OpenAlPlayer const&) = delete;
	OpenAlPlayer& operator=(OpenAlPlayer const&) = delete;

	virtual uint32_t loadAudioImpl(std::unique_ptr<PcmStream> &&pcmStream);
	virtual void unloadAudioImpl(uint32_t audioId);
	virtual uint32_t playAudioImpl(uint32_t playId, bool loop);
	virtual void pauseAudioImpl(uint32_t playId);
	virtual void stopAudioImpl(uint32_t playId);
	virtual bool isPlayingImpl(uint32_t playId) const;
};

}}

using namespace resource::audio; // Collapse
}

#endif
