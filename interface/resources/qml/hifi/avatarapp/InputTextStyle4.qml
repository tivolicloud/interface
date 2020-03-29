import controlsUit 1.0 as HifiControlsUit
import stylesUit 1.0

import QtQuick 2.0
import QtQuick.Controls 2.2

HifiControlsUit.TextField {
    id: control
    font.family: "Roboto"
    font.pixelSize: 15;
    implicitHeight: 40

    AvatarAppStyle {
        id: style
    }
}
