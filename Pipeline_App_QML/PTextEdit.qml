import QtQuick 2.12

TextEdit {
    height: 28
    padding: 6
    Rectangle
    {
        z:-1
        radius: 3
        anchors.fill: parent
        color:"#505050"
        border.color: "#656565"
    }

    color: "#e0e0e0"
    selectByMouse: true
}
