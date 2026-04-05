import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Pipeline.Models as PM
ColumnLayout
{
    id:root
    readonly property var rootIndex :breadcrumb.rootIndex
    property alias proxyModel : inputTable.model
    signal breadcrumbClicked(var modelIndex);
    signal cellDClicked(var row, var column);
    spacing: 0
    RowLayout
    {
        Layout.fillWidth: true
        spacing: 6
        PBreadCrumb
        {
            Layout.preferredHeight: 32
            id : breadcrumb
            color: "transparent"
            Layout.fillWidth: true
            index: (proxyModel && proxyModel.currentIndex) || null
            onClicked: (modelIndex) =>
           {
               root.breadcrumbClicked(modelIndex);
           }
        }

        Rectangle
        {
            width: 2
            color:"#303030"
            Layout.preferredHeight:breadcrumb.implicitHeight - 8
        }

        RowLayout {
            spacing: 6
            Layout.rightMargin: 8
            Label {
                text: "Value"
                color: inputTable.textColor
            }

            PTextEdit {
                Layout.preferredHeight: 24
                Layout.preferredWidth: 60
                bottomPadding: 4
                topPadding: 4
                text: (proxyModel &&  proxyModel.currentIndexValue) || ""
                onTextChanged: {
                    if(proxyModel.currentIndexValue !== text)
                    {
                        proxyModel.currentIndexValue = text
                    }
                }
            }

            Label {
                text: "Row Count:"
                color: inputTable.textColor
            }



            PTextEdit {
                Layout.preferredHeight: 24
                Layout.preferredWidth: 60
                bottomPadding: 4
                topPadding: 4
                text: (proxyModel && proxyModel.rows ) || 0
                onTextChanged: {
                    let val = Number(text)
                    if(proxyModel && proxyModel.rows !== val)
                    {
                        proxyModel.rows = val
                    }
                }
            }


            Label {
                text: "Column Count:"
                color: inputTable.textColor
            }

            PTextEdit {
                Layout.preferredHeight: 24
                Layout.preferredWidth: 60
                bottomPadding: 4
                topPadding: 4
                text: (proxyModel && proxyModel.columns) || 0
                onTextChanged: {
                    let val = Number(text)
                    if( proxyModel && proxyModel.columns !== val)
                    {
                        proxyModel.columns = val
                    }
                }
            }
        }
    }

    TableGridWidget {
        id: inputTable
        Layout.fillWidth: true
        Layout.fillHeight: true
        // copy inputModel inside

        onCellDClicked:(row,column) =>
        {
            root.cellDClicked(row,column);
        }
    }
}


