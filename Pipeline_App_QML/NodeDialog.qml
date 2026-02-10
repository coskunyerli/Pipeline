import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import QtQuick.Dialogs

Window {
    id: detachedDialog
    width: 1100
    height: 600
    title: "Python Node Dialog"
    flags: Qt.Dialog | Qt.WindowCloseButtonHint | Qt.WindowTitleHint
    modality: Qt.NonModal
    visible: true
    color: "#303030"
    signal dialogClosed()

    property alias model : tableView.model
    property alias pythonFilename : pythonFilenameTextEdit.text

    onClosing: dialogClosed()

    FileDialog {
        id: fileDialog
        title: "Select File"
        nameFilters: ["Python Files (*.py)"]
        fileMode: FileDialog.OpenFile

        onAccepted: {
            pythonFilename = selectedFile
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        RowLayout
        {
            Layout.fillWidth: true
            Label
            {
                color : tableView.textColor
                text: "Python File: "
            }

            TextEdit
            {
                id: pythonFilenameTextEdit
                Layout.fillWidth: true
                color : tableView.textColor
            }
            Button
            {
                text:"Select File"
                onClicked:
                {
                    fileDialog.open()
                }
            }
        }

        GridLayout
        {
            columns: 2
            Layout.fillWidth: true
            Label
            {
                color : tableView.textColor
                text: "Row Count: "
            }

            TextEdit
            {
                Layout.fillWidth: true
                color : tableView.textColor
                text: model.rows
                onTextChanged:
                {
                    let val = Number(text);
                    if(model.rows !== val)
                    {
                        model.rows = val
                    }
                }
            }

            Label
            {
                color : tableView.textColor
                text: "Column Count: "
            }

            TextEdit
            {
                Layout.fillWidth: true
                color : tableView.textColor
                text: model.columns
                onTextChanged:
                {
                    let val = Number(text);
                    if(model.columns !== val)
                    {
                        model.columns = val
                    }
                }
            }
        }

        TableGridWidget
        {
            id: tableView
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        RowLayout
        {
            Layout.fillWidth: true
            spacing: 8
            Item
            {
                Layout.fillWidth: true
            }

            Button {
                Layout.preferredWidth: 80
                text: "Cancel"
                onClicked: detachedDialog.close()
            }

            Button {
                Layout.preferredWidth: 80
                text: "OK"
                highlighted: true
                onClicked: {
                    detachedDialog.close()
                }
            }
        }
    }
}
