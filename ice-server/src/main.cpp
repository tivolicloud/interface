//
//  main.cpp
//  ice-server/src
//
//  Created by Stephen Birarda on 10/01/12.
//
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


#include <QtCore/QCoreApplication>

#include <SharedUtil.h>

#include "IceServer.h"

int main(int argc, char* argv[]) {
    setupHifiApplication("Ice Server");
    
    IceServer iceServer(argc, argv);
    return iceServer.exec();
}
