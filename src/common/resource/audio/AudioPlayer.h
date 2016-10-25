#ifndef _ROCKET_AUDIO_PLAYER_H_
#define _ROCKET_AUDIO_PLAYER_H_

#include <cstdint>
#include <memory>
#include <unordered_map>

#include "PlatformAudioPlayer.h"

#include "../ResourceId.h"

#include <rocket/Algorithm.h>

namespace rocket { namespace resource { namespace audio {

// Support three different kind of audio, effects and ambivialent bg music.
class AudioPlayer {
public:
	AudioPlayer(std::unique_ptr<PlatformAudioPlayer> &&platformAudioPlayer);

	// Resource management
	uint32_t load(rocket::resource::ResourceId resourceId);

	void free(uint32_t audioId);

	// Audio playing
	uint32_t play(uint32_t audioId, bool loop = false);

	void pause(uint32_t playId);

	void stop(uint32_t playId);

	bool isPlaying(uint32_t playId);

	// Volume management
	/*
	void setVolume(uint32_t audioTrack, float volume);
	float getVolume(uint32_t audioTrack);
	*/

private:
	std::unique_ptr<PlatformAudioPlayer> player;
	std::unordered_map<uint32_t, std::vector<uint32_t>> playIds;
};

class PooledAudioPlayer {
public:
	PooledAudioPlayer() {}

	explicit PooledAudioPlayer(AudioPlayer &audioPlayer) : audioPlayer(&audioPlayer) {}

	~PooledAudioPlayer() {
		release();
	}

	PooledAudioPlayer(PooledAudioPlayer const&) = delete;

	PooledAudioPlayer(PooledAudioPlayer &&pool) : audioPlayer(pool.audioPlayer) {
		std::swap(this->audioIds, pool.audioIds);
	}

	PooledAudioPlayer& operator=(PooledAudioPlayer const&) = delete;

	PooledAudioPlayer& operator=(PooledAudioPlayer &&pool) {
		this->audioIds.clear();
		std::swap(this->audioIds, pool.audioIds);
		return *this;
	}
	
	// Resource management
	uint32_t load(rocket::resource::ResourceId resourceId) {
		auto audioId = audioPlayer->load(resourceId);
		audioIds.push_back(audioId);
		return audioId;
	}

	void free(uint32_t audioId) {
		audioPlayer->free(audioId);
		erase(audioIds, audioId);
	}

	// Audio playing
	uint32_t play(uint32_t audioId, bool loop = false) {
		return audioPlayer->play(audioId, loop);
	}

	void pause(uint32_t playId) {
		audioPlayer->pause(playId);
	}

	void stop(uint32_t playId) {
		audioPlayer->stop(playId);
	}

	bool isPlaying(uint32_t playId) {
		return audioPlayer->isPlaying(playId);
	}

	void release() {
		for (auto audioId : audioIds) {
			audioPlayer->free(audioId);
		}
	}

private:
	AudioPlayer *audioPlayer = nullptr;
	std::vector<uint32_t> audioIds;
};

}}

using namespace resource::audio; // Collapse
}

#endif

// Example..
// auto& am = Application.getAudioManager();
// auto audioId = am.load(ResourceId("test.ogg"), false); // Wait until audio loaded.
// auto playId1 = am.play(audioId, 0);
// auto playId2 = am.play(audioId, 1);
