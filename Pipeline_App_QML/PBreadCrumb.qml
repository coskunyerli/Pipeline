import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    width: 600
    color: "#252525"
    implicitHeight: breadcrumbRow.implicitHeight + 8

    ListModel {
        id: breadcrumbModel
        ListElement { title: "Home" }
        ListElement { title: "Projects" }
        ListElement { title: "Pipeline" }
        ListElement { title: "Nodes" }
    }
    ColumnLayout
    {
        anchors.fill: parent
        id: breadcrumbRow
        RowLayout {
            spacing: 6
            Layout.leftMargin: 4
            Repeater {
                model: breadcrumbModel

                RowLayout {
                    spacing: 4

                    Button {
                        id :button
                        text: model.title

                        background: Rectangle {
                            color: button.hovered ? "#3a3a3a" : "#252525"
                            radius: 4
                        }

                        contentItem: Text {
                            text: button.text
                            color: "#dddddd"
                            font.pixelSize: 14
                            verticalAlignment: Text.AlignVCenter
                        }

                        onClicked: {
                            console.log("Clicked:", model.title, "index:", index)
                        }
                    }

                    Text {
                        Layout.alignment: Text.AlignVCenter
                        visible: index < breadcrumbModel.count - 1
                        text: ">"
                        color: "#888888"
                        font.pixelSize: 14
                    }
                }
            }
            Item
            {
                Layout.fillWidth: true
            }
        }

    }


}
