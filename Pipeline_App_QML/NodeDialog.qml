import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import QtQuick.Dialogs
import Pipeline.Models as PM
import Pipeline.Actors as PA
import Pipeline.Services as PS

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
    signal accepted(string name, string filename, string error)
    signal saveRequested()

    property var inputModel
    property var outputModel
    property string pythonFilename
    property string name
    property alias pythonError : outputConsole.text
    property var actor : PA.PythonNodeDialogActor
    {
        id:actor
        filename : pythonFilenameTextEdit.text
        inputData : inputDialogModel
        outputData: outputDialogModel
        onPythonErrorChanged:
        {
            detachedDialog.pythonError = actor.pythonError
        }
    }

    onClosing: dialogClosed()

    FileDialog {
        id: fileDialog
        title: "Select File"
        nameFilters: ["Python Files (*.py)"]
        fileMode: FileDialog.OpenFile

        onAccepted: {
            pythonFilenameTextEdit.text = selectedFile
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
                GridLayout {
                    columns: 3
                    columnSpacing: 10
                    Layout.fillWidth: true

                    Label {
                        text: "Name:"
                        color: inputTable.textColor
                    }

                    PTextEdit {
                        id: nameTextEdit
                        text:name
                        Layout.fillWidth: true
                        Layout.columnSpan: 2
                    }

                    Label {
                        text: "Python File:"
                        color: inputTable.textColor
                    }

                    PTextEdit {
                        id: pythonFilenameTextEdit
                        text:pythonFilename
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
                        text: inputDialogModel.rows
                        onTextChanged: {
                            let val = Number(text)
                            if(inputDialogModel.rows !== val)
                                inputDialogModel.rows = val
                        }
                    }


                    Label {
                        text: "Column Count:"
                        color: inputTable.textColor
                    }

                    PTextEdit {
                        Layout.preferredWidth: 120
                        text: inputDialogModel.columns
                        onTextChanged: {
                            let val = Number(text)
                            if(inputDialogModel.columns !== val)
                                inputDialogModel.columns = val
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
                        // copy inputModel inside
                        model:PM.NodeTableDialogModel
                        {
                            id: inputDialogModel
                            referenceModel: inputModel
                        }

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
                    // copy outputModel inside
                    model:PM.NodeTableDialogModel
                    {
                        id: outputDialogModel
                        referenceModel:outputModel
                    }

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
                    actor.runStandalone()
                }
            }


            Item {
                Layout.fillWidth: true
            }

            PButton {
                text: "Save"
                Layout.preferredWidth: 90
                onClicked:
                {
                    saveRequested()
                }
            }

            PButton {
                text: "Reset"
                Layout.preferredWidth: 90
                onClicked:
                {
                    inputDialogModel.resetData();
                    outputDialogModel.resetData();
                    detachedDialog.pythonError = ""
                }
            }

            PButton {
                text: "Cancel"
                Layout.preferredWidth: 90
                onClicked:
                {
                    inputDialogModel.resetData();
                    outputDialogModel.resetData();
                    detachedDialog.close()
                }
            }

            PButton {
                id: okButton
                Layout.preferredWidth: 90
                text: "OK"
                onClicked:
                {
                    // copy data into reference model
                    inputDialogModel.saveData();
                    outputDialogModel.saveData();
                    accepted(nameTextEdit.text, actor.filename, actor.pythonError)
                    detachedDialog.close()
                }
            }
        }
    }
}
