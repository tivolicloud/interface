import QtQuick.Dialogs 1.2 as OriginalDialogs

import "dialogs"

MessageDialog {
    id: root
    objectName: "ConnectionFailureDialog"

    title: "Unable to connect to this world"
    text: "Open the \"Explore\" app on the toolbar to visit another world."
    buttons: OriginalDialogs.StandardButton.Ok
    icon: OriginalDialogs.StandardIcon.Warning
    defaultButton: OriginalDialogs.StandardButton.NoButton;
}
