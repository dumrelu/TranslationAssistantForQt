import QtQuick 2.8
import QtQuick.Controls 2.1

ListView {
    id: root

    spacing: 10
    leftMargin: 10
    rightMargin: 10

    delegate: TranslationListViewDelegate {
        width: root.width - root.leftMargin - root.rightMargin
    }
}
