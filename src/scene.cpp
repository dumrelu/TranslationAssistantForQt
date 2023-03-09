#include "scene.h"
#include "scenehooks.h"

namespace qta
{

Scene::Scene(QQuickWindow *window)
    : m_window{ window }
{
    Q_ASSERT(m_window);

    // TODO: Should be able to configure these from outside
    m_textItemHandlers.push_back(QSharedPointer<TextPropertyItemHandler>::create());
}

Scene::~Scene()
{
    stop();
}

void Scene::start()
{
    SceneHooks::instance().subscribe(this);

    createTextItemsForSubtree(m_window->contentItem());
}

void Scene::stop()
{
    SceneHooks::instance().unsubscribe(this);
}

void Scene::addQQuickItemHook(QQuickItem *item)
{
    createTextItemIfRequired(item);
}

bool Scene::eventFilter(QObject *obj, QEvent *event)
{
    return QObject::eventFilter(obj, event);
}

void Scene::createTextItemsForSubtree(QQuickItem *root)
{
    createTextItemIfRequired(root);
    for(auto* child : root->childItems())
    {
        createTextItemsForSubtree(child);
    }
}

void Scene::createTextItemIfRequired(QQuickItem *item)
{
    if(m_textItems.contains(item))
    {
        return;
    }

    for(auto& textItemHandler : m_textItemHandlers)
    {
        auto textItems = textItemHandler->createTextItem(item);
        if(!textItems.empty())
        {
            insertTextItems(item, textItems);
            break;
        }
    }
}

void Scene::insertTextItems(QQuickItem* item, QList<QSharedPointer<TextItem>> textItems)
{
    for(auto& textItem : textItems)
    {
        auto* rawTextItemPointer = textItem.get();
        
        connect(
            rawTextItemPointer, &TextItem::textChanged,
            this, [this, rawTextItemPointer]()
            {
                auto sharedTextItemPtr = rawTextItemPointer->sharedFromThis();
                if(sharedTextItemPtr)
                {
                    emit textChanged(sharedTextItemPtr);
                }
            }
        );

        connect(
            rawTextItemPointer, &TextItem::invalidated, 
            this, [this, item]()
            {
                auto textItemsIt = m_textItems.find(item);
                if(textItemsIt != m_textItems.end())
                {
                    for(auto& textItem : textItemsIt.value())
                    {
                        emit textItemInvalidated(textItem);
                    }

                    m_textItems.erase(textItemsIt);
                }
            }
        );
    }

    m_textItems.insert(item, textItems);

    for(auto& textItem : textItems)
    {
        emit textChanged(textItem);
    }
}

}