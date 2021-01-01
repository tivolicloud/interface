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
	QList<QString> extensions;

	int count;
	sf_command(nullptr, SFC_GET_FORMAT_MAJOR_COUNT, &count, sizeof(int));
	
	for (int i = 0; i < count; i++) {
		SF_FORMAT_INFO info;
		info.format = i;
        sf_command(nullptr, SFC_GET_FORMAT_MAJOR, &info, sizeof(info));
		extensions.push_back(QString(info.extension).toLower());
	};

	return extensions;
}

std::pair<QByteArray, SoundProcessor::AudioProperties> SndfileConverter::convertToPcm(QByteArray input) {
	// https://gist.github.com/dtinth/1177001

	// create input file

	SF_INFO inputInfo;
    inputInfo.format = SF_FORMAT_OGG;

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

	short samples[2048];
	for (;;) {
		int items = sf_read_short(inputFile, samples, 2048);
		if (items > 0) {
			sf_write_short(outputFile, samples, items);
		} else {
			break;
		}
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