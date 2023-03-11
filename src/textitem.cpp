#include "textitem.h"

#include <QDebug>

namespace qta
{

TextItem::TextItem(QQuickItem *item)
    : m_item{ item }
{
    Q_ASSERT(m_item);

    connect(
        m_item, &QQuickItem::destroyed, 
        this, [this]()
        {
            m_item = nullptr;
            emit invalidated();
        }
    );
}

bool TextItem::isVisible() const
{
    return isValid() && m_item->isVisible() && m_item->opacity() > 0.0;
}

QString TextItem::text()
{
    if(!isValid())
    {
        qDebug() << "Could not retrieve text. TextItem is invalid";
        return {};
    }
    return doGetText();
}

}