#include "scene.h"

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

TextItem::TextItem(QQuickItem* item)
    : m_item{ item }
{
}

Scene::Scene(QObject* rootObject)
    : m_rootObject{ rootObject }
{
}

void Scene::start()
{
    //TODO: 
    //  1. Install event filters for this window
    //  2. Get(and save?) a list of all TextItem
    //  3. Inside TextItem, connect and forward the appropriate signals when text changes
    //  4. Send textChanged signals for all TextItems
}

}