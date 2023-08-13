import QtQuick 2.8
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.2
import TranslationAssistant 1.0

ItemDelegate {
    id: root

    text: model.source

    onClicked: {
        TranslationAssistant.highlightTranslation(model.id);
    }
}