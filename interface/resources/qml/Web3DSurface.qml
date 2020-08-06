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
        load(root.url, root.scriptUrl, root.transparentBackground);
    }

    onScriptUrlChanged: {
        if (root.item) {
            root.item.scriptUrl = root.scriptUrl;
        } else {
            load(root.url, root.scriptUrl, root.transparentBackground);
        }
    }

    onTransparentBackgroundChanged: {
        if (root.item) {
            root.item.transparentBackground = root.transparentBackground;
        } else {
            load(root.url, root.scriptUrl, root.transparentBackground);
        }
    }

    property var item: null

    function load(url, scriptUrl, transparentBackground) {
        // Ensure we reset any existing item to "about:blank" to ensure web audio stops: DEV-2375
        if (root.item != null) {
            root.item.url = "about:blank"
            root.item.destroy()
            root.item = null
        }
        QmlSurface.load("./controls/WebView.qml", root, function(newItem) {
            root.item = newItem
            root.item.url = url
            root.item.scriptUrl = scriptUrl
            root.item.transparentBackground = transparentBackground
        })
    }

    Component.onCompleted: {
        load(root.url, root.scriptUrl, root.transparentBackground);
    }

    signal sendToScript(var message);
}
