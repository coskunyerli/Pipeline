import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

RowLayout {
    property string label
    property var options: []
    property int selected: 0
    spacing: 6
    Layout.fillWidth: true

    Label {
        text: label
        color: "#cccccc"
        font.pixelSize: 13
        Layout.preferredWidth: 180
    }

    ComboBox {
        model: options
        currentIndex: selected
        Layout.fillWidth: true
        font.pixelSize: 13
        background: Rectangle {
            color: "#2a2a2a"
            border.color: "#444"
            radius: 3
        }
    }
}
