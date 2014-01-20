#include "VorbisDecoder.h"

#include "../../util/Log.h"

#include <stdexcept>

namespace rocket { namespace resource { namespace audio {

// typedef struct {
//   size_t (*read_func)  (void *ptr, size_t size, size_t nmemb, void *datasource);
//   int    (*seek_func)  (void *datasource, ogg_int64_t offset, int whence);
//   int    (*close_func) (void *datasource);
//   long   (*tell_func)  (void *datasource);
// } ov_callbacks;

static size_t istream_read(void *ptr, size_t size, size_t nmemb, void *istream_ptr) {
	std::istream *is = static_cast<std::istream*>(istream_ptr);
	is->read(static_cast<char*>(ptr), size*nmemb);
	return is->gcount();
}

static ov_callbacks ISTREAM_CALLBACKS {
	(size_t (*)(void *, size_t, size_t, void *))  istream_read,
	(int (*)(void *, ogg_int64_t, int))           nullptr,
	(int (*)(void *))                             nullptr,
	(long (*)(void *))                            nullptr
};

VorbisDecoder::VorbisDecoder(std::shared_ptr<std::istream> const& is) : is(is) {
	if (ov_open_callbacks(is.get(), &vf, NULL, 0, ISTREAM_CALLBACKS) < 0) {
		throw std::runtime_error("Input does not appear to be an Ogg bitstream.\n");
	}

	pcmInfo.bitsPerSample = 16;

	vorbis_info *vi=ov_info(&vf,-1);

	pcmInfo.channels = vi->channels;
	pcmInfo.frequency = vi->rate;
}

VorbisDecoder::~VorbisDecoder() noexcept(true) {
	ov_clear(&vf);
}

PcmInfo const& VorbisDecoder::getPcmInfoImpl() const {
	return pcmInfo;
}

bool VorbisDecoder::hasMorePacketsImpl() const {
	return !is->eof();
}

//! Appends next packet onto data.
void VorbisDecoder::readNextPacketImpl(std::vector<char> &data) {
	int current_section;
	long readBytes = ov_read(&vf, readBuffer, 4096, 0, 2, 1, &current_section);
	if (readBytes < 0) {
		LOGE("Unavailable to decode ogg stream");
		throw std::runtime_error("Unavailable to decode ogg stream");
	} else if (readBytes > 0) {
		data.insert(data.end(), &(readBuffer[0]), &(readBuffer[readBytes]));
	} else {
		LOGW("No packets left to read from ogg vorbis stream");
	}
}

}}}

