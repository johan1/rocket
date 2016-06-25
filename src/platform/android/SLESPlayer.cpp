#include "SLESPlayer.h"

#include <rocket/Algorithm.h>
#include <rocket/Log.h>

#include <boost/thread/locks.hpp> 

#define SL_EXECUTE(FUN, ...) \
	slExecute((boost::format("%s:%d") % __FILE__ % __LINE__).str(), FUN, __VA_ARGS__);

namespace rocket { namespace resource { namespace audio {

boost::recursive_mutex SLESPlayer::slesMutex;
std::unordered_map<SLAndroidSimpleBufferQueueItf, SLESPlayer::SLESPlayerContext> SLESPlayer::slesPlayerContexts;

static void checkSlError(SLresult res, std::string const& msg) {
	if (res != SL_RESULT_SUCCESS) {
		std::string errorMsg = (boost::format("OpenSl error (code=%d), (msg=%s)")
				% res % msg).str();

		LOGE(errorMsg);
		std::terminate();
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
    
    SL_EXECUTE((*engineEngine)->CreateOutputMix, engineEngine, &outputMix, 0, nullptr, nullptr);
    SL_EXECUTE((*outputMix)->Realize, outputMix, SL_BOOLEAN_FALSE);
}

SLESPlayer::~SLESPlayer() {
	for (auto& player : players) {
		stopAudio(player.first);
	}
	(*outputMix)->Destroy(outputMix);
	(*engineObject)->Destroy(engineObject);
}

uint32_t SLESPlayer::loadAudioImpl(std::unique_ptr<PcmStream> &&pcmStream) {
	boost::lock_guard<boost::recursive_mutex> lock(SLESPlayer::slesMutex);

	std::unique_ptr<PcmBuffer> pcmBuffer(new PcmBuffer());
	pcmBuffer->pcmStream = std::move(pcmStream);
	PcmInfo pcmInfo = pcmBuffer->pcmStream->getPcmInfo();

	pcmBuffer->format.formatType = SL_DATAFORMAT_PCM;
	pcmBuffer->format.numChannels = pcmInfo.channels;
	pcmBuffer->format.samplesPerSec = pcmInfo.frequency * 1000;
	pcmBuffer->format.bitsPerSample = pcmInfo.bitsPerSample;
	pcmBuffer->format.containerSize = 16; // ??

	LOGD("Channels: " << pcmBuffer->format.numChannels
		<< ", samples per sec: " << pcmBuffer->format.samplesPerSec
		<< ", bits per sample: " << pcmBuffer->format.bitsPerSample
		<< ", container size: " << pcmBuffer->format.containerSize);

	if (pcmBuffer->format.numChannels == 1) {
		pcmBuffer->format.channelMask = SL_SPEAKER_FRONT_CENTER;
	} else {
		pcmBuffer->format.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
	}
	pcmBuffer->format.endianness = SL_BYTEORDER_LITTLEENDIAN;

	buffers[++bufferCount] = std::move(pcmBuffer);
	return bufferCount;
}

void SLESPlayer::unloadAudioImpl(uint32_t audioId) {
	LOGD("unloadAudioImpl")
	boost::lock_guard<boost::recursive_mutex> lock(SLESPlayer::slesMutex);

	if (buffers.find(audioId) != buffers.end()) {
		auto& buffer = buffers[audioId];
		std::vector<uint32_t> playIds = buffer->playIds;
		for (uint32_t playId : playIds) {
			stopAudioImpl(playId);
		}

		buffers.erase(audioId);
	}
	LOGD("audioId: " << audioId << " unloaded");
}



void SLESPlayer::bqCallback(SLAndroidSimpleBufferQueueItf bqItf, void *context) {
	boost::lock_guard<boost::recursive_mutex> lock(SLESPlayer::slesMutex);

	LOGD("bqCallback");
	if (SLESPlayer::slesPlayerContexts.find(bqItf) == SLESPlayer::slesPlayerContexts.end()) {
		LOGW("No context, return");
		return;
	}

	SLESPlayerContext spContext = SLESPlayer::slesPlayerContexts[bqItf];

	if (spContext.slesPlayer->players.find(spContext.playerId) ==
			spContext.slesPlayer->players.end()) {
		LOGE("Callback on finished player");
		return;
	}

	PcmPlayer *pcmPlayer = spContext.slesPlayer->players[spContext.playerId].get();
	PcmBuffer* pcmBuffer = pcmPlayer->bufferPtr;

	auto packet = pcmBuffer->pcmStream->getPacket(++pcmPlayer->packetId);
	if (!packet && pcmPlayer->loop) {
		LOGD("Last packet, since looping, restart...");
		pcmPlayer->packetId = 0;
		packet = pcmBuffer->pcmStream->getPacket(0);
		if (!packet) {
			throw std::runtime_error("Unexpected empty pcm packet when looping");
		}
	} else if(!packet) {
		// We're done playing.
		LOGD("Last packet, done playing");
		spContext.slesPlayer->finishedPlayIds.push_back(spContext.playerId);

		return;
	}

	SL_EXECUTE((*bqItf)->Enqueue, bqItf, &((*packet)[0]), (*packet).size());
}

uint32_t SLESPlayer::playAudioImpl(uint32_t audioId, bool loop) {
	boost::lock_guard<boost::recursive_mutex> lock(SLESPlayer::slesMutex);

	for (auto playId : finishedPlayIds) {
		stopAudioImpl(playId);
	}
	finishedPlayIds.clear();

	if (buffers.find(audioId) == buffers.end()) {
		throw std::runtime_error("SLESPlayer: Unknown buffer id");
	}
	++playerCount;

	auto &buffer = buffers[audioId];

	std::unique_ptr<PcmPlayer> pcmPlayer(new PcmPlayer());

    SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 1};
    SLDataSource audioSource = {&loc_bufq, &buffer->format};

    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMix};
    SLDataSink audioSink = {&loc_outmix, nullptr};

