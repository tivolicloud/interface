#ifndef hifi_SndfileConverter_h
#define hifi_SndfileConverter_h

#include <QtCore/QByteArray>
#include <QtCore/QBuffer>

#include <sndfile.h>

#include "Sound.h"

class SndfileConverter {
private:
	SF_VIRTUAL_IO vio;

	// https://stackoverflow.com/a/59233795
	static sf_count_t qbuffer_get_filelen(void* user_data);
	static sf_count_t qbuffer_seek(sf_count_t offset, int whence, void* user_data);
	static sf_count_t qbuffer_read(void* ptr, sf_count_t count, void* user_data);
	static sf_count_t qbuffer_write(const void* ptr, sf_count_t count, void* user_data);
	static sf_count_t qbuffer_tell(void* user_data);

public:
	SndfileConverter();

	QList<QString> getAvailableExtensions();

	std::pair<QByteArray, SoundProcessor::AudioProperties> convertToPcm(QByteArray input);
};

#endif // hifi_SndfileConverter_h