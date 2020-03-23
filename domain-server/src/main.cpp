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


#include <BuildInfo.h>
#include <CrashAnnotations.h>
#include <LogHandler.h>
#include <SharedUtil.h>

#include "DomainServer.h"

int main(int argc, char* argv[]) {
    setupHifiApplication(BuildInfo::DOMAIN_SERVER_NAME);

    DomainServer::parseCommandLine(argc, argv);

    Setting::init();

    int currentExitCode = 0;

    // use a do-while to handle domain-server restart
    do {
        crash::annotations::setShutdownState(false);
        DomainServer domainServer(argc, argv);
        currentExitCode = domainServer.exec();
    } while (currentExitCode == DomainServer::EXIT_CODE_REBOOT);

    qInfo() << "Quitting.";
    return currentExitCode;
}
