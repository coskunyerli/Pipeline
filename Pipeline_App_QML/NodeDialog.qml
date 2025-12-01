import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

Window {
    id: customDialog
    width: 400
    height: 220
    flags: Qt.Window | Qt.CustomizeWindowHint
    modality: Qt.NonModal
    visible: true

    // Loader'ın kapandığını bilmesi için sinyal
    signal dialogClosed()

    onClosing: dialogClosed()

    Rectangle {
        anchors.fill: parent
        color: "white"
        border.color: "#888"

        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            // Başlık çubuğu
            Rectangle {
                id: titleBar
                height: 32
                Layout.fillWidth: true
                color: "#e0e0e0"

                RowLayout {
                    anchors.fill: parent
                    spacing: 8
                    Label {
                        text: "Custom Frame Dialog"
                        font.bold: true
                        verticalAlignment: Text.AlignVCenter
                        Layout.fillWidth: true
                        leftPadding: 8
                    }
                    Button {
                        text: "X"
                        //background: Rectangle { color: "transparent" }
                        onClicked: customDialog.close()
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    onPressed: customDialog.startSystemMove()
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "white"
                Label {
                    anchors.centerIn: parent
                    text: "Windows snap ve resize destekli custom frame"
                }
            }
        }

        // Kenarlardan resize alanları
        MouseArea { anchors.left: parent.left; width: 5; anchors.top: parent.top; anchors.bottom: parent.bottom; cursorShape: Qt.SizeHorCursor; onPressed: customDialog.startSystemResize(Qt.LeftEdge) }
        MouseArea { anchors.right: parent.right; width: 5; anchors.top: parent.top; anchors.bottom: parent.bottom; cursorShape: Qt.SizeHorCursor; onPressed: customDialog.startSystemResize(Qt.RightEdge) }
        MouseArea { anchors.top: parent.top; height: 5; anchors.left: parent.left; anchors.right: parent.right; cursorShape: Qt.SizeVerCursor; onPressed: customDialog.startSystemResize(Qt.TopEdge) }
        MouseArea { anchors.bottom: parent.bottom; height: 5; anchors.left: parent.left; anchors.right: parent.right; cursorShape: Qt.SizeVerCursor; onPressed: customDialog.startSystemResize(Qt.BottomEdge) }

        // Köşeler
        MouseArea { anchors.left: parent.left; anchors.top: parent.top; width: 8; height: 8; cursorShape: Qt.SizeFDiagCursor; onPressed: customDialog.startSystemResize(Qt.TopEdge | Qt.LeftEdge) }
        MouseArea { anchors.right: parent.right; anchors.top: parent.top; width: 8; height: 8; cursorShape: Qt.SizeBDiagCursor; onPressed: customDialog.startSystemResize(Qt.TopEdge | Qt.RightEdge) }
        MouseArea { anchors.left: parent.left; anchors.bottom: parent.bottom; width: 8; height: 8; cursorShape: Qt.SizeBDiagCursor; onPressed: customDialog.startSystemResize(Qt.BottomEdge | Qt.LeftEdge) }
        MouseArea { anchors.right: parent.right; anchors.bottom: parent.bottom; width: 8; height: 8; cursorShape: Qt.SizeFDiagCursor; onPressed: customDialog.startSystemResize(Qt.BottomEdge | Qt.RightEdge) }
    }
}
