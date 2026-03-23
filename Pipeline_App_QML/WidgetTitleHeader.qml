import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ColumnLayout
{
    property alias text : label.text

    default property alias actions: actionRow.data

    spacing: 0

    Rectangle
    {
        color: "#404040"
        Layout.preferredHeight: 28
        Layout.fillWidth: true

        RowLayout
        {
            anchors.fill: parent
            anchors.leftMargin: 8
            anchors.rightMargin: 8
            spacing: 6

            Label
            {
                id: label
                verticalAlignment: Text.AlignVCenter
            }

            Item { Layout.fillWidth: true } // spacer

            RowLayout
            {
                id: actionRow
                spacing: 4
            }
        }
    }

    Rectangle
    {
        height: 1
        Layout.fillWidth: true
        color: "#656565"
    }
}
