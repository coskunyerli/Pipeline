import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import QtQuick.Dialogs
import Pipeline.Models as PM
import Pipeline.Actors as PA
import Pipeline.Services as PS
import Pipeline.Contexts as PC

Window {
    id: detachedDialog
    width: 1100
    height: 850
    title: "Python Node Dialog"
    flags: Qt.Dialog | Qt.WindowCloseButtonHint | Qt.WindowTitleHint
    modality: Qt.NonModal
    visible: true
    color: "#252525"

    signal dialogClosed()
    signal accepted(var context)
    signal saveRequested()
    property var context;
    property var actor : PA.PythonNodeDialogActor
    {
        id:actor
        filename : pythonFilenameTextEdit.text
        inputData : inputDialogModel
        inputParameterData:parameterDialogModel
        outputData: outputDialogModel
        onPythonErrorChanged:
        {
            context.pythonError = actor.pythonError
        }
    }

    PC.PythonProcessDataContext
    {
        id:resultContext
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
                    color: "#505050"
                }
                GridLayout {
                    columns: 3
                    columnSpacing: 10
                    Layout.fillWidth: true

                    Label {
                        text: "Name:"
                        color: "#e0e0e0"
                    }

                    PTextEdit {
                        id: nameTextEdit
                        text:context.name
                        Layout.fillWidth: true
                        Layout.columnSpan: 2
                    }

                    Label {
                        text: "Python File:"
                        color: "#e0e0e0"
                    }

                    PTextEdit {
                        id: pythonFilenameTextEdit
                        text:context.filename
                        Layout.fillWidth: true
                    }

                    PButton {
                        id: control
                        Layout.preferredWidth: 90
                        text: "Browse"
                        onClicked: fileDialog.open()
                    }

                }

                SplitView
                {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.topMargin: 12
                    orientation: Qt.Vertical

                    ColumnLayout
                    {
                        spacing: 0
                        SplitView.preferredHeight: 200
                        SplitView.fillWidth: true
                        WidgetTitleHeader
                        {
                            Layout.fillWidth: true
                            text:"Parameters"
                        }

                        HierarchicalTableWidget
                        {
                            id:parameterTable
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            model:PM.NodeTableSliceProxyModel
                            {
                                id: parameterDialogProxyModel
                                sourceModel:PM.NodeTableDialogModel
                                {
                                    id: parameterDialogModel
                                    referenceModel: context.inputParameterModel
                                }
                            }

                            // onCellDClicked:(row,column) =>
                            // {
                            //     let modelIndex = inputDialogModel.index(row,column, inputDialogProxyModel.currentIndex);
                            //     if(!modelIndex.data(Qt.UserRole + 1))
                            //     {
                            //         modelIndex = inputDialogModel.createCell(modelIndex);
                            //     }
                            //     inputDialogProxyModel.currentIndex = modelIndex
                            // }
                        }
                    }

                    ColumnLayout
                    {
                        SplitView.fillWidth: true
                        SplitView.fillHeight: true
                        spacing: 0

                        WidgetTitleHeader
                        {
                            Layout.fillWidth: true
                            text:"Input Table"
                        }
                        HierarchicalTableWidget
                        {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            model:PM.NodeTableSliceProxyModel
                            {
                                id: inputDialogProxyModel
                                sourceModel:PM.NodeTableDialogModel
                                {
                                    id: inputDialogModel
                                    referenceModel: context.inputModel
                                }
                            }

                            // onCellDClicked:(row,column) =>
                            // {
                            //     let modelIndex = inputDialogModel.index(row,column, inputDialogProxyModel.currentIndex);
                            //     if(!modelIndex.data(Qt.UserRole + 1))
                            //     {
                            //         modelIndex = inputDialogModel.createCell(modelIndex);
                            //     }
                            //     inputDialogProxyModel.currentIndex = modelIndex
                            // }
                        }

                    }

                    ColumnLayout
                    {
                        SplitView.preferredHeight: 120
                        SplitView.fillWidth: true
                        spacing: 0

                        WidgetTitleHeader
                        {
                            Layout.fillWidth: true
                            text:"Console Output"
                        }


                        POutputConsole
                        {
                            id: outputConsole
                            text: context.pythonError
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                        }
                    }
                }
            }

            /* ---------------- OUTPUT TAB ---------------- */

            ColumnLayout {

                spacing: 0
                Layout.fillWidth: true
                Layout.fillHeight: true

                Rectangle
                {
                    color: "#404040"
                    height: 1
                    Layout.fillWidth: true
                    Layout.topMargin: 32
                }

                HierarchicalTableWidget
                {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    model:PM.NodeTableSliceProxyModel
                    {
                        id: outputDialogProxyModel
                        sourceModel:PM.NodeTableDialogModel
                        {
                            id: outputDialogModel
                            referenceModel:context.outputModel
                        }
                    }

                    // onCellDClicked:(row,column) =>
                    // {
                    //     let modelIndex = inputDialogModel.index(row,column, inputDialogProxyModel.currentIndex);
                    //     if(!modelIndex.data(Qt.UserRole + 1))
                    //     {
                    //         modelIndex = inputDialogModel.createCell(modelIndex);
                    //     }
                    //     inputDialogProxyModel.currentIndex = modelIndex
                    // }
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
                    context.pythonError = ""
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
                    resultContext.filename = actor.filename;
                    resultContext.name = nameTextEdit.text;
                    resultContext.pythonError = context.pythonError;
                    resultContext.inputModel = inputDialogModel;
                    resultContext.outputModel = outputDialogModel;
                    resultContext.inputParameterModel = parameterDialogModel
                    accepted(resultContext)
                    detachedDialog.close()
                }
            }
        }
    }
}
