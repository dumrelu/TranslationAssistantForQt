import QtQuick 2.8
import QtQuick.Controls 2.1
import TranslationAssistant 1.0

ListView {
    id: root

    spacing: 10
    leftMargin: 10
    rightMargin: 10

    delegate: TranslationListViewDelegate {
        id: delegate 

        width: root.width - root.leftMargin - root.rightMargin
        selected: index === root.currentIndex
        expanded: selected

        onTitleClicked: function() {
            if (root.currentIndex !== index) 
            {
                root.currentIndex = index;
                TranslationAssistant.translationClicked(model.id);
            }
            else
            {
                root.currentIndex = -1;
                TranslationAssistant.translationClicked();
            }
        }
    }
}
