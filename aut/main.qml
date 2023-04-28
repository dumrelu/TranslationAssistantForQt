import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12

Window {
    id: window

    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World", "disambiguation", 2)

    Rectangle {
        id: rect1
        width: window.width / 2
        height: parent.height / 2
        color: "red"

        Label {
            anchors.centerIn: parent
            objectName: "text1"
            text: qsTr("Text1")
        }
    }

    Rectangle {
        id: rect3
        anchors.top: rect1.bottom
        width: window.width / 2
        height: parent.height / 2
        
        color: "green"
        
        ListView {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            height: parent.height
            model: 1000
            clip: true

            delegate: ItemDelegate {
                id: delegate
                text: "ListViewText#" + index

                Timer {
                    interval: 2000
                    running: index === 2
                    onTriggered: {
                        delegate.text = "ListViewText#" + index + "_changed"
                    }
                }
            }
        }
    }

    Rectangle {
        id: rect2
        color: "blue"
        anchors.left: rect1.right
        anchors.right: parent.right
        height: parent.height / 2

        MyQmlType {
            objectName: "text2"
        }
    }

    Rectangle {
        id: rect4
        color: "pink"
        anchors.top: rect2.bottom
        anchors.bottom: parent.bottom
        anchors.left: rect1.right
        anchors.right: parent.right

        Loader {
            id: textLoader
            anchors.centerIn: parent
            active: false
            sourceComponent: Text {
                objectName: "text4"
                text: qsTr("Text4")
            }
        }

        Timer {
            interval: 3000
            running: true
            onTriggered: {
                textLoader.active = true;
            }
        }
    }

    BusyIndicator {
        anchors.top: parent.top
        anchors.right: parent.right
        running: true
    }
}
