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
        expanded: index === root.currentIndex
        selected: TranslationAssistant.selectedTranslationText ? false : expanded
        
        onTitleClicked: function() {
            if (root.currentIndex !== index) 
            {
                root.currentIndex = index;

                if(!TranslationAssistant.selectedTranslationText)
                    TranslationAssistant.translationClicked(model.id);
            }
            else
            {
                root.currentIndex = -1;
                if(!TranslationAssistant.selectedTranslationText)
                    TranslationAssistant.translationClicked();
            }
        }
    }
}
