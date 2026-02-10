// ExcelTableView.qml
import QtQuick
import QtQuick.Controls
import Qt.labs.qmlmodels

Rectangle {
    id: root

    // Renk paleti
    readonly property color bgDark: "#303030"
    readonly property color bgMid: "#404040"
    readonly property color bgLight: "#505050"
    readonly property color textColor: "#e0e0e0"
    readonly property color borderColor: "#505050"
    readonly property color selectionColor: "#0078d4"
    readonly property color hoverColor: "#3a3a3a"

    // Public API
    property alias model: tableView.model
    property alias selectionModel: tableView.selectionModel
    property int columnWidth: 100
    property int rowHeight: 30
    property int rowHeaderWidth: 24
    property int selectedRow: -1
    property int selectedCol: -1

    // Signals
    signal cellClicked(int row, int column)
    signal cellEdited(int row, int column, string value)
    color : bgDark

    function selectCell(row, col) {
        selectedRow = row
        selectedCol = col
    }

    function clearSelection() {
        selectedRow = -1
        selectedCol = -1
    }

    HorizontalHeaderView {
        id: horizontalHeader
        anchors.left: tableView.left
        anchors.top: parent.top
        syncView: tableView
        clip: true

        delegate: Label {
            required property var index
            required property string display

            text: display
            color: textColor
            font.bold: true
            padding: 8
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter

            background: Rectangle {
                color: bgMid
                border.color: borderColor
            }
        }
    }

    VerticalHeaderView {
       id: verticalHeader
       anchors.top: tableView.top
       anchors.left: parent.left
       syncView: tableView
       clip: true

       delegate: Label {
           required property var index
           required property string display

           text: row + 1
           color: textColor
           font.bold: true
           padding: 8
           horizontalAlignment: Text.AlignHCenter
           verticalAlignment: Text.AlignVCenter

           background: Rectangle {
               color: bgMid
               border.color: borderColor
           }
       }
   }

    // Ana tablo
    TableView {
        id: tableView
        anchors.left: verticalHeader.right
        anchors.top: horizontalHeader.bottom
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        clip: true
        boundsBehavior: Flickable.StopAtBounds

        selectionModel: ItemSelectionModel {
            model: tableView.model
        }

        delegate: Rectangle {
            implicitWidth: columnWidth
            implicitHeight: rowHeight

            color: {
                        if (selectedRow === row && selectedCol === column)
                            return selectionColor
                        if (selectedRow === row || selectedCol === column)
                            return hoverColor
                        return bgDark
                    }

            border.color: (selectedRow === row && selectedCol === column)
                                     ? selectionColor : borderColor
            border.width: (selectedRow === row && selectedCol === column) ? 2 : 1

            TextInput {
                id: cellInput
                anchors.fill: parent
                anchors.margins: 4
                verticalAlignment: Text.AlignVCenter
                text: display ?? ""
                selectByMouse: true
                color: textColor
                selectionColor: selectionColor
                selectedTextColor: "#ffffff"
                onTextEdited: {
                    display = text
                    root.cellEdited(row, column, text)
                }

                onActiveFocusChanged: {
                    if (activeFocus) {
                        selectedRow = row
                        selectedCol = column
                    }
                }
            }

            MouseArea {
                anchors.fill: parent
                propagateComposedEvents: true
                onClicked: mouse => {
                    selectedRow = row
                    selectedCol = column
                    cellInput.forceActiveFocus()
                    root.cellClicked(row, column)
                    mouse.accepted = false
                }
            }
        }
    }

}
