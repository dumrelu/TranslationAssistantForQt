#include "textitem.h"

#include <QDebug>

namespace qta
{
TextItem::TextItem(QQuickItem *item)
{
    Q_ASSERT(item);

    connect(
        item, &QQuickItem::destroyed, 
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
        qDebug() << "Text item is invalid";
        return {};
    }
    return doGetText();
}
}