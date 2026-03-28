import QtQuick 2.12
import QtQml 2.15 as QQML
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12
import Pipeline.Models 1.0 as PM
import QtQml.Models 2.12 as QM
import QtQuick.Controls  2.12 as QC
import Pipeline.Services 1.0 as PS
import Pipeline.Managers 1.0 as PMan

Window {
    width: 1280
    height: 800
    visible: true
    title: qsTr("Pipeline App")

    Loader {
        id: dialogLoader
        property var dialogNodeModel
        active: false
        sourceComponent: NodeDialog
        {
            id:nodeDialog
            context:dialogLoader.dialogNodeModel.actorAction.createDataContext(nodeDialog)
            onAccepted : (resultContext) =>
            {
                dialogLoader.dialogNodeModel.actorAction.saveContext(resultContext);
            }
            onSaveRequested :
            {
                let metadata = nodeDialog.localActor.actorAction.createMetadata()
                nodeModelService.saveNode("This is test description", metadata)
            }
            actor: dialogLoader.dialogNodeModel.actor
        }

        onLoaded: {
                    item.dialogClosed.connect(() => {
                        dialogLoader.active = false
                    })
                }
    }

    PS.GraphModelService
    {
        id: graphModelService
        model:nodeGraphTreeModel
        quickNodeModel: nodeModelService.model
    }

    PS.NodeModelService
    {
        id: nodeModelService
    }

    function addNode(name, pos)
    {
        graphModelService.addNode(name, pos)
    }

    PMan.NodeUIManager
    {
        id : nodeUIManager
        nodeSize: Qt.size(200,50)
        portSize: Qt.size(14,14)
        portSpacing: 6
    }

    PM.NodeGraphTreeModel
    {
        id: nodeGraphTreeModel
    }


    PM.NodeGraphModel
    {
        id: nodeGraphModel
        sourceModel: nodeGraphTreeModel
    }

    PM.NodeGraphViewModel
    {
        id: nodeGraphViewModel
        sourceModel: nodeGraphModel
        uiManager: nodeUIManager
    }

    PM.ConnectionGraphModel
    {
        id: connectionModel
        sourceModel: nodeGraphTreeModel
    }

    PM.ConnectionGraphViewModel
    {
        id: connectionViewModel
        sourceModel: connectionModel
        uiManager: nodeUIManager
    }

    QM.ItemSelectionModel
    {
        id:nodeSelectionModel
        model:nodeGraphViewModel
    }



    ColumnLayout
    {
        spacing: 0
        anchors.fill: parent
        Rectangle
        {
            Layout.fillWidth: true
            height: 1
            color:"#404040"
        }
        Rectangle
        {
            color:"#202020"
            Layout.preferredHeight: 32
            Layout.fillWidth: true
            RowLayout
            {
                anchors.fill: parent
                Item {
                    Layout.fillWidth: true
                }
                QC.Button
                {
                    icon.source: "qrc:/icons/play_arrow.svg"
                    onClicked:
                    {
                        graphModelService.start()
                    }
                }
                Item {
                    Layout.fillWidth: true
                }
            }
        }
        Rectangle
        {
            Layout.fillWidth: true
            height: 1
            color:"#404040"
        }

        RowLayout
        {
            spacing: 0
            QuickNodeListView
            {
                Layout.preferredWidth:240
                Layout.fillHeight: true
                model: nodeModelService.model
            }

            Item
            {
                Layout.fillHeight: true
                Layout.fillWidth: true
                GraphicsView
                    {
                        anchors.fill: parent
                        id:graphicsView
                        interactive:false
                        width: parent.width
                        height: parent.height
                        color: "#303030"
                        scene.color: "transparent"
                        Repeater
                        {
                            model: nodeGraphViewModel
                            id : moduleRepeater

                            Node
                            {
                                z:1
                                isSelected: nodeSelectionModel.currentIndex === nodeGraphViewModel.index(index,0)
                                nodeModel: model
                                id:node
                                onPortDropped: (portInIndex, portOutIndex) =>
                                {
                                    nodeGraphTreeModel.addConnection(portInIndex, portOutIndex)
                                }
                                onPressed:
                                {
                                    nodeSelectionModel.setCurrentIndex(nodeGraphViewModel.index(index,0), QM.ItemSelectionModel.SelectCurrent)
                                }
                                onDoubleClicked:
                                {
                                    if (!dialogLoader.active)
                                    {
                                        dialogLoader.dialogNodeModel = nodeModel
                                        dialogLoader.active = true
                                    }
                                }
                            }
                        }

                        Repeater
                        {
                            model:connectionViewModel

                            NodeConnection
                            {
                                color:"#ffcc00"

                                borderColor: "#cca300"
                                start:Qt.point(model.connectionInX, model.connectionInY)
                                end: Qt.point(model.connectionOutX,model.connectionOutY)
                                MouseArea
                                {
                                    id:connectionMouseArea
                                    anchors.fill: parent
                                    hoverEnabled: true
                                }
                            }
                        }


                    }

                DropArea {
                        anchors.fill: parent
                        keys: ["application/node"]

                        onDropped: (drop) => {

                            let str = drop.getDataAsString("application/node")
                            let data = JSON.parse(str)

                            let x = drop.x - data.hotX
                            let y = drop.y - data.hotY
                            addNode(data.name, Qt.point(x,y));
                        }
                    }

                MouseArea {
                    id: mouseArea
                    property point pos
                    anchors.fill: parent
                    acceptedButtons: Qt.RightButton
                    onClicked: (mouse) => {
                        if (mouse.button === Qt.RightButton)
                            {
                               contextMenu.popup();
                               pos = Qt.point(mouse.x,mouse.y);
                            }
                    }
                    onPressAndHold: (mouse) => {
                        if (mouse.source === Qt.MouseEventNotSynthesized)
                            contextMenu.popup()
                    }

                    QC.Menu {
                        id: contextMenu
                        QC.MenuItem
                        {
                            text: "Add Node"
                            onTriggered:
                            {
                                //addNode(graphicsView.mapToScene(mouseArea.pos.x,mouseArea.pos.y));
                            }

                        }
                        QC.MenuItem
                        {
                            text: "Add Node2"
                            onTriggered:
                            {
                                nodeUIManager.nodeSize = Qt.size(300,50)
                            }

                        }
                    }
                }
            }
        }


    }
}
