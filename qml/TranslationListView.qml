import QtQuick 2.8
import QtQuick.Controls 2.1
import TranslationAssistant 1.0

ListView {
    id: root

    property bool highlightSelections: true

    spacing: 10
    leftMargin: 10
    rightMargin: 10

    clip: true

    currentIndex: -1

    delegate: TranslationListViewDelegate {
        id: delegate 

        width: root.width - root.leftMargin - root.rightMargin
        expanded: index === root.currentIndex
        selected: root.highlightSelections ? expanded : false
        
        onTitleClicked: function() {
            if (root.currentIndex !== index) 
            {
                root.currentIndex = index;

                if(root.highlightSelections)
                    TranslationAssistant.translationClicked(model.id);
            }
            else
            {
                root.currentIndex = -1;
                if(root.highlightSelections)
                    TranslationAssistant.translationClicked();
            }
        }
    }
}
