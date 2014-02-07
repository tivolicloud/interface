//
//  Agent.h
//  hifi
//
//  Created by Stephen Birarda on 7/1/13.
//  Copyright (c) 2013 HighFidelity, Inc. All rights reserved.
//

#ifndef __hifi__Agent__
#define __hifi__Agent__

#include <vector>

#include <QtScript/QScriptEngine>
#include <QtCore/QObject>
#include <QtCore/QUrl>

#include <ParticleEditPacketSender.h>
#include <ParticleTree.h>
#include <ScriptEngine.h>
#include <ThreadedAssignment.h>
#include <VoxelEditPacketSender.h>


class Agent : public ThreadedAssignment {
    Q_OBJECT
    
    Q_PROPERTY(bool isAvatar READ isAvatar WRITE setIsAvatar)
public:
    Agent(const QByteArray& packet);
    
    void setIsAvatar(bool isAvatar) { _scriptEngine.setIsAvatar(isAvatar); }
    bool isAvatar() const { return _scriptEngine.isAvatar(); }
    
public slots:
    void run();
    
    void readPendingDatagrams();
signals:
    void willSendAudioDataCallback();
    void willSendVisualDataCallback();
private:
    ScriptEngine _scriptEngine;
    ParticleTree _particleTree;
    VoxelEditPacketSender _voxelEditSender;
    ParticleEditPacketSender _particleEditSender;
};

#endif /* defined(__hifi__Agent__) */
