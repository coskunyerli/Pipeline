import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Pipeline.Models as PM
ColumnLayout
{
    id:root
    property var referenceModel
    readonly property alias model: dialogModel
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
            index: modelProxyModel.currentIndex
            onClicked: (modelIndex) =>
                       {
                           modelProxyModel.currentIndex = dialogModel.index(modelIndex.row,modelIndex.column,modelIndex.parent)
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
                text: modelProxyModel.currentIndexValue
                onTextChanged: {
                    if(modelProxyModel.currentIndexValue !== text)
                    {
                        modelProxyModel.currentIndexValue = text
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
                text: dialogModel.rows
                onTextChanged: {
                    let val = Number(text)
                    if(dialogModel.rows !== val)
                        dialogModel.rows = val
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
                text: dialogModel.columns
                onTextChanged: {
                    let val = Number(text)
                    if(dialogModel.columns !== val)
                        dialogModel.columns = val
                }
            }
        }
    }

    TableGridWidget {
        id: inputTable
        Layout.fillWidth: true
        Layout.fillHeight: true
        // copy inputModel inside

        model:PM.NodeTableSliceProxyModel
        {
            id: modelProxyModel
            sourceModel:PM.NodeTableDialogModel
            {
                id: dialogModel
                referenceModel: root.referenceModel
            }
        }

        onCellDClicked:(row,column) =>
        {
            let modelIndex = dialogModel.index(row,column, modelProxyModel.currentIndex);
            if(!modelIndex.data(Qt.UserRole + 1))
            {
                modelIndex = dialogModel.createCell(modelIndex);
            }
            modelProxyModel.currentIndex = modelIndex
        }
    }
}


