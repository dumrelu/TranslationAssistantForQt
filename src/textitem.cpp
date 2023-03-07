#include "textitem.h"

namespace qta
{

TextItem::TextItem(QQuickItem *item)
    : m_item{ item }
{
}

QQuickItem* TextItem::item()
{
    return m_item;
}

}