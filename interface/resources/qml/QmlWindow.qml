import QtQuick 2.3

import "windows"
import "controls"
import controlsUit 1.0 as Controls
import "styles"
import stylesUit 1.0

Window {
    id: root
    HifiConstants { id: hifi }
    title: "QmlWindow"
    resizable: true
    shown: false
    focus: true
    property var channel;
    // Don't destroy on close... otherwise the JS/C++ will have a dangling pointer
    destroyOnCloseButton: false
    property var source;
    property var dynamicContent;

    property bool frameless: false
    readonly property var _hiddenFrame: HiddenFrame { }
    readonly property var _defaultFrame: DefaultFrame { 
        property var window: root
    }
    frame: root.frameless ? _hiddenFrame : _defaultFrame;


    // Keyboard control properties in case needed by QML content.
    property bool keyboardEnabled: false
    property bool keyboardRaised: false
    property bool punctuationMode: false

    onSourceChanged: {
        if (dynamicContent) {
            dynamicContent.destroy();
            dynamicContent = null; 
        }
        QmlSurface.load(source, contentHolder, function(newObject) {
            dynamicContent = newObject;
        });
    }

    // Handle message traffic from the script that launched us to the loaded QML
    function fromScript(message) {
        if (root.dynamicContent && root.dynamicContent.fromScript) {
            root.dynamicContent.fromScript(message);
        }
        root
    }

    function clearDebugWindow() {
        if (root.dynamicContent && root.dynamicContent.clearWindow) {
            root.dynamicContent.clearWindow();
        }
    }
    
    // Handle message traffic from our loaded QML to the script that launched us
    signal sendToScript(var message);
    onDynamicContentChanged: {
        if (dynamicContent && dynamicContent.sendToScript) {
            dynamicContent.sendToScript.connect(sendToScript);
        }
    }

    Item {
        id: contentHolder
        anchors.fill: parent
    }
}
