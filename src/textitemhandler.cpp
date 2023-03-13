#include "textitemhandler.h"

namespace ta
{

class TextPropertyTextItem : public TextItem
{
public:
    TextPropertyTextItem(QQuickItem* item, QMetaProperty textProperty)
        : TextItem{ item }
    {
        Q_ASSERT(item);
        Q_ASSERT(textProperty.isValid());

        connect(
            item, textProperty.notifySignal(), 
            this, QMetaMethod::fromSignal(&TextPropertyTextItem::textChanged)
        );
    }

    QString doGetText() override
    {
        auto textProperty = m_item->property("text");
        Q_ASSERT(textProperty.canConvert<QString>());

        return textProperty.toString();
    }
};

QList<QSharedPointer<TextItem>> TextPropertyItemHandler::createTextItem(QQuickItem* item)
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

    return {
        QSharedPointer<TextPropertyTextItem>::create(item, std::move(textProperty))
    };
}

}