#pragma once

#include <QObject>
#include <QQuickItem>
#include <QQuickWindow>
#include <QSet>

#include "textitem.h"

namespace qta
{

//! Different flows
/*
o Clicking on an item:
    - Intercept mouse click events on the QQuickWindow
    - emit textItemsClicked(list<TextItem> textItems), where textItems are found by:
        - search through all the visible TextItems
        - Return all the items with the smalles "z"
    - Highlight these textItems on the UI
    - In the ListModel for the translation list, filter to only show the relevant translations
        - edge case: This thing -> [qsTr("This %s")]
        - edge case: This : thing -> qsTr("This") + ":" + qsTr("thing") -> [qsTr("This"), qsTr("thing")]
        - Maybe add a verification step? Or at least display a warning sign in case we are undertain?
        - Consider also showing adjacent translations(e.g. translations from consecutive lines above/below)
    - Edit translations:
        - Create a temporary .ts file and load it with a QTranslator
        - When editing, edit this temp .ts file, and reload it
    - Commit translations
        - Commit the temp changes to the original .ts

o Identifying TextItems
    - TextItem is an interface(getText cached if possible, textChanged signal)
        - Helper function in base class to connect to property's signal
    - Have a way to register TextItemFactorys
        - For each QQuickItem, return list<TextItem>
        - e.g. Might want to return multiple TextItems for different properties(e.g. a TextField property)
    - Some factories: one for QQuickText(and label) specific, one for TextFields, ets
*/


class Scene : public QObject 
{
    Q_OBJECT

public:
    explicit Scene(QQuickWindow* window);

    /// @brief Start processing the scene described by the 
    ///rootObject. Until this method is called, no signals
    //will fire and some methods won't work properly due to
    //lack of data.
    void start();

signals:
    void textItemsClicked(QList<TextItem*> textItems);
    void textChanged(TextItem* textItem);
    void textItemDestroyed(TextItem* textItem);

private:
    
    QQuickWindow* m_window = nullptr;
    QSet<TextItem*> m_textItems;
};

}