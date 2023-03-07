#include "textitemhandler.h"

namespace qta
{

class TextPropertyTextItem : public TextItem
{
public:
    TextPropertyTextItem(QQuickItem* item, QMetaProperty textProperty)
        : TextItem{ item }
    {
        Q_ASSERT(item);

        connect(
            item, textProperty.notifySignal(), 
            this, QMetaMethod::fromSignal(&TextPropertyTextItem::textChanged)
        );
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
    auto* metaObject = item->metaObject();
    const auto textPropertyIndex = metaObject->indexOfProperty("text");
    if(textPropertyIndex < 0)
    {
        return {};
    }

    auto textProperty = metaObject->property(textPropertyIndex);
    if(!textProperty.hasNotifySignal())
    {
        return {};
    }

    return { new TextPropertyTextItem{ item, textProperty } };
}

}