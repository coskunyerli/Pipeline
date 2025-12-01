import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Item {
    id: section
    property string title
    property var cont: []

    width: parent.width

    ColumnLayout {
        width: parent.width

        ToolButton {
            id: header
            text: expanded ? "▼ " + section.title : "► " + section.title
            font.bold: true
            font.pixelSize: 14
            //foreground: "#dddddd"
            background: Rectangle {
                color: "#2a2a2a"
                height: 30
            }
            onClicked: expanded = !expanded
        }

        Item {
            visible: expanded
            Layout.fillWidth: true

            ColumnLayout {
                id: sectionContent
                spacing: 6
                width: parent.width
                Repeater {
                    model: section.content
                }
            }
        }
    }

    property bool expanded: true
}
