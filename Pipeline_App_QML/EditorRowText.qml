import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

RowLayout {
    property string label
    property string value: ""
    spacing: 6
    Layout.fillWidth: true

    Label {
        text: label
        color: "#cccccc"
        font.pixelSize: 13
        Layout.preferredWidth: 180
    }

    TextField {
        text: value
        font.pixelSize: 13
        background: Rectangle {
            color: "#2a2a2a"
            border.color: "#3a3a3a"
            radius: 3
        }
        Layout.fillWidth: true
    }
}
