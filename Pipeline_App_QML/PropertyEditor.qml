import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.15

Rectangle
{
    id:propertyEditor
    property var node
    color:"#202020"
    ColumnLayout
    {
        anchors.fill: parent
        anchors.leftMargin: 8
        anchors.topMargin: 8
        anchors.rightMargin: 8
        anchors.bottomMargin: 8

        Label
        {
            text:"Path"
            color:"#eeeeee"
        }
        Rectangle
        {
            Layout.fillWidth: true
            height:1
            color: "#dddddd"
        }

        TextEdit
        {
            Layout.fillWidth: true
            text: propertyEditor.node && propertyEditor.node.pythonFilename || ""
            onTextChanged:
            {
                if(text === propertyEditor.node.pythonFilename)
                {
                    return;
                }

                propertyEditor.node.pythonFilename = text
            }

            color: "#eeeeee"
        }


        Item
        {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}
