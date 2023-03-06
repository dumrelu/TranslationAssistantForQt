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

    findWindow();
    findAllTextItems(m_rootObject);

    for(auto* textItem : m_textItems)
    {
        emit textChanged(textItem);
        qDebug() << textItem->item();
    }
}

void Scene::findWindow()
{
    //TODO: how to implement this? Maybe recursive until we find a QQuickItem and get the window from it
    m_window = qobject_cast<QQuickWindow*>(m_rootObject);
    qDebug() << "Window: " << m_window;
}

void Scene::findAllTextItems(QObject* obj)
{
    if(auto* item = qobject_cast<QQuickItem*>(obj))
    {
        if(itemContainsText(item))
        {
            m_textItems.insert(new TextItem{this, item});
        }
    }

    for(auto* child : obj->children())
    {
        findAllTextItems(child);
    }
}

bool Scene::itemContainsText(QQuickItem *item) const
{
    if(!item)
    {
        return false;
    }

    const auto textVariant = item->property("text");
    return textVariant.isValid();
}

}