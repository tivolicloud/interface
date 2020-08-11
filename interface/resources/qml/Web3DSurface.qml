//
//  Web3DSurface.qml
//
//  Created by David Rowe on 16 Dec 2016.
//  Copyright 2016 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

import QtQuick 2.5

import "controls" as Controls

Item {
    id: root
    anchors.fill: parent
    property string url: ""
    property string scriptUrl: null
    property bool transparentBackground: false

    onUrlChanged: {
        if (root.item) {
            root.item.url = root.url;
        } else {
            load();
        }
    }

    onScriptUrlChanged: {
        if (root.item) {
            root.item.scriptUrl = root.scriptUrl;
        } else {
            load();
        }
    }

    onTransparentBackgroundChanged: {
        if (root.item) {
            root.item.transparentBackground = root.transparentBackground;
        } else {
            load();
        }
    }

    property var item: null

    function load() {
        // Ensure we reset any existing item to "about:blank" to ensure web audio stops: DEV-2375
        if (root.item != null) {
            root.item.url = "about:blank"
            root.item.destroy()
            root.item = null
        }
        QmlSurface.load("./controls/WebView.qml", root, function(newItem) {
            root.item = newItem
            root.item.url = root.url
            root.item.scriptUrl = root.scriptUrl
            root.item.transparentBackground = root.transparentBackground
        })
    }

    Component.onCompleted: {
        load();
    }

    signal sendToScript(var message);
}
