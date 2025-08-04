import QtQuick 2.12
import QtQuick.Shapes 1.12
Shape {
    property alias color: path.fillColor
    property alias borderColor:path.strokeColor
    id:shape
    ShapePath {
        id:path
        strokeWidth: 1
        strokeColor: "#0097a7"
        fillColor: "transparent"
        startX: 0; startY: 0
        PathLine { x: shape.width; y: shape.height / 2 }
        PathLine { x: 0; y: shape.height }
        PathLine { x: 0; y: 0 }
        PathLine { x: 0; y: 0 }
    }
}
