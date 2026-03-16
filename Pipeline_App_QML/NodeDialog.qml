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

                SplitView
                {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.topMargin: 12
                    orientation: Qt.Vertical
                    ColumnLayout
                    {
                        SplitView.fillWidth: true
                        SplitView.fillHeight: true
                        spacing: 0

                        Rectangle
                        {
                            color: "#404040"
                            height: 1
                            Layout.fillWidth: true
                        }

                        RowLayout
                        {
                            Layout.fillWidth: true
                            spacing: 6
                            PBreadCrumb
                            {
                                Layout.preferredHeight: 32
                                id : breadcrumb
                                color: "transparent"
                                Layout.fillWidth: true
                                index: inputDialogProxyModel.currentIndex
                                onClicked: (modelIndex) =>
                                           {
                                               inputDialogProxyModel.currentIndex = inputDialogModel.index(modelIndex.row,modelIndex.column,modelIndex.parent)
                                           }
                            }

                            Rectangle
                            {
                                width: 2
                                color:"#303030"
                                Layout.preferredHeight:breadcrumb.implicitHeight - 8
                            }

                            RowLayout {
                                spacing: 6
                                Layout.rightMargin: 8
                                Label {
                                    text: "Value"
                                    color: inputTable.textColor
                                }

                                PTextEdit {
                                    Layout.preferredHeight: 24
                                    Layout.preferredWidth: 60
                                    bottomPadding: 4
                                    topPadding: 4
                                    text: inputDialogProxyModel.currentIndexValue
                                    onTextChanged: {
                                        if(inputDialogProxyModel.currentIndexValue !== text)
                                        {
                                            inputDialogProxyModel.currentIndexValue = text
                                        }
                                    }
                                }

                                Label {
                                    text: "Row Count:"
                                    color: inputTable.textColor
                                }



                                PTextEdit {
                                    Layout.preferredHeight: 24
                                    Layout.preferredWidth: 60
                                    bottomPadding: 4
                                    topPadding: 4
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
                                    Layout.preferredHeight: 24
                                    Layout.preferredWidth: 60
                                    bottomPadding: 4
                                    topPadding: 4
                                    text: inputDialogModel.columns
                                    onTextChanged: {
                                        let val = Number(text)
                                        if(inputDialogModel.columns !== val)
                                            inputDialogModel.columns = val
                                    }
                                }
                            }
                        }


                        TableGridWidget {
                            id: inputTable
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            // copy inputModel inside
                            model:PM.NodeTableSliceProxyModel
                            {
                                id: inputDialogProxyModel
                                sourceModel:PM.NodeTableDialogModel
                                {
                                    id: inputDialogModel
                                    referenceModel: inputModel
                                }
                            }

                            onCellDClicked:(row,column) =>
                            {
                                let modelIndex = inputDialogModel.index(row,column, inputDialogProxyModel.currentIndex);
                                if(!modelIndex.data(Qt.UserRole + 1))
                                {
                                    modelIndex = inputDialogModel.createCell(modelIndex); 
                                }
                                inputDialogProxyModel.currentIndex = modelIndex
                            }
                        }

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

                RowLayout
                {
                    Layout.fillWidth: true
                    spacing: 6
                    PBreadCrumb
                    {
                        id : outputBreadcrumb
                        color: "transparent"
                        Layout.fillWidth: true
                    }

                    Rectangle
                    {
                        width: 2
                        color:"#303030"
                        Layout.preferredHeight:outputBreadcrumb.implicitHeight - 8
                    }

                    RowLayout {
                        spacing: 6
                        Layout.rightMargin: 8
                        Label {
                            text: "Row Count:"
                            color: outputTable.textColor
                        }



                        PTextEdit {
                            Layout.preferredHeight: 24
                            Layout.preferredWidth: 60
                            bottomPadding: 4
                            topPadding: 4
                            text: outputDialogModel.rows
                            onTextChanged: {
                                let val = Number(text)
                                if(outputDialogModel.rows !== val)
                                    outputDialogModel.rows = val
                            }
                        }


                        Label {
                            text: "Column Count:"
                            color: inputTable.textColor
                        }

                        PTextEdit {
                            Layout.preferredHeight: 24
                            Layout.preferredWidth: 60
                            bottomPadding: 4
                            topPadding: 4
                            text: outputDialogModel.columns
                            onTextChanged: {
                                let val = Number(text)
                                if(outputDialogModel.columns !== val)
                                    outputDialogModel.columns = val
                            }
                        }
                    }
                }

                TableGridWidget {
                    id: outputTable
                    // copy outputModel inside
                    model:PM.NodeTableSliceProxyModel
                    {
                        id: outputDialogProxyModel
                        sourceModel:PM.NodeTableDialogModel
                        {
                            id: outputDialogModel
                            referenceModel:outputModel
                        }
                    }

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
