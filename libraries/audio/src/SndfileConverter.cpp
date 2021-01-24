#include "SndfileConverter.h"

#include "AudioLogging.h"

sf_count_t SndfileConverter::qbuffer_get_filelen(void* user_data) {
	QBuffer* buffer = (QBuffer*)user_data;
	return buffer->size();
}

sf_count_t SndfileConverter::qbuffer_seek(sf_count_t offset, int whence, void* user_data) {
	QBuffer* buffer = (QBuffer*)user_data;
	if (whence == SEEK_SET) {
		buffer->seek(offset);
	} else if (whence == SEEK_CUR) {
		buffer->seek(buffer->pos() + offset);
	} else if (whence == SEEK_END) {
		buffer->seek(buffer->size() + offset);
	}
	return buffer->pos();
}

sf_count_t SndfileConverter::qbuffer_read(void* ptr, sf_count_t count, void* user_data) {
	QBuffer* buffer = (QBuffer*)user_data;
	return buffer->read((char*)ptr, count);
}

sf_count_t SndfileConverter::qbuffer_write(const void* ptr, sf_count_t count, void* user_data) {
	QBuffer* buffer = (QBuffer*)user_data;
	return buffer->write((const char*)ptr, count);
}

sf_count_t SndfileConverter::qbuffer_tell(void* user_data) {
	QBuffer* buffer = (QBuffer*)user_data;
	return buffer->pos();
}

SndfileConverter::SndfileConverter() {
	vio.get_filelen = qbuffer_get_filelen;
	vio.seek = qbuffer_seek;
	vio.read = qbuffer_read;
	vio.write = qbuffer_write;
	vio.tell = qbuffer_tell;
}

QList<QString> SndfileConverter::getAvailableExtensions() {
	QList<QString> exts { "ogg", "opus", "mp3" };

	int count;
	sf_command(nullptr, SFC_GET_FORMAT_MAJOR_COUNT, &count, sizeof(int));
	
	for (int i = 0; i < count; i++) {
		SF_FORMAT_INFO info;
		info.format = i;
        sf_command(nullptr, SFC_GET_FORMAT_MAJOR, &info, sizeof(info));
		QString ext = QString(info.extension).toLower();
		if (!exts.contains(ext)) exts.push_back(ext);
	};

	return exts;
}

std::pair<QByteArray, SoundProcessor::AudioProperties> SndfileConverter::convertToPcm(QByteArray input) {
	// https://gist.github.com/dtinth/1177001

	// create input file

	SF_INFO inputInfo;

	QBuffer inputBuffer(&input);
    inputBuffer.open(QIODevice::ReadOnly);
	
	SNDFILE* inputFile = sf_open_virtual(&vio, SFM_READ, &inputInfo, &inputBuffer);
	if (inputFile == NULL) {
		qCWarning(audio) << "Failed to read audio!" << sf_strerror(NULL);
		return std::pair<QByteArray, SoundProcessor::AudioProperties>(
			QByteArray(), SoundProcessor::AudioProperties()
		);
	}

	// create output file

	SF_INFO outputInfo;
    outputInfo.format = SF_FORMAT_RAW | SF_FORMAT_PCM_16 | SF_ENDIAN_LITTLE;
	outputInfo.samplerate = inputInfo.samplerate;
	outputInfo.channels = inputInfo.channels;

	QBuffer outputBuffer;
    outputBuffer.open(QIODevice::WriteOnly);

	SNDFILE* outputFile = sf_open_virtual(&vio, SFM_WRITE, &outputInfo, &outputBuffer);
	if (outputFile == NULL) {
		qCWarning(audio) << "Failed to create audio!" << sf_strerror(NULL);
		return std::pair<QByteArray, SoundProcessor::AudioProperties>(
			QByteArray(), SoundProcessor::AudioProperties()
		);
	}

	// convert

	// https://github.com/libsndfile/libsndfile/issues/194

	int subformat = inputInfo.format & SF_FORMAT_SUBMASK;
	bool normalize = (
		subformat == SF_FORMAT_FLOAT || subformat == SF_FORMAT_DOUBLE ||
		subformat == SF_FORMAT_VORBIS || subformat == SF_FORMAT_OPUS
	);

	double scale;
	if (normalize) {
		sf_command(inputFile, SFC_CALC_SIGNAL_MAX, &scale, sizeof(scale));
		scale = scale > 1.0 ? 1.0 / scale : 1.0;
	}

	double samples[2048];
	for (;;) {
		int items = sf_read_double(inputFile, samples, 2048);
		if (items == 0) break;
		if (normalize) {
			for (size_t i = 0; i < 2048; i++) {
				samples[i] *= scale;
			}
		}
		sf_write_double(outputFile, samples, items);
	}

	// clean up and return

	auto data = outputBuffer.data();

	auto properties = SoundProcessor::AudioProperties();
	properties.sampleRate = outputInfo.samplerate;
	properties.numChannels = outputInfo.channels;

	sf_close(inputFile);
	sf_close(outputFile);

	inputBuffer.close();
	outputBuffer.close();

	return std::pair<QByteArray, SoundProcessor::AudioProperties>(
		data, properties
	);
}