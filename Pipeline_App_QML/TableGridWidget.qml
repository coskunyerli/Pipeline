import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.12
Rectangle {
    id: excelWidget
    width: 900
    height: 600
    color: "#f0f0f0"

    // Tablo ayarları
    property int rowCount: 100
    property int columnCount: 26
    property int defaultColumnWidth: 100
    property int defaultRowHeight: 30
    property int headerSize: 30

    // Seçili hücre
    property int selectedRow: -1
    property int selectedCol: -1

    // JavaScript veri modeli
    property var cellData: ({})

    // Sütun genişlikleri
    property var columnWidths: ({})

    // Sütun harfini al (A, B, C, ... Z, AA, AB, ...)
    function getColumnLabel(col) {
        var label = ""
        var num = col
        while (num >= 0) {
            label = String.fromCharCode(65 + (num % 26)) + label
            num = Math.floor(num / 26) - 1
        }
        return label
    }

    // Hücre değerini al
    function getCellValue(row, col) {
        var key = row + "," + col
        return cellData[key] || ""
    }

    // Hücre değerini ayarla
    function setCellValue(row, col, value) {
        var key = row + "," + col
        cellData[key] = value
    }

    // Sütun genişliğini al
    function getColumnWidth(col) {
        return columnWidths[col] || defaultColumnWidth
    }

    // Sütun genişliğini ayarla
    function setColumnWidth(col, width) {
        columnWidths[col] = Math.max(50, width)
        columnHeaderView.positionViewAtIndex(col, ListView.Visible)
        tableView.forceLayout()
    }

    // Tüm veriyi temizle
    function clearAll() {
        cellData = {}
        tableView.forceLayout()
    }

    // CSV formatında veri al
    function getCSV() {
        var csv = []
        for (var row = 0; row < rowCount; row++) {
            var rowData = []
            for (var col = 0; col < columnCount; col++) {
                var value = getCellValue(row, col)
                if (value.includes(',') || value.includes('"') || value.includes('\n')) {
                    value = '"' + value.replace(/"/g, '""') + '"'
                }
                rowData.push(value)
            }
            csv.push(rowData.join(','))
        }
        return csv.join('\n')
    }

    Column {
        anchors.fill: parent
        spacing: 0

        // Üst araç çubuğu
        Rectangle {
            width: parent.width
            height: 45
            color: "#e0e0e0"
            border.color: "#c0c0c0"
            border.width: 1

            Row {
                anchors.fill: parent
                anchors.margins: 8
                spacing: 10

                Label {
                    anchors.verticalCenter: parent.verticalCenter
                    text: "📊 Excel Tablosu"
                    font.bold: true
                    font.pixelSize: 16
                }

                Rectangle {
                    anchors.verticalCenter: parent.verticalCenter
                    width: 1
                    height: 30
                    color: "#c0c0c0"
                }

                Button {
                    anchors.verticalCenter: parent.verticalCenter
                    text: "Temizle"
                    onClicked: clearAll()
                }

                Button {
                    anchors.verticalCenter: parent.verticalCenter
                    text: "CSV Yazdır"
                    onClicked: {
                        console.log("=== CSV Verisi ===")
                        console.log(getCSV())
                    }
                }

                Item {
                    width: parent.width - 400
                    height: 1
                }

                Label {
                    anchors.verticalCenter: parent.verticalCenter
                    text: rowCount + " × " + columnCount
                    font.pixelSize: 12
                    color: "#666"
                }
            }
        }

        // Tablo alanı container
        Item {
            width: parent.width
            height: parent.height - 45 - 28

            // Sol üst köşe
            Rectangle {
                id: cornerCell
                x: 0
                y: 0
                width: headerSize
                height: headerSize
                color: "#d8d8d8"
                border.color: "#999"
                border.width: 1
                z: 10
            }

            // Sütun başlıkları
            ListView {
                id: columnHeaderView
                anchors.left: cornerCell.right
                anchors.top: parent.top
                width: parent.width - headerSize
                height: headerSize
                orientation: ListView.Horizontal
                interactive: false
                clip: true
                z: 5

                // TableView ile senkronize scroll
                contentX: tableView.contentX

                model: columnCount

                delegate: Rectangle {
                    width: getColumnWidth(index)
                    height: headerSize
                    color: "#d8d8d8"
                    border.color: "#999"
                    border.width: 1

                    Label {
                        anchors.centerIn: parent
                        text: getColumnLabel(index)
                        font.bold: true
                        font.pixelSize: 12
                    }

                    // Sütun genişliği ayarlama
                    MouseArea {
                        anchors.right: parent.right
                        width: 8
                        height: parent.height
                        cursorShape: Qt.SplitHCursor

                        property real startX: 0
                        property real startWidth: 0

                        onPressed: function(mouse) {
                            startX = mouse.x
                            startWidth = getColumnWidth(index)
                        }

                        onPositionChanged: function(mouse) {
                            if (pressed) {
                                var delta = mouse.x - startX
                                setColumnWidth(index, startWidth + delta)
                            }
                        }
                    }
                }
            }

            // Satır başlıkları
            ListView {
                id: rowHeaderView
                anchors.left: parent.left
                anchors.top: cornerCell.bottom
                width: headerSize
                height: parent.height - headerSize
                orientation: ListView.Vertical
                interactive: false
                clip: true
                z: 5

                // TableView ile senkronize scroll
                contentY: tableView.contentY

                model: rowCount

                delegate: Rectangle {
                    width: headerSize
                    height: defaultRowHeight
                    color: "#d8d8d8"
                    border.color: "#999"
                    border.width: 1

                    Label {
                        anchors.centerIn: parent
                        text: index + 1
                        font.bold: true
                        font.pixelSize: 12
                    }
                }
            }

            // Ana tablo
            TableView {
                id: tableView
                anchors.left: rowHeaderView.right
                anchors.top: columnHeaderView.bottom
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                clip: true

                rowSpacing: 0
                columnSpacing: 0

                columnWidthProvider: function(column) {
                    return getColumnWidth(column)
                }

                rowHeightProvider: function(row) {
                    return defaultRowHeight
                }

                model: rowCount * columnCount

                // Header'ları senkronize et
                onContentXChanged: {
                    columnHeaderView.contentX = contentX
                }

                onContentYChanged: {
                    rowHeaderView.contentY = contentY
                }

                delegate: Rectangle {
                    id: cellDelegate
                    implicitWidth: getColumnWidth(column)
                    implicitHeight: defaultRowHeight

                    required property int index
                    property int row: Math.floor(index / columnCount)
                    property int column: index % columnCount

                    color: {
                        if (row === selectedRow && column === selectedCol) {
                            return "#cce5ff"
                        }
                        return "white"
                    }
                    border.color: "#d0d0d0"
                    border.width: 1

                    TextInput {
                        id: cellInput
                        anchors.fill: parent
                        anchors.margins: 4
                        verticalAlignment: Text.AlignVCenter
                        text: getCellValue(cellDelegate.row, cellDelegate.column)
                        font.pixelSize: 12
                        selectByMouse: true
                        clip: true

                        onTextEdited: {
                            setCellValue(cellDelegate.row, cellDelegate.column, text)
                        }

                        onActiveFocusChanged: {
                            if (activeFocus) {
                                selectedRow = cellDelegate.row
                                selectedCol = cellDelegate.column
                                selectAll()
                            }
                        }

                        Keys.onPressed: function(event) {
                            var handled = false

                            if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                                if (cellDelegate.row < rowCount - 1) {
                                    selectedRow = cellDelegate.row + 1
                                    selectedCol = cellDelegate.column
                                    handled = true
                                }
                            } else if (event.key === Qt.Key_Tab) {
                                if (cellDelegate.column < columnCount - 1) {
                                    selectedRow = cellDelegate.row
                                    selectedCol = cellDelegate.column + 1
                                } else if (cellDelegate.row < rowCount - 1) {
                                    selectedRow = cellDelegate.row + 1
                                    selectedCol = 0
                                }
                                handled = true
                            } else if (event.key === Qt.Key_Up && !event.isAutoRepeat) {
                                if (cellDelegate.row > 0) {
                                    selectedRow = cellDelegate.row - 1
                                    selectedCol = cellDelegate.column
                                    handled = true
                                }
                            } else if (event.key === Qt.Key_Down && !event.isAutoRepeat) {
                                if (cellDelegate.row < rowCount - 1) {
                                    selectedRow = cellDelegate.row + 1
                                    selectedCol = cellDelegate.column
                                    handled = true
                                }
                            } else if (event.key === Qt.Key_Left && cursorPosition === 0 && !event.isAutoRepeat) {
                                if (cellDelegate.column > 0) {
                                    selectedRow = cellDelegate.row
                                    selectedCol = cellDelegate.column - 1
                                    handled = true
                                }
                            } else if (event.key === Qt.Key_Right && cursorPosition === text.length && !event.isAutoRepeat) {
                                if (cellDelegate.column < columnCount - 1) {
                                    selectedRow = cellDelegate.row
                                    selectedCol = cellDelegate.column + 1
                                    handled = true
                                }
                            }

                            if (handled) {
                                event.accepted = true
                                tableView.forceLayout()
                            }
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            cellInput.forceActiveFocus()
                        }
                        onPressed: function(mouse) {
                            mouse.accepted = false
                        }
                    }
                }
            }
        }

        // Alt durum çubuğu
        Rectangle {
            width: parent.width
            height: 28
            color: "#e8e8e8"
            border.color: "#c0c0c0"
            border.width: 1

            Row {
                anchors.fill: parent
                anchors.margins: 6
                spacing: 15

                Label {
                    anchors.verticalCenter: parent.verticalCenter
                    text: selectedRow >= 0 && selectedCol >= 0 ?
                          "🎯 " + getColumnLabel(selectedCol) + (selectedRow + 1) :
                          "✓ Hazır"
                    font.pixelSize: 12
                }

                Rectangle {
                    anchors.verticalCenter: parent.verticalCenter
                    width: 1
                    height: 20
                    color: "#ccc"
                }

                Label {
                    anchors.verticalCenter: parent.verticalCenter
                    text: "Satır: " + (selectedRow + 1) + " / " + rowCount
                    font.pixelSize: 11
                    color: "#666"
                    visible: selectedRow >= 0
                }

                Label {
                    anchors.verticalCenter: parent.verticalCenter
                    text: "Sütun: " + getColumnLabel(selectedCol) + " (" + (selectedCol + 1) + ")"
                    font.pixelSize: 11
                    color: "#666"
                    visible: selectedCol >= 0
                }

                Item {
                    width: parent.parent.width - 500
                    height: 1
                }

                Label {
                    anchors.verticalCenter: parent.verticalCenter
                    text: "💾 Qt 6 TableView"
                    font.pixelSize: 11
                    color: "#888"
                }
            }
        }
    }
}
