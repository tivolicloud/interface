//
//  FileLogger.h
//  interface/src
//
//  Created by Stojce Slavkovski on 12/22/13.
//  Copyright 2013 High Fidelity, Inc.
//
//  Modified by Caitlyn Meeks on 12/24/19
//  Copyright 2019 Tivoli Cloud VR

#ifndef hifi_FileLogger_h
#define hifi_FileLogger_h

#include "AbstractLoggerInterface.h"
#include "../GenericQueueThread.h"

#include <QtCore/QFile>

class FileLogger : public AbstractLoggerInterface {
    Q_OBJECT

public:
    FileLogger(QObject* parent = NULL);
    virtual ~FileLogger();

    QString getFilename() const { return _fileName; }
    virtual void addMessage(const QString&) override;
    virtual void setSessionID(const QUuid&);
    virtual QString getLogData() override;
    virtual void locateLog() override;
    virtual void sync() override;

signals:
    void rollingLogFile(QString newFilename);

private:
    const QString _fileName;
    friend class FilePersistThread;
};



#endif // hifi_FileLogger_h
