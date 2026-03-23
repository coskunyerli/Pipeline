import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import QtQuick.Dialogs
import Pipeline.Actors as PA
import Pipeline.Services as PS
import Pipeline.Contexts as PC
import Pipeline.Models as PM

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
        //filename : pythonFilenameTextEdit.text
        inputData : inputTable.model
        outputData: outputTable.model
        inputParameterModel: paramListDialogModel
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
                id: controlParameter
                text: "Parameters"
            }

            PTabButton {
                id: controlInput
                text: "Input"
            }

            PTabButton {
                id: controlOutput
                text: "Output"
            }
        }

        SplitView
        {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.topMargin: 12
            orientation: Qt.Vertical



            StackLayout {

                SplitView.fillHeight: true
                SplitView.fillWidth: true
                currentIndex: tabBar.currentIndex

                /* ---------------- INPUT TAB ---------------- */
                ColumnLayout
                {

                    spacing: 8
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    WidgetTitleHeader
                    {
                        Layout.fillWidth: true
                        text:"Parameters"


                        PButton
                        {
                            text: "+"
                            Layout.preferredWidth: 22
                            Layout.preferredHeight: 22
                            ToolTip
                            {
                                text: "Add Parameter"
                            }
                        }


                        PTextEdit
                        {
                            id:columnCountEdit
                            text: "1"
                            Layout.preferredWidth: 22
                            Layout.preferredHeight: 22
                            ToolTip
                            {
                                text: "Add Parameter"
                            }
                        }

                    }
                    ParameterGrid
                    {
                        model: PM.NodeParamListDialogModel
                        {
                            id:paramListDialogModel
                            referenceModel: context.inputParameterModel
                        }

                        columnValue: Math.max(1,Number(columnCountEdit.text))
                    }
                }

                ColumnLayout {
                    spacing: 0
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    WidgetTitleHeader
                    {
                        Layout.fillWidth: true
                        text:"Input Table"
                    }

                    HierarchicalTableWidget
                    {
                        id:inputTable
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        referenceModel: context.inputModel
                    }
                }

                /* ---------------- OUTPUT TAB ---------------- */

                ColumnLayout {

                    spacing: 0
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    WidgetTitleHeader
                    {
                        Layout.fillWidth: true
                        text:"Output Table"
                    }

                    HierarchicalTableWidget
                    {
                        id:outputTable
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        referenceModel: context.outputModel
                    }
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
                    inputTable.model.resetData();
                    outputTable.model.resetData();
                    context.pythonError = ""
                }
            }

            PButton {
                text: "Cancel"
                Layout.preferredWidth: 90
                onClicked:
                {
                    inputTable.model.resetData();
                    outputTable.model.resetData();
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
                    resultContext.pythonError = context.pythonError;
                    resultContext.inputModel = inputTable.model;
                    resultContext.outputModel = outputTable.model;
                    resultContext.inputParameterModel = paramListDialogModel
                    accepted(resultContext)
                    detachedDialog.close()
                }
            }
        }
    }
}
