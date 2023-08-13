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

    delegate: TranslationDelegate {
        width: root.width
    }
}