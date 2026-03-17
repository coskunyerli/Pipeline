import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ColumnLayout
{
    property alias text : label.text
    spacing: 0
    Rectangle
    {
        color: "#404040"

        Layout.preferredHeight: 28
        Layout.fillWidth: true
        Label
        {
            id:label
            anchors.fill: parent
            anchors.leftMargin: 8
            verticalAlignment: Text.AlignVCenter
        }
    }

    Rectangle
    {
        height: 1
        Layout.fillWidth: true
        color: "#656565"
    }
}

