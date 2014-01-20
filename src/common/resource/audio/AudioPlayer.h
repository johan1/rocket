#ifndef _ROCKET_AUDIO_PLAYER_H_
#define _ROCKET_AUDIO_PLAYER_H_

#include <cstdint>
#include <memory>
#include <unordered_map>

#include "PlatformAudioPlayer.h"

#include "../ResourceId.h"

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
};

}}}

#endif

// Example..
// auto& am = Application.getAudioManager();
// auto audioId = am.load(ResourceId("test.ogg"), false); // Wait until audio loaded.
// auto playId1 = am.play(audioId, 0);
// auto playId2 = am.play(audioId, 1);
