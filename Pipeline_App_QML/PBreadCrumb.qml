import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id:root
    width: 600
    color: "#252525"
    implicitHeight: breadcrumbRow.implicitHeight + 8
    signal clicked(var index);
    property var index
    property int endDepth:0
    readonly property var rootIndex: privateObject.pRootIndex
    QtObject
    {
        id:privateObject
        property var pRootIndex:null
    }

    onIndexChanged:
    {
        createIndexList(index)
    }

    function createIndexList(index)
    {
        breadcrumbModel.clear()

        if(!index)
        {
            return [];
        }

        let list = [];
        let temp = index;
        while(temp.parent.valid)
        {
            list.push({display:temp.data(Qt.UserRole + 3), modelIndex:temp, enable:true});
            temp = temp.parent;
        }

        privateObject.pRootIndex = temp;
        list.push({display:"/", modelIndex:temp, enable: list.length > 0});


        list.reverse();

        for(let i=0; i<list.length; i++)
            breadcrumbModel.append(list[i])
    }

    ListModel
    {
        id:breadcrumbModel
    }

    ColumnLayout
    {
        anchors.fill: parent
        id: breadcrumbRow
        RowLayout {
            spacing: 6
            Layout.leftMargin: 4
            Repeater {
                id:repeater
                model: breadcrumbModel

                RowLayout {
                    spacing: 4

                    Button {
                        id :button
                        enabled: model.enable
                        text: model.display

                        background: Rectangle {
                            color:
                            {
                                if(button.hovered)
                                {
                                    return "#3a3a3a";
                                }
                                else if(button.enabled)
                                {
                                    return "#252525";
                                }
                                else
                                {
                                    return "#404040";
                                }
                            }

                            radius: 4
                        }

                        contentItem: Text {
                            text: button.text
                            color: "#dddddd"
                            font.pixelSize: 14
                            verticalAlignment: Text.AlignVCenter
                        }

                        onClicked: {
                            root.clicked(model.modelIndex)
                        }
                    }

                    Text {
                        Layout.alignment: Text.AlignVCenter
                        visible: index < repeater.count - 1
                        text: ">"
                        color: "#888888"
                        font.pixelSize: 14
                    }
                }
            }
            Item
            {
                Layout.fillWidth: true
            }
        }

    }


}
