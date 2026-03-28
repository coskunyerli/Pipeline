import QtQuick 2.12

Rectangle
{
    id : rootItem
    property var model
    color:"#202020"
    ListView
    {
        id:nodeListView
        anchors.fill:parent
        model: rootItem.model

        delegate: Rectangle {
            id: root
            width: ListView.view.width
            height: 64
            radius: 6

            color: hovered ? "#3a3a3a" : "#2b2b2b"
            border.color: "#444"

            // Delegate içindeki en üst seviyeye ekle
            Rectangle {
                id: closeButton
                width: 16
                height: 16
                radius: 8
                color: "#202020"      // arkaplan kırmızı
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.topMargin: 4
                anchors.rightMargin: 4
                z: 10                 // diğer itemların üstünde

                Image {
                    anchors.fill: parent
                    source: "qrc:/icons/close_24.svg"
                    fillMode: Image.PreserveAspectFit
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        rootItem.model.removeNode(rootItem.model.index(index,0))
                    }
                }
            }

            property bool hovered: false

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                hoverEnabled: true
                drag.target: dragItem   // 👈 drag başlatmak için

                onPressed: {
                    dragItem.Drag.imageSource = "";

                    root.grabToImage(function(result) {
                        dragItem.Drag.imageSource = result.url
                    }, Qt.size(root.width, root.height))

                }

                onEntered: root.hovered = true
                onExited: root.hovered = false
            }

            // 🔥 Drag edilen görsel (ghost)
            Rectangle {
                id: dragItem
                width: root.width
                height: root.height
                visible: false

                Drag.active: mouseArea.drag.active
                Drag.hotSpot.x: width / 2
                Drag.hotSpot.y: height / 2

                // 🔥 En önemli kısım: taşınan veri
                Drag.mimeData: {
                    "application/node": JSON.stringify({
                            name: model.name,
                            type: model.type,
                            hotX: root.width / 2,
                            hotY: root.height / 2
                        })
                }

                Drag.dragType: Drag.Automatic
            }

            Row {
                anchors.fill: parent
                anchors.margins: 8
                spacing: 10

                Rectangle {
                    width: 40
                    height: 40
                    radius: 4
                    color: "#555"

                    Text {
                        anchors.centerIn: parent
                        text: model.shortName || "N"
                        color: "white"
                        font.bold: true
                    }
                }

                Column {
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 2

                    Text {
                        text: model.name
                        color: "#e0e0e0"
                        font.pixelSize: 14
                        font.bold: true
                    }

                    Text {
                        text: "This is test node description"
                        color: "#a0a0a0"
                        font.pixelSize: 11
                        elide: Text.ElideRight
                        width: 160
                    }
                }
            }
        }
    }
}
