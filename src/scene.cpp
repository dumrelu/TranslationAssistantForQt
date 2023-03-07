#include "scene.h"

#include <QQuickWindow>

namespace qta
{

Scene::Scene(QQuickWindow* window)
    : m_window{ window }
{
    Q_ASSERT(m_window);

    // TODO: These should be configured from outside
    m_textItemHandlers.push_back(std::make_shared<TextPropertyItemHandler>());
}

void Scene::start()
{
    //TODO: 
    //  1. Install event filters for this window
    //  2. Get(and save?) a list of all TextItem
    //  3. Inside TextItem, connect and forward the appropriate signals when text changes
    //  4. Send textChanged signals for all TextItems

    createTextItemsForEntireScene(m_window->contentItem());
    for(auto& textItems : m_textItems)
    {
        for(auto* textItem : textItems)
        {
            textChanged(textItem);
        }
    }
}

void Scene::createTextItemsForEntireScene(QQuickItem* root)
{
    createTextItemsIfRequired(root);
    for(auto* child : root->childItems())
    {
        createTextItemsForEntireScene(child);
    }
}

void Scene::createTextItemsIfRequired(QQuickItem *item)
{
    if(m_textItems.contains(item))
    {
        return;
    }

    QList<TextItem*> textItems;
    for(auto& textItemHandler : m_textItemHandlers)
    {
        textItems = textItemHandler->createTextItem(item);
        if(!textItems.empty())
        {
            break;
        }
    }

    if(!textItems.empty())
    {
        // Assign the scene as a parent for memory management
        for(auto* textItem : textItems)
        {
            textItem->setParent(this);
        }

        m_textItems.insert(item, std::move(textItems));
    }
}

}