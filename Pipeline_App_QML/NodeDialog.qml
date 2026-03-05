import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import QtQuick.Dialogs

Window {
    id: detachedDialog
    width: 1100
    height: 650
    title: "Python Node Dialog"
    flags: Qt.Dialog | Qt.WindowCloseButtonHint | Qt.WindowTitleHint
    modality: Qt.NonModal
    visible: true
    color: "#252525"

    signal dialogClosed()

    property alias inputModel : inputTable.model
    property alias outputModel : outputTable.model
    property alias pythonFilename : pythonFilenameTextEdit.text

    onClosing: dialogClosed()

    FileDialog {
        id: fileDialog
        title: "Select File"
        nameFilters: ["Python Files (*.py)"]
        fileMode: FileDialog.OpenFile

        onAccepted: {
            pythonFilename = selectedFile
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0
        anchors.margins: 8

        TabBar {
            id: tabBar
            Layout.fillWidth: true

            TabButton {
                id: controlInput
                text: "Input"
                height: 24

                background: Rectangle {
                    radius: 3
                    color: controlInput.checked
                           ? inputTable.selectionColor
                           : controlInput.hovered
                                ? inputTable.hoverColor
                                : inputTable.bgLight

                    border.color: inputTable.borderColor
                }

                contentItem: Text {
                    text: controlInput.text
                    color: inputTable.textColor
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.bold: controlInput.checked
                }
            }

            TabButton {
                id: controlOutput
                text: "Output"
                height: 24

                background: Rectangle {
                    radius: 3
                    color: controlOutput.checked
                           ? inputTable.selectionColor
                           : controlOutput.hovered
                                ? inputTable.hoverColor
                                : inputTable.bgLight

                    border.color: inputTable.borderColor
                }

                contentItem: Text {
                    text: controlOutput.text
                    color: inputTable.textColor
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.bold: controlOutput.checked
                }
            }
        }

        StackLayout {

            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: tabBar.currentIndex

            /* ---------------- INPUT TAB ---------------- */

            ColumnLayout {
                spacing: 8
                Layout.fillWidth: true
                Layout.fillHeight: true

                Rectangle {
                    Layout.fillWidth: true
                    height: 1
                    color: inputTable.borderColor
                }

                RowLayout {
                    spacing: 10
                    Layout.fillWidth: true

                    Label {
                        text: "Python File:"
                        color: inputTable.textColor
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        height: 28
                        radius: 3
                        color: inputTable.bgLight
                        border.color: inputTable.borderColor

                        TextEdit {
                            id: pythonFilenameTextEdit
                            anchors.fill: parent
                            anchors.margins: 4
                            color: inputTable.textColor
                            selectByMouse: true
                        }
                    }

                    Button {
                        id: control
                        text: "Browse"
                        height: 28

                        background: Rectangle {
                            radius: 3
                            color: control.down ? Qt.darker(inputTable.bgLight, 1.2)
                                                : control.hovered ? inputTable.hoverColor
                                                : inputTable.bgLight
                            border.color: inputTable.borderColor
                        }

                        contentItem: Text {
                            text: control.text
                            color: inputTable.textColor
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            font.pixelSize: 13
                        }
                        onClicked: fileDialog.open()
                    }
                }

                GridLayout {
                    columns: 4
                    columnSpacing: 10
                    rowSpacing: 6
                    Layout.fillWidth: true

                    Label {
                        text: "Row Count:"
                        color: inputTable.textColor
                    }

                    Rectangle {
                        Layout.preferredWidth: 120
                        height: 28
                        radius: 3
                        color: inputTable.bgLight
                        border.color: inputTable.borderColor

                        TextEdit {
                            anchors.fill: parent
                            anchors.margins: 4
                            color: inputTable.textColor
                            text: inputModel.rows
                            selectByMouse: true

                            onTextChanged: {
                                let val = Number(text)
                                if(inputModel.rows !== val)
                                    inputModel.rows = val
                            }
                        }
                    }

                    Label {
                        text: "Column Count:"
                        color: inputTable.textColor
                    }

                    Rectangle {
                        Layout.preferredWidth: 120
                        height: 28
                        radius: 3
                        color: inputTable.bgLight
                        border.color: inputTable.borderColor

                        TextEdit {
                            anchors.fill: parent
                            anchors.margins: 4
                            color: inputTable.textColor
                            text: inputModel.columns

                            onTextChanged: {
                                let val = Number(text)
                                if(inputModel.columns !== val)
                                    inputModel.columns = val
                            }
                        }
                    }
                }

                TableGridWidget {
                    id: inputTable
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
            }

            /* ---------------- OUTPUT TAB ---------------- */

            ColumnLayout {

                Layout.fillWidth: true
                Layout.fillHeight: true

                TableGridWidget {
                    id: outputTable
                    Layout.topMargin: 32
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
            }
        }

        /* ---------------- BUTTONS ---------------- */

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: "#454545"
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.topMargin: 8
            spacing: 8

            Item {
                Layout.fillWidth: true
            }

            Button {
                id: controlCancel
                text: "Cancel"
                height: 28
                Layout.preferredWidth: 90
                background: Rectangle {
                    radius: 3
                    color: controlCancel.down ? Qt.darker(inputTable.bgLight, 1.2)
                                        : controlCancel.hovered ? inputTable.hoverColor
                                        : inputTable.bgLight
                    border.color: inputTable.borderColor
                }

                contentItem: Text {
                    text: controlCancel.text
                    color: inputTable.textColor
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: 13
                }
                onClicked: detachedDialog.close()
            }

            Button {
                id: okButton
                text: "OK"
                height: 28
                Layout.preferredWidth: 90
                highlighted: true
                background: Rectangle {
                    radius: 3
                    color: okButton.down ? Qt.darker(inputTable.bgLight, 1.2)
                                        : okButton.hovered ? inputTable.hoverColor
                                        : inputTable.bgLight
                    border.color: inputTable.borderColor
                }

                contentItem: Text {
                    text: okButton.text
                    color: inputTable.textColor
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: 13
                }
                onClicked: detachedDialog.close()
            }
        }
    }
}
