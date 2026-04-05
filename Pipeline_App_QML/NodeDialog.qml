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


    // Popup {
    //     id: popup
    //     modal: false
    //     focus: true
    //     dim: true
    //     x: label.mapToItem(null, 0, 0).x
    //     y: label.mapToItem(null, 0, 0).y

    //     Overlay.modeless: Rectangle {
    //         color: "#80000000"   // rgba: alpha=0.5
    //     }

    //     width: root.maxWidth + 8
    //     height: 28

    //     background: Rectangle {
    //         radius: 3
    //         color: "#505050"
    //         border.color: "#656565"
    //     }

    //     contentItem: PTextEdit {
    //         id: edit
    //         anchors.fill: parent
    //         selectByMouse: true

    //         Keys.onReturnPressed: finish()
    //         Keys.onEnterPressed: finish()
    //         Keys.onEscapePressed: popup.close()

    //         function finish() {
    //             modelData.value = text
    //             popup.close()
    //         }

    //         onFocusChanged: {
    //             if (!focus) popup.close()
    //         }
    //     }
    // }

    signal dialogClosed()
    signal accepted(var context)
    signal saveRequested()
    property var context;
    readonly property var localContext : resultContext
    readonly property var localActor : actor

    QtObject
    {
        id:privateObject
        function createIndexList(model)
        {
            let indexArr = []
            for(let i = 0; i< model.rowCount();i++)
            {
                indexArr.push(model.index(i,0));
            }
            return indexArr;
        }
    }

    property var actor : PA.PythonNodeDialogActor
    {
        id:actor
        inputData : dialogInputModel
        outputData: dialogOutputModel
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
        inputModel : dialogInputModel || null
        outputModel : dialogOutputModel || null
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
                text: "Inputs"
            }

            PTabButton {
                id: controlOutput
                text: "Outputs"
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

                    }
                    ParameterGrid
                    {
                        model: PM.NodeParamListDialogModel
                        {
                            id:paramListDialogModel
                            referenceModel: context.inputParameterModel
                        }

                        columnValue: 1
                    }
                }

                SplitView
                {
                    spacing: 0
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    ColumnLayout
                    {

                        spacing: 0
                        SplitView.preferredWidth: 200
                        SplitView.fillHeight: true

                        WidgetTitleHeader
                        {
                            Layout.fillWidth: true
                            text:"Input Ports"
                        }

                        Component {
                            id: portDelegate

                            Item {
                                id: root
                                width: listView.width
                                height: 34

                                property bool selected: listView.currentIndex === indexData

                                Rectangle {
                                    id: background
                                    anchors.fill: parent

                                    color: root.selected
                                           ? "#3a82f7"        // selected
                                           : mouseArea.containsMouse
                                             ? "#2a2a2a"      // hover
                                             : "#1e1e1e"      // normal

                                    border.color: root.selected ? "#5aa0ff" : "#2c2c2c"
                                    border.width: 1
                                }

                                Text {
                                    anchors.fill: parent
                                    padding: 6
                                    text: (modelData && modelData.cellName) || ""
                                    color: root.selected ? "white" : "#cccccc"
                                    verticalAlignment: Text.AlignVCenter
                                }

                                MouseArea {
                                    id: mouseArea
                                    anchors.fill: parent
                                    hoverEnabled: true

                                    onClicked: {
                                        listView.currentIndex = indexData
                                    }
                                    onDoubleClicked:
                                    {
                                        popup.open()
                                        edit.text = modelData.cellName
                                        edit.selectAll()
                                        edit.forceActiveFocus()
                                    }
                                }
                            }
                        }

                        ListView
                        {
                            id:portListView
                            property var currentPortIndex:null
                            property var indexList : privateObject.createIndexList(dialogInputModel)
                            onCurrentIndexChanged:
                            {
                                portListView.currentPortIndex = indexList[portListView.currentIndex]
                            }

                            model: context.inputModel
                            delegate: Loader
                            {
                                sourceComponent:portDelegate
                                property var modelData : model
                                property var indexData : index
                                property var listView : portListView
                            }

                            Layout.fillWidth: true
                            Layout.fillHeight: true
                        }

                        PButton
                        {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 36
                            text: "Add Port"
                            onClicked:
                            {

                            }
                        }
                    }

                    ColumnLayout
                    {
                        spacing: 0
                        SplitView.fillWidth: true
                        SplitView.fillHeight: true

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
                            proxyModel:PM.NodeTableSliceProxyModel
                            {
                                id: inputSliceProxyModel
                                currentIndex:portListView.currentPortIndex
                                sourceModel:PM.NodeTableDialogModel
                                {
                                    id: dialogInputModel
                                    referenceModel: context.inputModel
                                }
                            }
                            onBreadcrumbClicked: (modelIndex) =>
                            {
                                let i = dialogInputModel.index(modelIndex.row,modelIndex.column,modelIndex.parent);
                                portListView.indexList[inputTable.rootIndex.row] = i;
                                portListView.currentPortIndex = i
                            }
                            onCellDClicked: (row, column) =>
                            {
                                let modelIndex = dialogInputModel.index(row,column, inputSliceProxyModel.currentIndex);
                                if(!modelIndex.data(Qt.UserRole + 1))
                                {
                                    modelIndex = dialogInputModel.createCell(modelIndex);
                                }
                                portListView.indexList[inputTable.rootIndex.row] = modelIndex;
                                portListView.currentPortIndex = modelIndex
                            }
                        }
                    }
                }

                /* ---------------- OUTPUT TAB ---------------- */

                SplitView
                {
                    spacing: 0
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    ColumnLayout
                    {

                        spacing: 0
                        SplitView.preferredWidth: 200
                        SplitView.fillHeight: true

                        WidgetTitleHeader
                        {
                            Layout.fillWidth: true
                            text:"Output Ports"
                        }

                        ListView
                        {
                            id:outPortListView
                            property var currentPortIndex:null
                            property var indexList : privateObject.createIndexList(dialogOutputModel)
                            onCurrentIndexChanged:
                            {
                                outPortListView.currentPortIndex = indexList[outPortListView.currentIndex]
                            }

                            model: context.outputModel
                            delegate: Loader
                            {
                                sourceComponent:portDelegate
                                property var modelData : model
                                property var indexData : index
                                property var listView : outPortListView
                            }

                            Layout.fillWidth: true
                            Layout.fillHeight: true
                        }

                        PButton
                        {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 36
                            text: "Add Port"
                            onClicked:
                            {

                            }
                        }
                    }

                    ColumnLayout
                    {

                        spacing: 0
                        SplitView.fillWidth: true
                        SplitView.fillHeight: true

                        WidgetTitleHeader
                        {
                            Layout.fillWidth: true
                            text:"Output Table"
                        }

                        HierarchicalTableWidget
                        {
                            id: outputTable
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            proxyModel:PM.NodeTableSliceProxyModel
                            {
                                id: outputSliceProxyModel
                                currentIndex:outPortListView.currentPortIndex
                                sourceModel:PM.NodeTableDialogModel
                                {
                                    id: dialogOutputModel
                                    referenceModel: context.outputModel
                                }
                            }
                            onBreadcrumbClicked: (modelIndex) =>
                            {
                                let i = dialogOutputModel.index(modelIndex.row,modelIndex.column,modelIndex.parent);
                                outPortListView.indexList[outputTable.rootIndex.row] = i;
                                outPortListView.currentPortIndex = i
                            }
                            onCellDClicked: (row, column) =>
                            {
                                let modelIndex = dialogOutputModel.index(row,column, outputSliceProxyModel.currentIndex);
                                if(!modelIndex.data(Qt.UserRole + 1))
                                {
                                    modelIndex = dialogOutputModel.createCell(modelIndex);
                                }
                                outPortListView.indexList[outputTable.rootIndex.row] = modelIndex;
                                outPortListView.currentPortIndex = modelIndex
                            }
                        }
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
                    dialogInputModel.resetData();
                    dialogOutputModel.resetData();
                    context.pythonError = ""
                }
            }

            PButton {
                text: "Cancel"
                Layout.preferredWidth: 90
                onClicked:
                {
                    dialogInputModel.resetData();
                    dialogOutputModel.resetData();
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
