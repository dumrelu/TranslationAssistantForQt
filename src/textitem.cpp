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