import QtQuick 2.12
import Pipeline.Models 1.0 as PM
Item
{
    property bool isSelected:false
    property var nodeModel
    readonly property var itemModel : nodeModel ? nodeModel.dataIndex.model : nullptr
    signal portDropped(var portInIndex, var portOutIndex);
    signal pressed(var event);
    signal released(var event);
    signal clicked(var event);
    signal doubleClicked(var event);
    id: node
    x:nodeModel.posX
    y:nodeModel.posY
    width: nodeModel.nodeWidth
    height: nodeModel.nodeHeight

    // Pozisyon takibi için property
    property real dragStartX: 0
    property real dragStartY: 0


    NodeConnection
    {
        property bool dragActive
        property var portIndex
        id: connectionPreview
        color:"#ffcc00"
        borderColor: "#cca300"
        parent:node.parent
        //visible: Drag.active
        //anchors.fill: parent
        Drag.active: dragActive
        Drag.dragType: Drag.Automatic
        Drag.mimeData: {
            "text/plain": "Copied text"
        }
    }

    Rectangle
    {
        id: background
        anchors.fill: parent
        radius: 6
        color: "#404040"
        border.color: isSelected ? "#808080" : "#505050"
        border.width: 1

        // Başlık (sürüklenebilir alan)
        Text
        {
            id: title
            text: "Node"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            anchors.fill: parent
            color: "#ddd"
            font.pixelSize: 16
            MouseArea
            {
                id: nodeMouseArea
                property point clickedPos
                anchors.fill: parent
                onPressed:(mouse) =>
                {
                    nodeMouseArea.clickedPos = nodeMouseArea.mapToGlobal(mouse.x,mouse.y)
                    node.pressed(mouse);
                }
                onPositionChanged:(mouse) =>
                {
                    let globalPos = nodeMouseArea.mapToGlobal(mouse.x,mouse.y)
                    let diff = Qt.point(globalPos.x - nodeMouseArea.clickedPos.x, globalPos.y - nodeMouseArea.clickedPos.y)
                    nodeModel.posX = nodeModel.posX + diff.x
                    nodeModel.posY = nodeModel.posY + diff.y
                    nodeMouseArea.clickedPos = globalPos
                }

                onReleased:(mouse) =>
                {
                    nodeMouseArea.clickedPos = Qt.point(-1,-1)
                    node.released(mouse)
                }

                onClicked: (mouse) =>
                {
                    node.clicked(mouse)
                }
                onDoubleClicked: (mouse) =>
                {
                    node.doubleClicked(mouse);
                }
            }
        }


        // Input port (left)

            Repeater
            {
                model : PM.PortGraphViewModel
                {
                    id: portInModel
                    nodeIndex:nodeModel.dataIndex
                    sourceModel : itemModel
                    isInPort : true
                    uiManager:itemModel.uiManager
                }

                Port
                {
                    id: inPort
                    x:model.portX
                    y:model.portY
                    width: model.portWidth
                    height:model.portHeight

                    borderColor: "#00bcd4"
                    color: model.hasConnection || portDropArea.containsDrag ? "#0097a7" : "#404040"
                    DropArea
                    {
                        id:portDropArea
                        anchors.fill: parent
                        onEntered:(drag) =>
                        {
                            let nodePos = inPort.mapToItem(node, 0, inPort.height / 2)
                            let endPos = node.mapToItem(node.parent, 0, nodePos.y)
                            drag.source.end = endPos;
                        }
                        onExited:
                        {
                            drag.source.end = drag.source.start;
                        }

                        onDropped:
                        {

                            let inPort = model.dataIndex
                            drag.source.end = drag.source.start;
                            node.portDropped(inPort, drag.source.portIndex)
                        }
                    }
                }
            }


            Repeater
            {
                model : PM.PortGraphViewModel
                {
                    id: portOutModel
                    nodeIndex:nodeModel.dataIndex
                    sourceModel : itemModel
                    isInPort : false
                    uiManager:itemModel.uiManager
                }

                Port
                {
                    //color: "#f57c00"
                    id:outPort
                    x:model.portX
                    y:model.portY
                    width: model.portWidth
                    height:model.portHeight
                    borderColor: "#ff9800"
                    color: model.hasConnection === true || outPortMouseArea.containsMouse || connectionPreview.dragActive ? "#cc7a00" : "#404040"

                    MouseArea
                    {
                        id: outPortMouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        //acceptedButtons: Qt.LeftButton
                        drag.target: connectionPreview

                        drag.onActiveChanged:
                        {
                            if(drag.active)
                            {
                                let startPos = outPort.mapToItem(node.parent, outPort.width / 2, outPort.height / 2)
                                //let viewPos = graphicsView.mapFromScene(startPos.x,startPos.y);
                                connectionPreview.start = startPos
                                connectionPreview.x = 0
                                connectionPreview.y = 0
                                connectionPreview.end = startPos
                                connectionPreview.portIndex = model.dataIndex
                            }
                            else
                            {
                                connectionPreview.portIndex = undefined
                            }

                            connectionPreview.dragActive = drag.active
                        }
                    }
                }
            }


        // Output port (right)

    }
}
