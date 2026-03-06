import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import QtQuick.Dialogs

Window {
    id: detachedDialog
    width: 1100
    height: 750
    title: "Python Node Dialog"
    flags: Qt.Dialog | Qt.WindowCloseButtonHint | Qt.WindowTitleHint
    modality: Qt.NonModal
    visible: true
    color: "#252525"

    signal dialogClosed()
    property var actionObject
    property var inputModel
    property var outputModel
    property alias pythonFilename : pythonFilenameTextEdit.text
    property alias pythonError : outputConsole.text

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

            PTabButton {
                id: controlInput
                text: "Input"
            }

            PTabButton {
                id: controlOutput
                text: "Output"
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

                    PTextEdit {
                        id: pythonFilenameTextEdit
                        Layout.fillWidth: true
                    }

                    PButton {
                        id: control
                        Layout.preferredWidth: 90
                        text: "Browse"
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



                    PTextEdit {
                        Layout.preferredWidth: 120
                        text: inputModel.rows
                        onTextChanged: {
                            let val = Number(text)
                            if(inputModel.rows !== val)
                                inputModel.rows = val
                        }
                    }


                    Label {
                        text: "Column Count:"
                        color: inputTable.textColor
                    }

                    PTextEdit {
                        Layout.preferredWidth: 120
                        text: inputModel.columns
                        onTextChanged: {
                            let val = Number(text)
                            if(inputModel.columns !== val)
                                inputModel.columns = val
                        }
                    }
                }

                SplitView
                {
                    orientation: Qt.Vertical
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    TableGridWidget {
                        id: inputTable
                        model:inputModel
                        SplitView.fillWidth: true
                        SplitView.fillHeight: true
                    }

                    ColumnLayout
                    {
                        SplitView.preferredHeight: 250
                        SplitView.fillWidth: true
                        spacing: 0

                        Rectangle
                        {
                            color: "#404040"

                            Layout.preferredHeight: 28
                            Layout.fillWidth: true
                            Label
                            {
                                anchors.fill: parent
                                anchors.leftMargin: 8
                                verticalAlignment: Text.AlignVCenter
                                text:"Console Output"
                            }
                        }

                        Rectangle
                        {
                            height: 1
                            Layout.fillWidth: true
                            color: "#656565"
                        }


                        POutputConsole
                        {
                            id: outputConsole
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                        }
                    }
                }
            }

            /* ---------------- OUTPUT TAB ---------------- */

            ColumnLayout {

                Layout.fillWidth: true
                Layout.fillHeight: true

                TableGridWidget {
                    id: outputTable
                    model:outputModel
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
            color: "#656565"
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.topMargin: 8
            spacing: 8

            PButton {
                text: "Run"
                Layout.preferredWidth: 90
                onClicked: (mouse) =>
                {
                    actionObject.runStandalone()
                }
            }


            Item {
                Layout.fillWidth: true
            }

            PButton {
                text: "Cancel"
                Layout.preferredWidth: 90
                onClicked: detachedDialog.close()
            }

            PButton {
                id: okButton
                Layout.preferredWidth: 90
                text: "OK"
                onClicked: detachedDialog.close()
            }
        }
    }
}
