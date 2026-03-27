import QtQuick 2.12
import QtQuick.Controls
TextField {
    id: customTextField
    height: 28
    padding: 6
    color: "#e0e0e0"         // text color
    selectByMouse: true

    background: Rectangle {
        anchors.fill: parent
        radius: 3
        color: "#505050"
        border.color: "#656565"
        z: -1
    }

    // Eğer focus olunca border rengini değiştirmek istersen
    focus: false
    onActiveFocusChanged: {
        if(activeFocus) background.border.color = "#a0a0a0"
        else background.border.color = "#656565"
    }
}
