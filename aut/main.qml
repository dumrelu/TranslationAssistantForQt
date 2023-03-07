import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12

Window {
    id: window

    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

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
        
        Loader {
            id: text3Loader
            anchors.centerIn: parent
            active: false
            sourceComponent: Text {
                text: qsTr("Text3")
            }
        }

        Timer {
            interval: 3000
            running: true
            onTriggered: {
                text3Loader.active = true;
            }
        }
    }

    Rectangle {
        id: rect2
        color: "blue"
        anchors.left: rect1.right
        anchors.right: parent.right
        height: parent.height

        MyQmlType {
            objectName: "text2"
        }
    }

    BusyIndicator {
        anchors.top: parent.top
        anchors.right: parent.right
        running: true
    }
}
