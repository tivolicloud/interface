#include "Brotli.h"

// #include "brotli/encode.h"
#include "brotli/decode.h"

// bool brotliCompress(QByteArray source, QByteArray &destination, int quality) {
// }

bool brotliDecompress(QByteArray source, QByteArray& destination) {
	auto state = BrotliDecoderCreateInstance(nullptr, nullptr, nullptr);
	if (!state) return false;
	
	BrotliDecoderSetParameter(state, BROTLI_DECODER_PARAM_LARGE_WINDOW, 1u);

	destination = QByteArray();
	destination.resize(4096);

	size_t availableIn = source.size();
	const uint8_t* nextIn = reinterpret_cast<const uint8_t*>(source.constData());

	size_t availableOut = destination.size();
	uint8_t* nextOut = reinterpret_cast<uint8_t*>(destination.data());

	bool success = false;

	for (;;) {
		auto result = BrotliDecoderDecompressStream(
			state, &availableIn, &nextIn, &availableOut, &nextOut, nullptr
		);
		if (result == BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT) {
			size_t offset = destination.size() - availableOut;
			availableOut += destination.size();
			destination.resize(destination.size() * 2);
			nextOut = reinterpret_cast<uint8_t*>(destination.data()) + offset;
		} else if (result == BROTLI_DECODER_RESULT_SUCCESS) {
			success = true;
			destination.resize(destination.size() - availableOut);
			break;
		} else {
			success = false;
			destination.clear();
			break;
		}
	}

	BrotliDecoderDestroyInstance(state);

	return success;
}