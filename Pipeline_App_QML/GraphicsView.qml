import QtQuick 2.12
import QtQuick.Controls 2.12


Rectangle
{
    id: container

    function setPos(sceneX, sceneY)
    {
        let flickPoint = privateObject.mapToFlickable(sceneX,sceneY)
        flickArea.contentX = flickPoint.x
        flickArea.contentY = flickPoint.y
        privateObject.fitContent();
    }

    function move(sceneDiffX, sceneDiffY)
    {
        let flickPoint = privateObject.mapToFlickable(sceneDiffX,sceneDiffY)
        flickArea.contentX += flickPoint.x
        flickArea.contentY += flickPoint.y
        privateObject.fitContent();
    }
    function ensureVisible(sceneTopLeft, sceneBottomRight)
    {
        let flickTopLeftPos = privateObject.mapToFlickable(sceneTopLeft.x,sceneTopLeft.y)

        let flickBottomRightPos = privateObject.mapToFlickable(sceneBottomRight.x,sceneBottomRight.y)

        if (flickArea.contentX >= flickTopLeftPos.x)
        {
            flickArea.contentX = flickTopLeftPos.x
        }
        else if (flickArea.contentX+flickArea.width <= flickBottomRightPos.x)
        {
            flickArea.contentX = flickBottomRightPos.x-flickArea.width
        }
        if (flickArea.contentY >= flickTopLeftPos.y)
        {
            flickArea.contentY = flickTopLeftPos.y
        }
        else if (flickArea.contentY+flickArea.height <= flickBottomRightPos.y)
        {
            flickArea.contentY = flickBottomRightPos.y-flickArea.height
        }

        privateObject.fitContent();
    }

    function zoom(value, scenePos)
    {
        let flickPos = privateObject.mapToFlickable(scenePos.x,scenePos.y)
        let newZoom = inner.zoomValue * value
        inner.zoomValue = newZoom
        flickArea.resizeContent(inner.width * newZoom, inner.height * newZoom, flickPos)
        privateObject.fitContent();
    }

    function setZoom(value, scenePos)
    {
        let flickPos = privateObject.mapToFlickable(scenePos.x,scenePos.y)
        let newZoom = value
        inner.zoomValue = newZoom
        flickArea.resizeContent(inner.width * newZoom, inner.height * newZoom, flickPos)
        privateObject.fitContent();
    }

    function mapToScene(x, y)
    {
        let flickPoint = Qt.point(flickArea.contentX + x, flickArea.contentY + y);
        return privateObject.mapFromFlickable(flickPoint.x, flickPoint.y)
    }

    function mapFromScene(sceneX, sceneY)
    {
        let flickPoint = privateObject.mapToFlickable(sceneX, sceneY)
        return Qt.point(flickPoint.x - flickArea.contentX, flickPoint.y - flickArea.contentY)
    }


    function zoomArea(sign, pos)
    {
        let value = Math.max(graphicsViewMouseArea.minZoom, Math.min(graphicsViewMouseArea.maxZoom, graphicsView.zoomValue + (sign < 0 ? -0.1 : 0.1)))
        pos = pos || Qt.point(0,0);
        container.setZoom(value, pos);
    }

    QtObject
    {
        id: privateObject
        function fitContent()
        {
            flickArea.contentX =  Math.max(Math.min(flickArea.contentX, flickArea.contentWidth - flickArea.width), 0)
            flickArea.contentY = Math.max(Math.min(flickArea.contentY, flickArea.contentHeight - flickArea.height), 0)
        }

        function mapToFlickable(sceneX,sceneY)
        {
            return Qt.point(sceneX / inner.width * flickArea.contentWidth , sceneY / inner.height *  flickArea.contentHeight)

        }

        function mapFromFlickable(flickX,flickY)
        {
            return Qt.point(flickX / flickArea.contentWidth * inner.width , flickY / flickArea.contentHeight * inner.height)
        }
    }

    default property alias content: inner.children
    property alias scene: inner
    readonly property double zoomValue : inner.zoomValue
    property alias horizontalScrollBar: hbar
    property alias verticalScrollBar: vbar
    property alias interactive: flickArea.interactive
    property alias contentHeight :flickArea.contentHeight
    property alias contentWidth :flickArea.contentWidth

    readonly property rect visibleContentArea :
    {
        let imageStartPoint = container.mapToScene(0, 0)
        let imageEndPoint = container.mapToScene(container.width, container.height)
        return Qt.rect(imageStartPoint.x,imageStartPoint.y,imageEndPoint.x - imageStartPoint.x, imageEndPoint.y - imageStartPoint.y);
    }

    Item
    {
        width: Math.min(container.width, flickArea.contentWidth)
        height: Math.min(container.height, flickArea.contentHeight)

        Flickable
        {
            id: flickArea
            clip:true
            interactive: false
            anchors.fill : parent
            focus: true
            boundsBehavior: Flickable.StopAtBounds

            Rectangle
            {
                width: childrenRect.width
                height: childrenRect.height
                property real zoomValue:1
                transformOrigin: Item.TopLeft
                id: inner
                scale:inner.zoomValue



                onWidthChanged:
                {
                    flickArea.contentWidth= inner.width * inner.zoomValue
                }
                onHeightChanged:
                {
                    flickArea.contentHeight= inner.height * inner.zoomValue
                }

                // DropArea
                // {
                //     anchors.fill: parent
                //     onPositionChanged:
                //     {
                //         console.log(drag.x,drag.y)
                //     }
                //     z : -1
                // }
            }
        }
    }

    ScrollBar
    {
        id: vbar
        hoverEnabled: true
        active: hovered || pressed
        policy: flickArea.contentHeight <= container.height ? ScrollBar.AlwaysOff : ScrollBar.AlwaysOn
        orientation: Qt.Vertical
        size: flickArea.height / flickArea.contentHeight
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        onPositionChanged: {
            flickArea.contentY = position * flickArea.contentHeight
        }
        Connections
        {
            target: flickArea
            function onContentYChanged() {
                let newPosition = flickArea.contentY / flickArea.contentHeight;
                if(vbar.position !== newPosition) {
                    vbar.position = newPosition;
                }
            }
        }
    }

    ScrollBar
    {
        id: hbar
        hoverEnabled: true
        active: hovered || pressed
        policy: flickArea.contentWidth <= container.width ? ScrollBar.AlwaysOff : ScrollBar.AlwaysOn
        orientation: Qt.Horizontal
        size: flickArea.width / flickArea.contentWidth
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        onPositionChanged: {
            flickArea.contentX = position * flickArea.contentWidth
        }
        Connections
        {
            target: flickArea
            function onContentXChanged() {
                let newPosition = flickArea.contentX / flickArea.contentWidth;
                if(hbar.position !== newPosition) {
                    hbar.position = newPosition;
                }
            }
        }
    }

    MouseArea
    {
        id:graphicsViewMouseArea
        property point pressedPoint: Qt.point(-1,-1)
        property double minZoom: 0.5
        property double maxZoom: 1.5
        anchors.fill: parent
        acceptedButtons: Qt.MiddleButton
        hoverEnabled: false
        z:-2
        onPressed:
        {
            graphicsViewMouseArea.pressedPoint = graphicsView.mapToScene(mouse.x,mouse.y)
        }
        onReleased:
        {
            graphicsViewMouseArea.pressedPoint = Qt.point(-1,-1)
        }
        onPositionChanged:
        {
            let scenePos = container.mapToScene(mouse.x, mouse.y)
            container.move(graphicsViewMouseArea.pressedPoint.x - scenePos.x, graphicsViewMouseArea.pressedPoint.y - scenePos.y)
            graphicsViewMouseArea.pressedPoint = container.mapToScene(mouse.x, mouse.y);
        }
        onWheel:(wheel) =>
        {
            let sign = Math.abs(wheel.angleDelta.y) / wheel.angleDelta.y
            if(wheel.modifiers & Qt.ControlModifier)
            {
                let scenePos = container.mapToScene(wheel.x,wheel.y)
                container.zoomArea(sign, scenePos)
                wheel.accepted = true
                return
            }
            else
            {
                container.move(0, -sign * 10)
            }
            wheel.accepted = false
        }

        Component.onCompleted:
        {
            cursorShape = undefined
        }
    }

    scene.width: Math.max(container.width / graphicsViewMouseArea.minZoom,
                          (container.width / container.zoomValue).toFixed(4),
                          container.scene.childrenRect.right + 50)

    scene.height: Math.max(container.height / graphicsViewMouseArea.minZoom,
                           (container.height / container.zoomValue).toFixed(4),
                           container.scene.childrenRect.bottom + 50)
}
