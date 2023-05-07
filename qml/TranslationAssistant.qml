import QtQuick 2.0

Item {
    id: root

    anchors.fill: parent

    Item {
        id: trayContainer

        anchors.top: parent.top
        anchors.right: parent.right

        width: parent.width / 2.5
        height: parent.height

        Rectangle {
            anchors.fill: parent
            color: "red"
        }

        state: "visible"
        states: [
            State {
                name: "visible"
            },
            State {
                name: "hidden"
                PropertyChanges {
                    target: trayContainer
                    width: 0
                }
            }
        ]

        transitions: Transition {
            from: "*"
            to: "*"

            NumberAnimation {
                properties: "width"
                duration: 500
                easing.type: Easing.InOutQuad
            }
        }
    }

    Timer {
        running: true
        interval: 2000
        repeat: true
        onTriggered: function() {
            if (trayContainer.state == "visible") {
                trayContainer.state = "hidden"
            } else {
                trayContainer.state = "visible"
            }
        }
    }
}
