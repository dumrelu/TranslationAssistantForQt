#include "scene.h"

#include <QQuickWindow>

#include <private/qhooks_p.h>

#include <iostream>

// Implementation notes(use hooks): 
//  https://codebrowser.dev/qt5/qtbase/src/corelib/global/qhooks_p.h.html
//  https://github.com/KDAB/GammaRay/blob/master/probe/hooks.cpp 
namespace qta
{

//TODO: synchonizing?
QHash<QQuickWindow*, Scene*> Scene::g_scenes;
bool Scene::g_hooksInstalled = false;
std::unordered_set<QObject*> Scene::g_objectQueue;

Scene::Scene(QQuickWindow* window)
    : m_window{ window }
{
    Q_ASSERT(m_window);

    g_scenes.insert(m_window, this);
    m_window->installEventFilter(this);

    // TODO: These should be configured from outside
    m_textItemHandlers.push_back(std::make_shared<TextPropertyItemHandler>());
}

Scene::~Scene()
{
    g_scenes.remove(m_window);

    //TODO: call stop
}

void Scene::start()
{
    if(!g_hooksInstalled)
    {
        installHooks();
    }

    createTextItemsForAllItems(m_window->contentItem());
}

bool Scene::eventFilter(QObject* obj, QEvent* event)
{
    if(obj == m_window)
    {
        //TODO: clicks

        // false -> propagate
        return false;
    }
    return QObject::eventFilter(obj, event);
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

            // TextItem signal connections
            connect(textItem, &TextItem::textChanged, [this, textItem]()
                {
                    emit textChanged(textItem);
                }
            );
            connect(textItem, &TextItem::itemDestroyed, [this, textItem]()
                {
                    auto textItemsIt = m_textItems.find(textItem->item());
                    if(textItemsIt != m_textItems.end())
                    {
                        for(auto* textItem : textItemsIt.value())
                        {
                            emit textItemDestroyed(textItem);
                            textItem->deleteLater();
                        }
                        m_textItems.erase(textItemsIt);
                    }
                }
            );

            // Emit textChanged when TextItems are initially created
            //instead of a textCreated signal for simplicity.
            emit textChanged(textItem);
        }

        m_textItems.insert(item, std::move(textItems));
    }
}

void Scene::createTextItemsForAllItems(QQuickItem *item)
{
    createTextItemsIfRequired(item);
    for(auto* child : item->childItems())
    {
        createTextItemsForAllItems(child);
    }
}

void Scene::installHooks()
{
    Q_ASSERT(qtHookData[QHooks::HookDataVersion] >= 1);

    //TODO: Save existing hooks and call them from our hooks
    qtHookData[QHooks::AddQObject] = reinterpret_cast<quintptr>(Scene::addQObjectHook);
    qtHookData[QHooks::RemoveQObject] = reinterpret_cast<quintptr>(Scene::removeQObjectHook);
    g_hooksInstalled = true;
}

void Scene::itemAdded(QQuickItem *item)
{
    Q_ASSERT(item);
    createTextItemsIfRequired(item);
}

void Scene::itemRemoved(QQuickItem *item)
{
    Q_UNUSED(item);
}

void Scene::addQObjectHook(QObject* object)
{
    if(g_scenes.empty())
    {
        return;
    }

    //TODO: sync
    g_objectQueue.insert(object);

    QMetaObject::invokeMethod(
        // Use the first scene as a context for synchronization
        g_scenes.begin().value(), 
        [&, object]()
        {
            if(g_objectQueue.count(object) == 0)
            {
                return;
            }

            auto* item = dynamic_cast<QQuickItem*>(object);
            if(item)
            {
                auto sceneIt = g_scenes.find(item->window());
                // TODO: There are possible edge cases where the item
                //was not yet constructed if there are multiple threads
                Q_ASSERT(sceneIt != g_scenes.end());
                
                if(sceneIt != g_scenes.end())
                {
                    auto* scene = sceneIt.value();
                    // Invoke itemAdded from the Scene thread
                    QMetaObject::invokeMethod(
                        sceneIt.value(), 
                        [item, scene]()
                        {
                            if(g_objectQueue.count(static_cast<QObject*>(item)) > 0)
                            {
                                scene->itemAdded(item);
                                g_objectQueue.erase(static_cast<QObject*>(item));
                            }
                        }, 
                        Qt::QueuedConnection
                    );
                }
            }
        }, 
        Qt::QueuedConnection
    );
}

void Scene::removeQObjectHook(QObject* object)
{
    if(g_scenes.empty())
    {
        return;
    }

    //TODO: sync
    g_objectQueue.erase(object);
}

}