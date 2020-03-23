//  Tivoli Cloud VR
//  Copyright (C) 2020, Tivoli Cloud VR, Inc
//  
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Affero General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Affero General Public License for more details.
//  
//  You should have received a copy of the GNU Affero General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//  



#ifndef hifi_ACClientApp_h
#define hifi_ACClientApp_h

#include <QCoreApplication>
#include <udt/Constants.h>
#include <udt/Socket.h>
#include <ReceivedMessage.h>
#include <NetworkPeer.h>
#include <NodeList.h>


class ACClientApp : public QCoreApplication {
    Q_OBJECT
public:
    ACClientApp(int argc, char* argv[]);
    ~ACClientApp();

private slots:
    void domainConnectionRefused(const QString& reasonMessage, int reasonCodeInt, const QString& extraInfo);
    void domainChanged(QUrl domainURL);
    void nodeAdded(SharedNodePointer node);
    void nodeActivated(SharedNodePointer node);
    void nodeKilled(SharedNodePointer node);
    void notifyPacketVersionMismatch();

private:
    NodeList* _nodeList;
    void timedOut();
    void printFailedServers();
    void finish(int exitCode);
    bool _verbose;

    bool _sawEntityServer { false };
    bool _sawAudioMixer { false };
    bool _sawAvatarMixer { false };
    bool _sawAssetServer { false };
    bool _sawMessagesMixer { false };

    QString _username;
    QString _password;
};

#endif //hifi_ACClientApp_h
