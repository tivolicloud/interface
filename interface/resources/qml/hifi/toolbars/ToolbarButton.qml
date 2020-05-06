import QtQuick 2.5
import QtGraphicalEffects 1.12

Item {
    id: button

    readonly property int size: 48
    width: size
    height: size + 16 // text underneath

    // NOTE: These properties form part of the "TabletButtonProxy.ButtonProperties" type.
    // Keep the type's JSDoc up to date with any changes.
    // "libraries/ui/src/ui/TabletScriptingInterface.cpp"

    property color defaultCaptionColor: "#ffffff"
    property color captionColor: defaultCaptionColor
    property color backgroundColor: "#1d1f21"

    property bool buttonEnabled: true
    property bool isActive: false
    property bool isEntered: false

    property int imageOffOut: 1
    property int imageOffIn: 3
    property int imageOnOut: 0
    property int imageOnIn: 2

    property string text: ""
    property string hoverText: button.text
    property string activeText: button.text
    property string activeHoverText: button.activeText

    property string icon: "icons/tablet-icons/blank.svg"
    property string hoverIcon: button.icon
    property string activeIcon: button.icon
    property string activeHoverIcon: button.activeIcon
    property int iconPadding: 8

    property int sortOrder: 100
    property int stableOrder: 0
    property var uuid;

    signal clicked()

    function changeProperty(key, value) {
        button[key] = value;
    }

    function urlHelper(src) {
        if (src.match(/\bhttp/) || src.match(/\bfile:/)) {
            return src;
        } else {
            return "../../../" + src;
        }
    }

    function updateState() {
        if (buttonEnabled) {
            if (!button.isEntered && !button.isActive) {
                buttonState = imageOffOut;
            } else if (!button.isEntered && button.isActive) {
                buttonState = imageOnOut;
            } else if (button.isEntered && !button.isActive) {
                buttonState = imageOffIn;
            } else {
                buttonState = imageOnIn;
            }
        } else {
            buttonState = 0;
        }
    }

    onIsActiveChanged: updateState();
    onButtonEnabledChanged: updateState();

    Timer {
        id: asyncClickSender
        interval: 10
        repeat: false
        running: false
        onTriggered: {
            button.clicked();
        }
    }

    MouseArea {
        id: mouseArea
        hoverEnabled: buttonEnabled
        anchors.fill: parent
        onClicked: asyncClickSender.start();
        onEntered: {
            button.isEntered = true;
            updateState();
        }
        onExited: {
            button.isEntered = false;
            updateState();
        }
    }

    // rounded icon mask
    Rectangle {
        id: roundedIconMask
        width: button.size
        height: button.size
        radius: 4
        visible: false
    }

    // icon background
    Rectangle {
        width: button.size - (!button.isActive ? 0 : 8)
        height: button.size - (!button.isActive ? 0 : 8)
        anchors.top: parent.top
        anchors.topMargin: 0 + (!button.isActive ? 0 : 4)
        anchors.horizontalCenter: parent.horizontalCenter
        
        color: button.backgroundColor

        layer.enabled: true
        layer.effect: OpacityMask {
            maskSource: roundedIconMask
        }
    }

    readonly property int iconSize: size - (iconPadding * 2) 

    // icon image
    Image {
        id: icon
        width: button.iconSize - (!button.isActive ? 0 : 8)
        height: button.iconSize - (!button.isActive ? 0 : 8)
        anchors.top: parent.top
        anchors.topMargin: button.iconPadding + (!button.isActive ? 0 : 4)
        anchors.horizontalCenter: parent.horizontalCenter
        
        smooth: true
        fillMode: Image.PreserveAspectFit
        source: urlHelper(
            button.isActive ? 
            (button.isEntered ? button.activeHoverIcon : button.activeIcon) :
            (button.isEntered ? button.hoverIcon : button.icon)
        )
        // svg antialiasing fix
        sourceSize.width: button.iconSize
        sourceSize.height: button.iconSize

        layer.enabled: iconPadding <= 0
        layer.effect: OpacityMask {
            maskSource: roundedIconMask
        }
    }

    // caption
    Text {
        id: caption
        color: button.isActive ? "#fff" : captionColor
        text: (
            button.isActive ?
            (button.isEntered ? button.activeHoverText : button.activeText) :
            (button.isEntered ? button.hoverText : button.text)
        )
        font.bold: false
        font.pixelSize: 11
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
        anchors.horizontalCenter: parent.horizontalCenter
        horizontalAlignment: Text.AlignHCenter
    }
}

