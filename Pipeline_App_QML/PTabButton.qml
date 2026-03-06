import QtQuick 2.12
import QtQuick.Controls as QC

QC.TabButton {
    id: controlOutput
    height: 24

    background: Rectangle {
        radius: 3
        color: controlOutput.checked
               ? "#707070"
               : controlOutput.hovered
                    ? "#3a3a3a"
                    : "#505050"

        border.color: "#656565"
    }

    contentItem: Text {
        text: controlOutput.text
        color: "#e0e0e0"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.bold: controlOutput.checked
    }
}
