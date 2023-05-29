import QtQuick 2.8
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.1
import TranslationAssistant 1.0

Page {
    id: root

    title: qsTr("Translation Assistant")

    TranslationListView {
        anchors.fill: parent

        model: TranslationAssistant
    }
}