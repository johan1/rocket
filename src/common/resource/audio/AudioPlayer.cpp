#include "AudioPlayer.h"

#include "VorbisDecoder.h"
#include "WavDecoder.h"
#include "PcmStream.h"

#include "../ResourceId.h"

#include "../../util/Log.h"

#include "../../Application.h"

namespace rocket { namespace resource { namespace audio {

AudioPlayer::AudioPlayer(std::unique_ptr<PlatformAudioPlayer> &&player) :
		player(std::move(player)) {}

std::unique_ptr<PcmStream> createPcmStream(std::string const& suffix, std::shared_ptr<std::istream> const& is) {
	if (suffix == ".ogg") {
		return std::unique_ptr<PcmStream>(
				new PcmStream(std::unique_ptr<PcmDecoder>(new VorbisDecoder(is))));
	} else if (suffix == ".wav") {
		return std::unique_ptr<PcmStream>(
				new PcmStream(std::unique_ptr<PcmDecoder>(new WavDecoder(is))));
	} else {
		std::string errorMsg = (boost::format("Filetype: %s is not supported") % suffix).str();
		LOGE(errorMsg);
		throw std::runtime_error(errorMsg);
	}
}

// Resource management
uint32_t AudioPlayer::load(rocket::resource::ResourceId resourceId) {
	auto filename = resourceId.getFileName();
	auto suffix = filename.substr(filename.size()-4, 4);

	auto resources = Application::getApplication().getResources();
	std::shared_ptr<std::istream> is = resources.openResource(resourceId);

	// Creating pcm stream 
	std::unique_ptr<PcmStream> pcmStream = createPcmStream(suffix, is);
	return player->loadAudio(std::move(pcmStream));
}

void AudioPlayer::free(uint32_t audioId) {
	player->unloadAudio(audioId);
}

// Audio playing
uint32_t AudioPlayer::play(uint32_t audioId, bool loop) {
	return player->playAudio(audioId, loop);
}

void AudioPlayer::pause(uint32_t playId) {
	player->pauseAudio(playId);
}

void AudioPlayer::stop(uint32_t playId) {
	player->stopAudio(playId);
}

bool AudioPlayer::isPlaying(uint32_t playId) {
	return player->isPlaying(playId);
}

/*
// Volume management
void AudioPlayer::setVolume(uint32_t audioTrack, float volume) {
	LOGD("TODO: implement AudioPlayer::setVolume");
}

float AudioPlayer::getVolume(uint32_t audioTrack) {
	LOGD("TODO: implement AudioPlayer::getVolume");
	return 0;
}
*/

}}}
