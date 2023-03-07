#include "textitemhandler.h"

namespace qta
{

class TextPropertyTextItem : public TextItem
{
public:
    TextPropertyTextItem(QQuickItem* item)
        : TextItem{ item }
    {
        Q_ASSERT(item);

        // TODO: connect to the signal
    }

    QString text() override
    {
        auto textProperty = m_item->property("text");
        Q_ASSERT(textProperty.canConvert<QString>());

        return textProperty.toString();
    }
};

QList<TextItem*> TextPropertyItemHandler::createTextItem(QQuickItem *item)
{
    // TODO: proper implementation(check meta object that it has a "text" prop with signal)
    if(item && item->property("text").isValid())
    {
        return { new TextPropertyTextItem{ item } };
    }
    return {};
}

}