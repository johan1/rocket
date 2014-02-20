#ifndef _ROCKET_PLATFORM_AUDIO_PLAYER_H_
#define _ROCKET_PLATFORM_AUDIO_PLAYER_H_

#include <cstdint>
#include <memory>

#include "PcmStream.h"

namespace rocket { namespace resource { namespace audio {

class PlatformAudioPlayer {
public:
	virtual ~PlatformAudioPlayer();

	uint32_t loadAudio(std::unique_ptr<PcmStream> &&pcmStream);
	void unloadAudio(uint32_t audioId);

	uint32_t playAudio(uint32_t audioId, bool loop);
	void pauseAudio(uint32_t playId);
	void stopAudio(uint32_t playId);
	bool isPlaying(uint32_t playId) const;

	//void setVolume(uint32_t playId, float volume);
	//float getVolume(uint32_t playId) const;

private:
	virtual uint32_t loadAudioImpl(std::unique_ptr<PcmStream> &&pcmStream) = 0;
	virtual void unloadAudioImpl(uint32_t audioId) = 0;
	virtual uint32_t playAudioImpl(uint32_t playId, bool loop) = 0;
	virtual void pauseAudioImpl(uint32_t playId) = 0;
	virtual void stopAudioImpl(uint32_t playId) = 0;
	virtual bool isPlayingImpl(uint32_t playId) const = 0;
	//virtual void setVolumeImpl(uint32_t playId, float volume) = 0;
	//virtual float getVolumeImpl(uint32_t playId) const = 0;
};

inline
PlatformAudioPlayer::~PlatformAudioPlayer() {}

inline
uint32_t PlatformAudioPlayer::loadAudio(std::unique_ptr<PcmStream> &&pcmStream) {
	return loadAudioImpl(std::move(pcmStream));
}

inline
void PlatformAudioPlayer::unloadAudio(uint32_t audioId) {
	return unloadAudioImpl(audioId);
}

inline
uint32_t PlatformAudioPlayer::playAudio(uint32_t audioId, bool loop = false) {
	return playAudioImpl(audioId, loop);
}

inline
void PlatformAudioPlayer::pauseAudio(uint32_t playId) {
	pauseAudioImpl(playId);
}

inline
void PlatformAudioPlayer::stopAudio(uint32_t playId) {
	stopAudioImpl(playId);
}

inline
bool PlatformAudioPlayer::isPlaying(uint32_t playId) const {
	return isPlayingImpl(playId);
}
/*
inline
void PlatformAudioPlayer::setVolume(uint32_t playId, float volume) {
	return setVolumeImpl(playId, volume);
}

inline
float PlatformAudioPlayer::getVolume(uint32_t playId) const {
	return getVolumeImpl(playId);
}
*/

}}
using namespace resource::audio; // Collapse
}

#endif

