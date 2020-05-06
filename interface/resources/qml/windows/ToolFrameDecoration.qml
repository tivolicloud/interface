//
//  DefaultFrame.qml
//
//  Created by Bradley Austin Davis on 12 Jan 2016
//  Copyright 2016 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

import QtQuick 2.5
import QtGraphicalEffects 1.0

import "."
import stylesUit 1.0

Decoration {
    id: root
    HifiConstants { id: hifi }

    property bool horizontalSpacers: false
    property bool verticalSpacers: false

    // Dialog frame
    frameMargin: 8 // margin around buttons
    property int handleWidth: 8
    property int handleRadius: 4
    property int handleMargin: handleWidth + 4 // margin inwards from handles 

    frameMarginLeft: frameMargin + (horizontalSpacers ? handleMargin : 0)
    frameMarginRight: frameMargin + (horizontalSpacers ? handleMargin : 0)
    frameMarginTop: frameMargin + (verticalSpacers ? handleMargin : 0)
    frameMarginBottom: frameMargin + (verticalSpacers ? handleMargin : 0)
    radius: hifi.dimensions.borderRadius

    readonly property color handleColor: Qt.rgba(0.6,0.6,0.6,0.6)

    // onInflateDecorations: {
    //     if (!HMD.active) {
    //         return;
    //     }
    //     root.frameMargin = 18
    //     root.spacerWidth = 16
    //     root.spacerRadius = 8
    //     root.spacerMargin = 8
    // }

    // onDeflateDecorations: {
    //     root.frameMargin = 6
    //     root.spacerWidth = 8
    //     root.spacerRadius = 4
    //     root.spacerMargin = 12
    // }

    Rectangle {
        visible: horizontalSpacers
        anchors.left: parent.left
        anchors.leftMargin: 6
        anchors.verticalCenter: parent.verticalCenter
        width: root.handleWidth
        height: decoration.height - 12
        color: root.handleColor
        radius: root.handleRadius
    }

    Rectangle {
        visible: horizontalSpacers
        anchors.right: parent.right
        anchors.rightMargin: 6
        anchors.verticalCenter: parent.verticalCenter
        width: root.handleWidth
        height: decoration.height - 12
        color: root.handleColor
        radius: root.handleRadius
    }

    Rectangle {
        visible: verticalSpacers
        anchors.top: parent.top
        anchors.topMargin: 6
        anchors.horizontalCenter: parent.horizontalCenter
        height: root.handleWidth
        width: decoration.width - 12
        color: root.handleColor
        radius: root.handleRadius
    }

    Rectangle {
        visible: verticalSpacers
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 6
        anchors.horizontalCenter: parent.horizontalCenter
        height: root.handleWidth
        width: decoration.width - 12
        color: root.handleColor
        radius: root.handleRadius
    }
}

