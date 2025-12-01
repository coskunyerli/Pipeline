import QtQuick 2.12
import QtQml 2.15 as QQML
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12
import Pipeline.Models 1.0 as PM
import QtQml.Models 2.12 as QM
import QtQuick.Controls  2.15
import Pipeline.Services 1.0 as PS
import Pipeline.Managers 1.0 as PMan

Window {
    width: 1280
    height: 800
    visible: true
    title: qsTr("Hello World")

    Loader {
        id: dialogLoader
        active: false
        onLoaded: {
                    // Yüklenen pencere kapandığında Loader'ı sıfırla
                    item.dialogClosed.connect(() => {
                        dialogLoader.active = false
                    })
                }
    }

    PS.GraphModelService
    {
        id: graphModelService
        model:nodeGraphTreeModel
    }

    function addNode(pos)
    {
        graphModelService.addNode(pos)
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
        // onSelectedIndexesChanged:
        // {
        //     console.log(selectedIndexes,currentIndex)
        // }
    }



    ColumnLayout
    {
        spacing: 0
        anchors.fill: parent
        Button
        {
            text:"Run"
            onClicked:
            {
                graphModelService.start()
            }
        }
        RowLayout
        {
            spacing: 0
            Rectangle
            {
                Layout.preferredWidth:200
                Layout.fillHeight: true
                color:"#202020"
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
                                    //propertyEditor.node = model
                                }
                                onDoubleClicked:
                                {
                                    if (!dialogLoader.active)
                                    {
                                        dialogLoader.source = "NodeDialog.qml"
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

                    Menu {
                        id: contextMenu
                        MenuItem
                        {
                            text: "Add Node"
                            onTriggered:
                            {
                                addNode(graphicsView.mapToScene(mouseArea.pos.x,mouseArea.pos.y));
                            }

                        }
                        MenuItem
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
