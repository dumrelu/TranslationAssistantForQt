#include "textitem.h"

namespace qta
{

TextItem::TextItem(QQuickItem *item)
    : m_item{ item }
{
    Q_ASSERT(item);
    connect(item, &QObject::destroyed, this, &TextItem::itemDestroyed);
}

QQuickItem* TextItem::item()
{
    return m_item;
}

}