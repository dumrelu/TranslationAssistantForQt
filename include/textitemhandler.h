#pragma once

#include "textitem.h"

namespace qta
{

/// @brief Interface for creating TextItems from QQuickItems
/**
 *  The Scene will use call a list of TextItemHandlers until one
 * of them is able to create a TextItem (Chain of Command design pattern)
*/
class TextItemHandler
{
public:
    virtual ~TextItemHandler() = default;

    /// @brief Try creating a TextItem for the given QQuickItem
    /// @param item 
    /// @return A new TextItems(that are not memory managed) if successful. Empty list otherwise
    virtual QList<TextItem*> createTextItem(QQuickItem* item) = 0;
};


/// @brief Create a TextItem for any QQuickItem that has a 
///(non-dynamic) property called "text".
/**
 *  The property should have a getter and a signal.
*/
class TextPropertyItemHandler : public TextItemHandler {
public:

    QList<TextItem*> createTextItem(QQuickItem* item) override;
};

}