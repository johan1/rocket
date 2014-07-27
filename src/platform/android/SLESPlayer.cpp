#include "SLESPlayer.h"

#include <rocket/Algorithm.h>
#include <rocket/Log.h>

#define SL_EXECUTE(FUN, ...) \
	slExecute((boost::format("%s:%d") % __FILE__ % __LINE__).str(), FUN, __VA_ARGS__);

namespace rocket { namespace resource { namespace audio {

static void checkSlError(SLresult res, std::string const& msg) {
	if (res != SL_RESULT_SUCCESS) {
		std::string errorMsg = (boost::format("OpenSl error (code=%d), (msg=%s)")
				% res % msg).str();

		LOGE(errorMsg);
		throw std::runtime_error(errorMsg);
	}
}

template <typename Fun, typename ...Args>
void slExecute(std::string const& errorMsg, Fun && slFun, Args... args) {
	SLresult res = slFun(args...);
	checkSlError(res, errorMsg);
}

SLESPlayer::SLESPlayer() : bufferCount(0), playerCount(0) {
	SL_EXECUTE(slCreateEngine, &engineObject, 0, nullptr, 0, nullptr, nullptr);
	SL_EXECUTE((*engineObject)->Realize, engineObject, SL_BOOLEAN_FALSE);
    SL_EXECUTE((*engineObject)->GetInterface, engineObject, SL_IID_ENGINE, &engineEngine);
}

SLESPlayer::~SLESPlayer() {
	for (auto& player : players) {
		stopAudio(player.first);
	}
	(*engineObject)->Destroy(engineObject);
}

uint32_t SLESPlayer::loadAudioImpl(std::unique_ptr<PcmStream> &&pcmStream) {
	std::unique_ptr<PcmBuffer> pcmBuffer(new PcmBuffer());
	pcmBuffer->pcmStream = std::move(pcmStream);
	PcmInfo pcmInfo = pcmBuffer->pcmStream->getPcmInfo();

	pcmBuffer->bufferQueue.locatorType = SL_DATALOCATOR_BUFFERQUEUE;
	pcmBuffer->bufferQueue.numBuffers = 1;

	pcmBuffer->format.formatType = SL_DATAFORMAT_PCM;	
	pcmBuffer->format.numChannels = pcmInfo.channels;
	pcmBuffer->format.samplesPerSec = pcmInfo.frequency * 1000;
	pcmBuffer->format.bitsPerSample = pcmInfo.bitsPerSample;
	pcmBuffer->format.containerSize = 16; // ??
	pcmBuffer->format.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
	pcmBuffer->format.endianness = SL_BYTEORDER_LITTLEENDIAN;

	pcmBuffer->audioSource.pFormat = static_cast<void*>(&pcmBuffer->format);
	pcmBuffer->audioSource.pLocator = static_cast<void*>(&pcmBuffer->bufferQueue);

	buffers[++bufferCount] = std::move(pcmBuffer);
	return bufferCount;
}

void SLESPlayer::unloadAudioImpl(uint32_t audioId) {
	if (buffers.find(audioId) != buffers.end()) {
		auto& buffer = buffers[audioId];
		std::vector<uint32_t> playIds = buffer->playIds;
		for (uint32_t playId : playIds) {
			stopAudio(playId);
		}

		buffers.erase(audioId);
	}
}

void SLESPlayer::bqCallback(SLBufferQueueItf bqItf, void *context) {
	PcmPlayer* pcmPlayer = static_cast<PcmPlayer*>(context);
	PcmBuffer* pcmBuffer = pcmPlayer->bufferPtr;

	auto packet = pcmBuffer->pcmStream->getPacket(++pcmPlayer->packetId);
	if (!packet && pcmPlayer->loop) {
		pcmPlayer->packetId = 0;
		packet = pcmBuffer->pcmStream->getPacket(0);
		if (!packet) {
			throw std::runtime_error("Unexpected empty pcm packet when looping");
		}
	} else if(!packet) {
		return;
	}

	SL_EXECUTE((*bqItf)->Enqueue, bqItf, &((*packet)[0]), (*packet).size());
}

uint32_t SLESPlayer::playAudioImpl(uint32_t audioId, bool loop) {
	if (buffers.find(audioId) == buffers.end()) {
		throw std::runtime_error("SLESPlayer: Unknown buffer id");
	}
	auto &buffer = buffers[audioId];

	std::unique_ptr<PcmPlayer> pcmPlayer(new PcmPlayer());

    // create output mix
    { 
		SL_EXECUTE((*engineEngine)->CreateOutputMix, engineEngine, &pcmPlayer->outputMix, 0, nullptr, nullptr);
		SL_EXECUTE((*pcmPlayer->outputMix)->Realize, pcmPlayer->outputMix, SL_BOOLEAN_FALSE);
	}

	pcmPlayer->outputMixLocator.locatorType = SL_DATALOCATOR_OUTPUTMIX;
	pcmPlayer->outputMixLocator.outputMix = pcmPlayer->outputMix;
	pcmPlayer->audioSink.pLocator = &pcmPlayer->outputMixLocator;
	pcmPlayer->audioSink.pFormat = nullptr;
	
	pcmPlayer->loop = loop;

	pcmPlayer->bufferPtr = buffer.get();
	pcmPlayer->bufferId = audioId;
	pcmPlayer->packetId = 0;

	// Creating player
	{
		const SLInterfaceID ids[1] = { SL_IID_BUFFERQUEUE, };
    	const SLboolean req[1] = { SL_BOOLEAN_TRUE };
		SL_EXECUTE((*engineEngine)->CreateAudioPlayer, engineEngine, &pcmPlayer->player, 
				&buffer->audioSource, &pcmPlayer->audioSink, 1, ids, req);
		SL_EXECUTE((*pcmPlayer->player)->Realize, pcmPlayer->player, SL_BOOLEAN_FALSE);
		SL_EXECUTE((*pcmPlayer->player)->GetInterface, pcmPlayer->player, SL_IID_PLAY,
				&pcmPlayer->playItf);
		SL_EXECUTE((*pcmPlayer->player)->GetInterface, pcmPlayer->player, SL_IID_BUFFERQUEUE,
				&pcmPlayer->bufferQueueItf);

		SL_EXECUTE((*pcmPlayer->bufferQueueItf)->RegisterCallback, pcmPlayer->bufferQueueItf, bqCallback, pcmPlayer.get());
	}

	auto packet = buffer->pcmStream->getPacket(0);
	if (!packet) {
		LOGE("SLESPlayer empty pcm stream, WTF?");
		throw std::runtime_error("Empty pcm stream WTF");
	}

	SL_EXECUTE((*pcmPlayer->bufferQueueItf)->Enqueue, pcmPlayer->bufferQueueItf, 
			&((*packet)[0]), (*packet).size());

	SL_EXECUTE((*pcmPlayer->playItf)->SetPlayState, pcmPlayer->playItf, SL_PLAYSTATE_PLAYING);

	pcmPlayer->bufferId = audioId;
	players[++playerCount] = std::move(pcmPlayer);
	buffer->playIds.push_back(playerCount);

	return playerCount;
}

void SLESPlayer::pauseAudioImpl(uint32_t playId) {
	if (players.find(playId) != players.end()) {
		auto& player = players[playId];
		SLuint32 state;
		SL_EXECUTE((*player->playItf)->GetPlayState, player->playItf, &state);

		if (state == SL_PLAYSTATE_PLAYING) {
			SL_EXECUTE((*player->playItf)->SetPlayState, player->playItf, SL_PLAYSTATE_PAUSED);
		} else if (state == SL_PLAYSTATE_PAUSED) {
			SL_EXECUTE((*player->playItf)->SetPlayState, player->playItf, SL_PLAYSTATE_PLAYING);
		}
	}
}

void SLESPlayer::stopAudioImpl(uint32_t playId) {
	if (players.find(playId) != players.end()) {
		auto& player = players[playId];
		SLuint32 state;
		SL_EXECUTE((*player->playItf)->GetPlayState, player->playItf, &state);

		if (state == SL_PLAYSTATE_PLAYING || state == SL_PLAYSTATE_PAUSED) {
			SL_EXECUTE((*player->playItf)->SetPlayState, player->playItf, SL_PLAYSTATE_STOPPED);
		}

		(*player->player)->Destroy(player->player);
		(*player->outputMix)->Destroy(player->outputMix);

		// Clean up book-keeping.
		auto& buffer = buffers[player->bufferId];
		erase(buffer->playIds, playId);
		players.erase(playId);
	}
}

bool SLESPlayer::isPlayingImpl(uint32_t playId) const {
	auto it = players.find(playId);
	if (it != players.end()) {
		auto& player = it->second;
		SLuint32 state;
		SL_EXECUTE((*player->playItf)->GetPlayState, player->playItf, &state);

		return state == SL_PLAYSTATE_PLAYING;
	} else {
		return false;
	}
}

}}}
