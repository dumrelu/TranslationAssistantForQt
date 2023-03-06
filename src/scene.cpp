#include "scene.h"

#include <QQuickWindow>

namespace qta
{

QQuickItem* TextItem::item()
{
    return m_item;
}

QString TextItem::text()
{
    return "TODO: implement me!";
}

TextItem::TextItem(QObject* parent, QQuickItem* item)
    : QObject{ parent }
    , m_item{ item }
{
}

Scene::Scene(QQuickWindow* window)
    : m_window{ window }
{
    Q_ASSERT(m_window);
}

void Scene::start()
{
    //TODO: 
    //  1. Install event filters for this window
    //  2. Get(and save?) a list of all TextItem
    //  3. Inside TextItem, connect and forward the appropriate signals when text changes
    //  4. Send textChanged signals for all TextItems
}

QList<TextItem*> Scene::textItemsAt(QPoint position)
{
    //TODO
    Q_UNUSED(position);
    return QList<TextItem*>{};
}

}