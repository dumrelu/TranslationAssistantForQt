import QtQuick 2.8
import QtQuick.Controls 2.1
import TranslationAssistant 1.0

ListView {
    id: root

    spacing: 10
    leftMargin: 10
    rightMargin: 10

    clip: true

    currentIndex: -1

    add: Transition {
        NumberAnimation { properties: "x,y"; duration: 500; easing.type: Easing.OutBounce }
    }

    displaced: Transition {
        NumberAnimation { properties: "x,y"; duration: 500; easing.type: Easing.OutBounce }
    }

    populate: Transition {
        id: populateTransition
        
        NumberAnimation {
            property: "y"
            duration: 500
            easing.type: Easing.OutBounce
            from: 0
            to: populateTransition.ViewTransition.destination.y
        }
    }

    delegate: TranslationDelegate {
        width: root.width
    }
}