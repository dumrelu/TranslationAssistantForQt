#include "scene.h"

#include <QQuickWindow>

#include <private/qhooks_p.h>

#include <iostream>

// Implementation notes(use hooks): 
//  https://codebrowser.dev/qt5/qtbase/src/corelib/global/qhooks_p.h.html
//  https://github.com/KDAB/GammaRay/blob/master/probe/hooks.cpp 
namespace qta
{

QList<Scene*> Scene::g_scenes;
bool Scene::g_hooksInstalled = false;

Scene::Scene(QQuickWindow* window)
    : m_window{ window }
{
    Q_ASSERT(m_window);

    g_scenes.push_back(this);
    m_window->installEventFilter(this);

    // TODO: These should be configured from outside
    m_textItemHandlers.push_back(std::make_shared<TextPropertyItemHandler>());
}

Scene::~Scene()
{
    g_scenes.removeAll(this);

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

    qtHookData[QHooks::AddQObject] = reinterpret_cast<quintptr>(Scene::addQObjectHook);
    qtHookData[QHooks::RemoveQObject] = reinterpret_cast<quintptr>(Scene::removeQObjectHook);
    g_hooksInstalled = true;
}

void Scene::addQObjectHook(QObject* object)
{
    Q_UNUSED(object);
    std::cout << "Object added\n";
    //TODO: Check first if QQuickItem then fwd
}

void Scene::removeQObjectHook(QObject* object)
{
    Q_UNUSED(object);
    std::cout << "Object removed\n";
    //TODO: Check first if QQuickItem then fwd
}

}