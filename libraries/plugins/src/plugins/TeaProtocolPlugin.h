#ifndef hifi_TeaProtocolPlugin_h
#define hifi_TeaProtocolPlugin_h

// #include <functional>

#include <QUrl>

#ifndef hifi_ResourceRequest_h
#include <ResourceRequest.h>
#endif

class TeaProtocolPlugin {
public:
    virtual ResourceRequest* initRequest(
        const QUrl& url,
        const bool isObservable = true,
        const qint64 callerId = -1,
        const QString& extra = ""
    ) = 0;

};

#endif // hifi_TeaProtocolPlugin_h