	pcmPlayer->loop = loop;
	pcmPlayer->bufferPtr = buffer.get();
	pcmPlayer->bufferId = audioId;
	pcmPlayer->packetId = 0;

	// Creating player
	{
		const SLInterfaceID ids[1] = { SL_IID_BUFFERQUEUE, };
    	const SLboolean req[1] = { SL_BOOLEAN_TRUE };
		SL_EXECUTE((*engineEngine)->CreateAudioPlayer, engineEngine, &pcmPlayer->player,
				&audioSource, &audioSink, 1, ids, req);
		SL_EXECUTE((*pcmPlayer->player)->Realize, pcmPlayer->player, SL_BOOLEAN_FALSE);
		SL_EXECUTE((*pcmPlayer->player)->GetInterface, pcmPlayer->player, SL_IID_PLAY,
				&pcmPlayer->playItf);
		SL_EXECUTE((*pcmPlayer->player)->GetInterface, pcmPlayer->player, SL_IID_BUFFERQUEUE,
				&pcmPlayer->bufferQueueItf);

		slesPlayerContexts[pcmPlayer->bufferQueueItf] = {this, playerCount};
		SL_EXECUTE((*pcmPlayer->bufferQueueItf)->RegisterCallback, pcmPlayer->bufferQueueItf, bqCallback, nullptr);
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
	players[playerCount] = std::move(pcmPlayer);
	buffer->playIds.push_back(playerCount);

	return playerCount;
}

void SLESPlayer::pauseAudioImpl(uint32_t playId) {
	boost::lock_guard<boost::recursive_mutex> lock(SLESPlayer::slesMutex);

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
	boost::lock_guard<boost::recursive_mutex> lock(SLESPlayer::slesMutex);

	LOGD("stopAudioImpl, playId: " << playId);
	if (players.find(playId) != players.end()) {
		LOGD("stopAudioImpl");
		auto& player = players[playId];
		SLuint32 state;
		SL_EXECUTE((*player->playItf)->GetPlayState, player->playItf, &state);
		LOGD("stopAudioImpl");
		if (state == SL_PLAYSTATE_PLAYING || state == SL_PLAYSTATE_PAUSED) {
			SL_EXECUTE((*player->playItf)->SetPlayState, player->playItf, SL_PLAYSTATE_STOPPED);
		}
		LOGD("Destroy player");
		(*player->player)->Destroy(player->player);
		LOGD("stopAudioImpl");

		// Clean up book-keeping.
		auto& buffer = buffers[player->bufferId];
		LOGD("stopAudioImpl");
		slesPlayerContexts.erase(player->bufferQueueItf);
		LOGD("stopAudioImpl");
		erase(buffer->playIds, playId);
		LOGD("stopAudioImpl");
		players.erase(playId);
		LOGD("stopAudioImpl");
	}
}

bool SLESPlayer::isPlayingImpl(uint32_t playId) const {
	boost::lock_guard<boost::recursive_mutex> lock(SLESPlayer::slesMutex);

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
