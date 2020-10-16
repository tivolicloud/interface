import QtQuick 2.5
// import QtGraphicalEffects 1.12
import TabletScriptingInterface 1.0

Item {
    id: button

    property bool isTablet: false // is tablet
    readonly property int scale: isTablet ? 2 : 1 // make everything x time bigger

    readonly property int size: 48 * scale
    width: size
    height: size + (16 * scale) // text underneath

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

    // tablet only
    property bool inDebugMode: false // unused
    property var tabletRoot; 
    property var flickable: null // unused
    property var gridView: null 
    property int buttonIndex: -1 

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

    // function updateState() {
    //     if (buttonEnabled) {
    //         if (!button.isEntered && !button.isActive) {
    //             buttonState = imageOffOut;
    //         } else if (!button.isEntered && button.isActive) {
    //             buttonState = imageOnOut;
    //         } else if (button.isEntered && !button.isActive) {
    //             buttonState = imageOffIn;
    //         } else {
    //             buttonState = imageOnIn;
    //         }
    //     } else {
    //         buttonState = 0;
    //     }
    // }

    // onIsActiveChanged: updateState();
    // onButtonEnabledChanged: updateState();

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
        onClicked: {
            if (isTablet) {
                gridView.currentIndex = buttonIndex
                if (tabletRoot) {
                    Tablet.playSound(TabletEnums.ButtonClick);
                }
            }

            asyncClickSender.start();
        }
        onEntered: {
            if (isTablet) {
                gridView.currentIndex = buttonIndex
                Tablet.playSound(TabletEnums.ButtonHover);
            }

            button.isEntered = true;
            // updateState();
        }
        onExited: {
            button.isEntered = false;
            // updateState();
        }
    }

    // rounded icon mask
    Rectangle {
        id: roundedIconMask
        width: button.size
        height: button.size
        radius: isTablet ? 12 : 4
        visible: false
    }

    // isactive background
    Rectangle {
        width: button.size
        height: button.size
        anchors.top: parent.top
        anchors.topMargin: 0
        anchors.horizontalCenter: parent.horizontalCenter
        
        color: Qt.rgba(0.6,0.6,0.6,0.6) // same as toolframedecoration handlecolor
        visible: isActive

        // layer.enabled: true
        // layer.effect: OpacityMask {
        //     maskSource: roundedIconMask
        // }
    }

    readonly property int activePadding: 8 * scale
    readonly property int iconSize: (size - (iconPadding * 2 * scale)) 

    // icon background
    Rectangle {
        width: button.size - (!button.isActive ? 0 : button.activePadding)
        height: button.size - (!button.isActive ? 0 : button.activePadding)
        anchors.top: parent.top
        anchors.topMargin: 0 + (!button.isActive ? 0 : button.activePadding / 2)
        anchors.horizontalCenter: parent.horizontalCenter
        
        color: button.backgroundColor

        // layer.enabled: true
        // layer.effect: OpacityMask {
        //     maskSource: roundedIconMask
        // }
    }

    // icon image
    Image {
        id: icon
        width: button.iconSize - (!button.isActive ? 0 : button.activePadding)
        height: button.iconSize - (!button.isActive ? 0 : button.activePadding)
        anchors.top: parent.top
        anchors.topMargin: (button.iconPadding * button.scale) + (!button.isActive ? 0 : button.activePadding / 2)
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

        // layer.enabled: iconPadding <= 0
        // layer.effect: OpacityMask {
        //     maskSource: roundedIconMask
        // }
    }

    // hover overlay
    Rectangle {
        width: button.size - (!button.isActive ? 0 : button.activePadding)
        height: button.size - (!button.isActive ? 0 : button.activePadding)
        anchors.top: parent.top
        anchors.topMargin: 0 + (!button.isActive ? 0 : button.activePadding / 2)
        anchors.horizontalCenter: parent.horizontalCenter
        
        color: Qt.rgba(1,1,1,0.2)
        visible: isEntered

        // layer.enabled: true
        // layer.effect: OpacityMask {
        //     maskSource: roundedIconMask
        // }
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
        font.family: "Roboto Condensed"
        font.pixelSize: isTablet ? 22 : 12
        anchors.bottom: parent.bottom
        anchors.bottomMargin: -1
        anchors.horizontalCenter: parent.horizontalCenter
        horizontalAlignment: Text.AlignHCenter
    }
}

