import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
ColumnLayout
{
    property alias model : inputTable.model
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
            index: inputDialogProxyModel.currentIndex
            onClicked: (modelIndex) =>
                       {
                           inputDialogProxyModel.currentIndex = inputDialogModel.index(modelIndex.row,modelIndex.column,modelIndex.parent)
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
                text: inputDialogProxyModel.currentIndexValue
                onTextChanged: {
                    if(inputDialogProxyModel.currentIndexValue !== text)
                    {
                        inputDialogProxyModel.currentIndexValue = text
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
                text: inputDialogModel.rows
                onTextChanged: {
                    let val = Number(text)
                    if(inputDialogModel.rows !== val)
                        inputDialogModel.rows = val
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
                text: inputDialogModel.columns
                onTextChanged: {
                    let val = Number(text)
                    if(inputDialogModel.columns !== val)
                        inputDialogModel.columns = val
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
            let modelIndex = inputDialogModel.index(row,column, inputDialogProxyModel.currentIndex);
            if(!modelIndex.data(Qt.UserRole + 1))
            {
                modelIndex = inputDialogModel.createCell(modelIndex);
            }
            inputDialogProxyModel.currentIndex = modelIndex
        }
    }
}


