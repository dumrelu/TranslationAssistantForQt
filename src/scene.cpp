#include "scene.h"

#include <QQuickWindow>

namespace qta
{

Scene::Scene(QQuickWindow* window)
    : m_window{ window }
{
    Q_ASSERT(m_window);
    m_window->installEventFilter(this);
    // TODO: qApp->installEventFilter(this);

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

    // TODO: connect to signals so that when QQuickItems get destroyed
    //we also destroy the TextItem
    createTextItemsForEntireScene(m_window->contentItem());

    connect(&m_searchForTextItemsTimer, &QTimer::timeout, [this]()
        {
            createTextItemsForEntireScene(m_window->contentItem());
        }
    );
    m_searchForTextItemsTimer.setInterval(std::chrono::milliseconds{ 500 });
    m_searchForTextItemsTimer.start();
}

bool Scene::eventFilter(QObject* obj, QEvent* event)
{
    if(obj == m_window)
    {
        //TODO: clicks

        // Continue to propagate event
        return false;
    }

    // TODO: ChildAdded/ChildRemoved/ParentChanged??
    
    return QObject::eventFilter(obj, event);
}

void Scene::createTextItemsForEntireScene(QQuickItem *root)
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
        for(auto* textItem : textItems)
        {
            // For memory management
            textItem->setParent(this);

            connect(textItem, &TextItem::textChanged, [this, textItem]()
                {
                    emit textChanged(textItem);
                }
            );

            emit textChanged(textItem);
        }

        m_textItems.insert(item, std::move(textItems));
    }
}

}