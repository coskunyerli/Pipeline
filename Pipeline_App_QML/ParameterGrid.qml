import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs as QD
import Pipeline.Models as PM

GridLayout {
    property int maxWidth : 0
    id: root
    columns: repeater.model.maxColumns * columnValue
    columnSpacing: 12
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
                case 3: return checkboxDelegate
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

        Item {
            Component.onCompleted:
            {
                if(rootItem.width > root.maxWidth)
                {
                    root.maxWidth = rootItem.width;
                }
            }

            id: rootItem
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            implicitHeight: 28
            implicitWidth: label.contentWidth
            Label {
                id: label
                anchors.fill: parent
                text: modelData.value + ":"
                color: "#e0e0e0"
                verticalAlignment: Text.AlignVCenter

                MouseArea {
                    anchors.fill: parent
                    onDoubleClicked: {
                        popup.open()
                        edit.text = modelData.value
                        edit.selectAll()
                        edit.forceActiveFocus()
                    }
                }
            }

            Popup {
                id: popup
                modal: false
                focus: true
                dim: true
                x: label.mapToItem(null, 0, 0).x
                y: label.mapToItem(null, 0, 0).y

                Overlay.modeless: Rectangle {
                    color: "#80000000"   // rgba: alpha=0.5
                }

                width: root.maxWidth + 8
                height: 28

                background: Rectangle {
                    radius: 3
                    color: "#505050"
                    border.color: "#656565"
                }

                contentItem: PTextEdit {
                    id: edit
                    anchors.fill: parent
                    selectByMouse: true

                    Keys.onReturnPressed: finish()
                    Keys.onEnterPressed: finish()
                    Keys.onEscapePressed: popup.close()

                    function finish() {
                        modelData.value = text
                        popup.close()
                    }

                    onFocusChanged: {
                        if (!focus) popup.close()
                    }
                }
            }
        }
    }

    Component {
        id: inputDelegate
        PTextEdit {
            text: modelData.value
            Layout.fillWidth: true
            onEditingFinished: {

                modelData.value = text
            }
        }
    }

    Component {
        id: checkboxDelegate
        CheckBox {

            id: control
            checked: modelData.value
            onClicked:
            {
                if(modelData.value !== checked)
                {
                    modelData.value = checked
                }
            }

            implicitHeight: 28
            spacing: 8

            // Text stilin
            contentItem: Text {
                text: control.text
                color: "#e0e0e0"
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: 13
            }

            indicator: Rectangle {
                anchors.verticalCenter: parent.verticalCenter
                width: 18
                height: 18
                radius: 3

                color: control.checked ? "#6c8cff" : "#505050"
                border.color: control.checked ? "#6c8cff" : "#656565"

                // hover efekti
                opacity: control.hovered ? 0.9 : 1.0

                // tik işareti
                Canvas {
                    anchors.fill: parent
                    visible: control.checked

                    onPaint: {
                        var ctx = getContext("2d")
                        ctx.reset()

                        ctx.strokeStyle = "#ffffff"
                        ctx.lineWidth = 2
                        ctx.beginPath()
                        ctx.moveTo(width * 0.2, height * 0.55)
                        ctx.lineTo(width * 0.45, height * 0.75)
                        ctx.lineTo(width * 0.8, height * 0.3)
                        ctx.stroke()
                    }
                }
            }

            background: Rectangle {
                anchors.fill: parent
                radius: 3
                color: "transparent"
            }
        }
    }

    Component {
        id: buttonDelegate
        RowLayout {
            spacing: 4

            PButton {
                text: modelData.value
                Layout.fillWidth: true
                Layout.preferredWidth: 40
                onClicked: fileDialog.open()
            }

            QD.FileDialog {
                id: fileDialog
                onAccepted: {
                     modelData.value = selectedFile
                }
            }
        }
    }
}
