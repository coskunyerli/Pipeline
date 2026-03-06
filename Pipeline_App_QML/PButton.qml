import QtQuick
import QtQuick.Controls
Button {
    id: control
    height: 28

    background: Rectangle {
        radius: 3
        color: control.down ? Qt.darker("#505050", 1.2)
                            : control.hovered ? "#3a3a3a"
                            : "#505050"
        border.color: "#656565"
    }

    contentItem: Text {
        text: control.text
        color: "#e0e0e0"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 13
    }
    onClicked: fileDialog.open()
}
