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
    readonly property var localContext : resultContext
    readonly property var localActor : actor
    property var actor : PA.PythonNodeDialogActor
    {
        id:actor
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
        pythonError : context.pythonError
        inputModel : inputTable.model
        outputModel : outputTable.model
        inputParameterModel : paramListDialogModel
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


                        Button
                        {
                            visible: context.inputParameterModel.editable
                            id:addParameterButton
                            icon.source: "qrc:/icons/add_24.svg"
                            Layout.preferredWidth: 22
                            Layout.preferredHeight: 22
                            ToolTip
                            {
                                text: "Add Parameter"
                            }

                            onClicked:
                            {
                                contextMenu.open()
                            }

                            Menu {
                                    id: contextMenu

                                    x: addParameterButton.x
                                    y: addParameterButton.y + addParameterButton.height + 4
                                    implicitWidth: 160
                                    background: Rectangle {
                                        color: "#505050"
                                        border.color: "#656565"
                                    }

                                    delegate: MenuItem {
                                        id: item

                                        contentItem: Label {
                                            text: item.text
                                            color: "#e0e0e0"
                                            anchors.verticalCenter: parent.verticalCenter
                                            leftPadding: 10
                                        }

                                        background: Rectangle {
                                            color: item.highlighted ? "#606060" : "transparent"
                                        }
                                    }

                                    MenuItem {
                                        text: "Add String"
                                        onTriggered: paramListDialogModel.addParameter("String",3,"")
                                    }

                                    MenuItem {
                                        text: "Add Bool"
                                        onTriggered: paramListDialogModel.addParameter("Bool",2, false)
                                    }

                                    MenuItem {
                                        text: "Add Browse"
                                        onTriggered: paramListDialogModel.addParameter("Browse",6,"")
                                    }
                                }
                        }


                        // PTextEdit
                        // {
                        //     id:columnCountEdit
                        //     text: "1"
                        //     Layout.preferredWidth: 22
                        //     Layout.preferredHeight: 22
                        //     ToolTip
                        //     {
                        //         text: "Add Parameter"
                        //     }
                        // }

                    }
                    ParameterGrid
                    {
                        model: PM.NodeParamListDialogModel
                        {
                            id:paramListDialogModel
                            referenceModel: context.inputParameterModel
                        }

                        columnValue: 1 //Math.max(1,Number(columnCountEdit.text))
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

                    accepted(resultContext)
                    detachedDialog.close()
                }
            }
        }
    }
}
