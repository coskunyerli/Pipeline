import QtQuick 2.12
import QtQuick.Shapes 1.12

Shape {
    id: connection
    property point start
    property point end
    property color borderColor;
    property color color;
    x:0
    y:0
    width: Math.max(start.x,end.x) + 10
    height: Math.max(start.y,end.y) + 10
    layer.enabled: true
    layer.smooth: true
    antialiasing: true
    ShapePath {
        strokeWidth: 6
        strokeColor: connection.color
        fillColor: "transparent"
        capStyle: ShapePath.RoundCap
        joinStyle: ShapePath.RoundJoin

        startX: start.x
        startY: start.y

        PathCubic {
            x: end.x
            y: end.y
            control1X: start.x + (end.x - start.x) / 2
            control1Y: start.y
            control2X: end.x - (end.x - start.x) / 2
            control2Y: end.y
        }
    }

    ShapePath {
        strokeWidth: 3
        strokeColor: connection.borderColor
        fillColor: "transparent"
        capStyle: ShapePath.RoundCap
        joinStyle: ShapePath.RoundJoin

        startX: start.x
        startY: start.y

        PathCubic {
            x: end.x
            y: end.y
            control1X: start.x + (end.x - start.x) / 2
            control1Y: start.y
            control2X: end.x - (end.x - start.x) / 2
            control2Y: end.y
        }
    }
}
