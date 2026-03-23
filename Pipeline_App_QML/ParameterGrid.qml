import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs as QD
import Pipeline.Models as PM

GridLayout {
    id: root
    columns: repeater.model.maxColumns * columnValue
    columnSpacing: 4
    rowSpacing: 6

    property var model
    property int columnValue : 1
    Repeater {
        id :repeater
        model: PM.NodeParamUIListModel
        {
            parameterModel:root.model
        }

        delegate: Loader {
            // sadece fillWidth TextEdit için
            Layout.columnSpan: model.columnSpan
            Layout.fillWidth: model.fillWidth
            sourceComponent: {
                switch(model.uiType) {
                case 0: return labelDelegate
                case 1: return inputDelegate
                case 2: return buttonDelegate
                }
            }
            property var modelData : model
        }
    }

    // =========================
    // Delegates
    // =========================
    Component {
        id: labelDelegate
        Label {
            text: modelData.text + ":"
            color: "#e0e0e0"
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
        }
    }

    Component {
        id: inputDelegate
        PTextEdit {
            text: modelData.text
            Layout.fillWidth: true
            onEditingFinished: {

                modelData.text = text
            }
        }
    }

    Component {
        id: buttonDelegate
        RowLayout {
            spacing: 4

            PButton {
                text: modelData.text
                Layout.fillWidth: true
                Layout.preferredWidth: 40
                onClicked: fileDialog.open()
            }

            QD.FileDialog {
                id: fileDialog
                onAccepted: {
                     modelData.text = selectedFile
                }
            }
        }
    }
}
