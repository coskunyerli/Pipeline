import QtQuick
import QtQuick.Controls as QC

Rectangle {

    color: "#1e1e1e"

    QC.ScrollView {
        anchors.fill: parent

        QC.TextArea {
            id: consoleText
            readOnly: true
            color: "#dddddd"
            font.family: "Consolas"
            //wrapMode: TextArea.Wrap
        }
    }

    // Connections {
    //     target: consoleBridge

    //     function onMessage(msg) {
    //         consoleText.append(msg)
    //     }

    //     function onError(msg) {
    //         consoleText.append("[ERROR] " + msg)
    //     }
    // }
}
