import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

RowLayout {
    property string label
    property bool checked: false
    spacing: 6
    Layout.fillWidth: true

    Label {
        text: label
        color: "#cccccc"
        font.pixelSize: 13
        Layout.preferredWidth: 180
    }

    CheckBox {
        id: root
        checked: parent.checked

        // Rectangle{
        //     anchors.fill: parent
        //     color:"red"
        // }

        indicator: Item {
            width: 18
            height: 18

            Rectangle {
                id: box
                anchors.fill: parent
                radius: 3
                color: "transparent"
                border.color: root.hovered ? "#5A5A5A" : "#444"
                border.width: 1

                Text {
                    anchors.centerIn: parent
                    visible: root.checked
                    text: "\u2713" // Unicode for checkmark (✓)
                    color: "#004d99"
                    font.pixelSize: 14
                    font.bold: true
                }
            }
        }

        font.pixelSize: 14
        background: Rectangle {
            color: "transparent"
        }
    }



}
