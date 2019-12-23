//
//  TabletAssetDialog.qml
//
//  Created by David Rowe on 18 Apr 2017
//  Copyright 2017 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

import QtQuick 2.5
import stylesUit 1.0

Rectangle {
    width: 480
    height: 706

    color: "#404040"

    Column {
        x: 45
        y: 30
        spacing: 5


        Item { height: 30; width: 1 }
        Column {
            id: buildColumm
            anchors.left: parent.left
            anchors.leftMargin: 70
            RalewayRegular {
                text: "Build " + HiFiAbout.buildVersion
                size: 16
                color: "white"
            }
            RalewayRegular {
                text: "Released " + HiFiAbout.buildDate
                size: 16
                color: "white"
            }
        }
        Item { height: 10; width: 1 }
        RalewayRegular {
            text: "Tivoli Cloud VR"
            size: 20
            color: "white"
        }
        RalewayRegular {
            textFormat: Text.StyledText
            linkColor: "#00B4EF"
            color: "white"
            text: "<a href=\"https:/www.tivolicloud.com\">www.tivolicloud.com</a>."
            size: 20
            onLinkActivated: {
                HiFiAbout.openUrl("https:/www.tivolicloud.com");
            }
        }
        Item { height: 40; width: 1 }
        Row {
            spacing: 5
            Image {
                sourceSize.width: 34
                sourceSize.height: 25
                source: "../../../images/about-qt.png"
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        HiFiAbout.openUrl("https://www.qt.io/");
                    }
                }
            }
            RalewayRegular {
                color: "white"
                text: "Built the High Fidelity architecture & Qt " + HiFiAbout.qtVersion
                size: 12
                anchors.verticalCenter: parent.verticalCenter
            }
            Item { height: 1; width: 15 }
            Image {
                sourceSize.width: 70
                sourceSize.height: 26
                source: "../../../images/about-physics.png"
            }
            RalewayRegular {
                color: "white"
                text: "Physics powered by Bullet"
                size: 12
                anchors.verticalCenter: parent.verticalCenter
            }
        }
        Item { height: 20; width: 1 }
        
        RalewayRegular {
            color: "white"
            text: "Copyright 2020 Tivoli Cloud VR, Inc. All rights reserved."
            size: 14
        }
    }
}
