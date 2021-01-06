#ifndef tivoli_Brotli_h
#define tivoli_Brotli_h

#include <QtCore/QByteArray>

// Quality is between 0 to 11
// 0 gives no compression at all
// 1 gives best speed
// 11 gives best compression
// -1 is default, currently ?

// bool brotliCompress(QByteArray source, QByteArray& destination, int quality = -1);

bool brotliDecompress(QByteArray source, QByteArray& destination);

#endif // tivoli_Brotli_h
