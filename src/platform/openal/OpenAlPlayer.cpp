#include "OpenAlPlayer.h"

#include <rocket/Log.h>

#include <AL/al.h>
#include <boost/format.hpp>

using namespace boost;

namespace rocket { namespace resource { namespace audio {

void resetAlError() {
	alGetError();
}

// Note: when used in destructor set abortOnError to true, to avoid throwing in destructor
void checkAlError(std::string const& tag, bool abortOnError = false) {
	auto errorCode = alGetError();
	if (errorCode == AL_NO_ERROR) {
		return;
	}

	std::string errorLabel;	
	switch (errorCode) {
	case AL_INVALID_NAME:
		errorLabel = "AL_INVALID_NAME";
		break;
	case AL_INVALID_ENUM:
		errorLabel = "AL_INVALID_ENUM";
		break;
	case AL_INVALID_VALUE:
		errorLabel = "AL_INVALID_VALUE";
		break;
	case AL_INVALID_OPERATION:
		errorLabel = "AL_INVALID_OPERATION";
		break;
	case AL_OUT_OF_MEMORY:
		errorLabel = "AL_OUT_OF_MEMORY";
		break;
	default:
		errorLabel = "UNSPECIFIED ERROR";
		break;
	}

	std::string errorMsg = (boost::format("AL error, %s %s (%d)") 
			% tag % errorLabel % errorCode).str();
	LOGE(errorMsg);

	if (abortOnError) {
		std::abort();
	} else {
		throw std::runtime_error(errorMsg);
	}
}

// Note: when used in destructor set abortOnError to true, to avoid throwing in destructor
void checkAlcError(ALCdevice *device, std::string const& tag, bool abortOnError = false) {
	auto errorCode = alcGetError(device);
	if (errorCode == ALC_NO_ERROR) {
		return;
	}

	std::string errorLabel;	
	switch (errorCode) {
	case ALC_INVALID_DEVICE:
		errorLabel = "ALC_INVALID_DEVICE";
		break;
	case ALC_INVALID_CONTEXT:
		errorLabel = "ALC_INVALID_CONTEXT";
		break;
	case ALC_INVALID_ENUM:
		errorLabel = "ALC_INVALID_ENUM";
		break;
	case ALC_INVALID_VALUE:
		errorLabel = "ALC_INVALID_VALUE";
		break;
	case ALC_OUT_OF_MEMORY:
		errorLabel = "ALC_OUT_OF_MEMORY";
		break;
	default:
		errorLabel = "UNSPECIFIED ERROR";
		break;
	}

	std::string errorMsg = (boost::format("ALC error, %s %s (%d)") 
			% tag % errorLabel % errorCode).str();
	LOGE(errorMsg);

	if (abortOnError) {
		std::abort();
	} else {
		throw std::runtime_error(errorMsg);
	}
}

OpenAlPlayer::OpenAlPlayer() : OpenAlPlayer("") {}

OpenAlPlayer::OpenAlPlayer(std::string const& devicePath) : audioIdCount(0), streamerCount(0) {
	if (devicePath.empty()) {
		devicePtr = alcOpenDevice(nullptr);
	} else {
		devicePtr = alcOpenDevice(devicePath.c_str());
	}

	if (!devicePtr) {
		std::string errorMsg;
		if (devicePath.empty()) {
			errorMsg = "Unable to open default audio device";
		} else {
			errorMsg = boost::format("Unable to open audio device").str();
		}
		LOGE(errorMsg);
		throw std::runtime_error(errorMsg);
	}
	contextPtr = alcCreateContext(devicePtr, nullptr); // Maybe we should give some attributes here?
	if (!contextPtr) {
		std::string errorMsg("Unable to create context");
		LOGE(errorMsg);
		throw std::runtime_error(errorMsg);
	}

	alcMakeContextCurrent(contextPtr);

	LOGD("Contructed player");
}

OpenAlPlayer::~OpenAlPlayer() {
	// LOGD("Destructor called");
	audioStreams.clear();
	bufferLoaders.clear();

	alcGetError(devicePtr); // Clear alc error state

	alcDestroyContext(contextPtr);
	checkAlcError(devicePtr, "alcDestroyContext", true);
	alcCloseDevice(devicePtr);
}

uint32_t OpenAlPlayer::loadAudioImpl(std::unique_ptr<PcmStream> &&pcmStream) {
	bufferLoaders[++audioIdCount] = std::make_shared<BufferLoader>(std::move(pcmStream));
	return audioIdCount;
}

void OpenAlPlayer::unloadAudioImpl(uint32_t audioId) {
	bufferLoaders.erase(audioId);
}

uint32_t OpenAlPlayer::playAudioImpl(uint32_t audioId, bool loop) {
	if (bufferLoaders.find(audioId) == bufferLoaders.end()) {
		std::string msg = (boost::format("Audio id %d, is not available") % audioId).str();
		throw std::runtime_error(msg);
	}

	auto& bufferLoader = bufferLoaders[audioId];
	audioStreams[++streamerCount] = std::unique_ptr<AudioStreamer>(new AudioStreamer(bufferLoader, loop));
	return streamerCount;
}

void OpenAlPlayer::pauseAudioImpl(uint32_t playId) {
	if (audioStreams.find(playId) != audioStreams.end() ) {
		auto& audioStreamer = audioStreams[playId];
		audioStreamer->pause();
	}
}

void OpenAlPlayer::stopAudioImpl(uint32_t playId) {
	// TODO: Do we want to pause here?
	if (audioStreams.find(playId) != audioStreams.end()) {
		audioStreams.erase(playId);
	}
}

bool OpenAlPlayer::isPlayingImpl(uint32_t playId) const {
	auto it = audioStreams.find(playId);
	if (it != audioStreams.end()) {
		return it->second->isPlaying();
	} else {
		return false;
	}
}

OpenAlPlayer::BufferLoader::BufferLoader(std::unique_ptr<PcmStream> &&pcmStream) : pcmStream(std::move(pcmStream)), finishedLoading(false) {
	pcmInfo = this->pcmStream->getPcmInfo();

	if (pcmInfo.bitsPerSample == 8) {
		if (pcmInfo.channels == 1) {
			format = AL_FORMAT_MONO8;
		} else {
			format = AL_FORMAT_STEREO8;
		}
	} else if (pcmInfo.bitsPerSample == 16) {
		if (pcmInfo.channels == 1) {
			format = AL_FORMAT_MONO16;
		} else {
			format = AL_FORMAT_STEREO16;
		}
	} else {
		std::string errMsg = (boost::format("Unable to determine pcm format, channels=%d, bitsPerSample=%d")
				% pcmInfo.channels % pcmInfo.bitsPerSample).str();
		LOGE(errMsg);
		
		throw std::runtime_error(errMsg);
	}
}

OpenAlPlayer::BufferLoader::~BufferLoader() {
	alDeleteBuffers(bufferIds.size(), &(bufferIds[0]));
}

bool OpenAlPlayer::BufferLoader::loadCompleted() {
	return finishedLoading;
}

//! Generates and loads buffer for stream.
boost::optional<ALuint> OpenAlPlayer::BufferLoader::getBufferId(uint32_t packetIndex) {
	// This method is accessed through several threads, for buffer management to work
	// we need to lock access to this method to one thread at a time.
	lock_guard<mutex> lock(bufferAccessMutex); 

	// Check if packet has already been loaded
	if (packetIndex < bufferIds.size()) {
		return bufferIds[packetIndex];
	} else if(packetIndex > bufferIds.size()) {
		throw std::runtime_error("Invalid usage, getBufferId needs to be called with sequentially increasing packetIndex");
	}

	if (finishedLoading) {
		return boost::optional<ALuint>();
	}

	// Fetch packet
	auto packet = pcmStream->getPacket(packetIndex); // Blocks until packet is available.
	if (!packet) {
		finishedLoading = true;
		pcmStream.reset();
		return boost::optional<ALuint>();
	}

	resetAlError();

	ALuint bufferId;
	alGenBuffers(1, &bufferId);
	checkAlError("alGenBuffers");

	alBufferData(bufferId, format, &((*packet)[0]), (*packet).size(), pcmInfo.frequency);
	checkAlError("alBufferData");

	bufferIds.push_back(bufferId);
	return bufferId;
}

OpenAlPlayer::AudioStreamer::AudioStreamer(std::shared_ptr<BufferLoader> const& bufferLoader, bool loop) : bufferLoader(bufferLoader), isTearingDown(false) {
	resetAlError();

	alGenSources(1, &sourceId);
	checkAlError("alGenSources");
	
	auto bufferId = this->bufferLoader->getBufferId(0);

	alSourceQueueBuffers(sourceId, 1, &(*bufferId));
	checkAlError("alSourceQueueBuffers, AL_BUFFER");

	LOGD("Set looping " << loop);
	alSourcei(sourceId, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
	checkAlError("alSourcei, AL_LOOPING");

	alSourcePlay(sourceId);
	checkAlError("alSourcePlay");

	streamThread = thread([this]() {
		uint32_t i = 1;
		
		auto bufferId = this->bufferLoader->getBufferId(i++);
		while (bufferId && !isTearingDown) {
			alSourceQueueBuffers(sourceId, 1, &(*bufferId));
			checkAlError("alSourceQueueBuffers, AL_BUFFER");

			bufferId = this->bufferLoader->getBufferId(i++);
		}
	});
}

OpenAlPlayer::AudioStreamer::~AudioStreamer() {
	isTearingDown = true;
	streamThread.join();

	alSourceStop(sourceId);
	checkAlError("alSourceStop");

	alDeleteSources(1, &sourceId);
	checkAlError("alDeleteSources");
}

void OpenAlPlayer::AudioStreamer::pause() {
	ALint sourceState;
	alGetSourcei(sourceId, AL_SOURCE_STATE, &sourceState);
	checkAlError("alGetSourcei, AL_SOURCE_STATE");

	if (sourceState == AL_PLAYING) {
		alSourcePause(sourceId);
		checkAlError("alSourcePause");
	} else if (sourceState == AL_PAUSED) {
		alSourcePlay(sourceId);
		checkAlError("alSourcePlay");
	}
}

bool OpenAlPlayer::AudioStreamer::isPlaying() const {
	resetAlError();

	ALint sourceState;
	alGetSourcei(sourceId, AL_SOURCE_STATE, &sourceState);
	checkAlError("alGetSourcei, AL_SOURCE_STATE");

	return sourceState == AL_PLAYING;
}

}}}
