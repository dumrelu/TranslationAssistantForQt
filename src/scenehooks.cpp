#include "scenehooks.h"

#include <iostream>

#include <QMutexLocker>
#include <private/qhooks_p.h>

namespace ta
{

static QHooks::AddQObjectCallback g_existingAddQObjectHook = nullptr;
static QHooks::RemoveQObjectCallback g_existingRemoveQObjectCallback = nullptr;

void SceneHooks::addQObjectHook(QObject* object)
{
    instance().addQObject(object);

    if(g_existingAddQObjectHook)
    {
        g_existingAddQObjectHook(object);
    }
}

void SceneHooks::removeQObjectHook(QObject* object)
{
    instance().removeQObject(object);

    if(g_existingRemoveQObjectCallback)
    {
        g_existingRemoveQObjectCallback(object);
    }
}

void SceneHooks::installHooks()
{
    Q_ASSERT(qtHookData[QHooks::HookDataVersion] >= 1);
    Q_ASSERT(qtHookData[QHooks::HookDataSize] >= 6);

    std::clog << "Installing hooks" << std::endl;

    g_existingAddQObjectHook = reinterpret_cast<QHooks::AddQObjectCallback>(qtHookData[QHooks::AddQObject]);
    g_existingRemoveQObjectCallback = reinterpret_cast<QHooks::RemoveQObjectCallback>(qtHookData[QHooks::RemoveQObject]);

    qtHookData[QHooks::AddQObject] = reinterpret_cast<quintptr>(&SceneHooks::addQObjectHook);
    qtHookData[QHooks::RemoveQObject] = reinterpret_cast<quintptr>(&SceneHooks::removeQObjectHook);
}

void SceneHooks::uninstallHooks()
{
    qtHookData[QHooks::AddQObject] = reinterpret_cast<quintptr>(g_existingAddQObjectHook);
    qtHookData[QHooks::RemoveQObject] = reinterpret_cast<quintptr>(g_existingRemoveQObjectCallback);
}

SceneHooks &SceneHooks::instance()
{
    static SceneHooks g_instance;
    return g_instance;
}

void SceneHooks::subscribe(Scene *scene)
{
    if(!scene)
    {
        return;
    }

    QMutexLocker lock{ &m_mutex };

    if(!m_hooksInstalled)
    {
        installHooks();
    }

    auto& sceneList = m_subscribers[scene->window()];
    if(!sceneList.contains(scene))
    {
        sceneList.push_back(scene);
        QObject::connect(scene, &Scene::destroyed, scene, [this, scene]()
            {
                unsubscribe(scene);
            }
        );
    }
}

void SceneHooks::unsubscribe(Scene *scene)
{
    if(!scene)
    {
        return;
    }

    QMutexLocker lock{ &m_mutex };

    auto sceneListIt = m_subscribers.find(scene->window());
    if(sceneListIt != m_subscribers.end())
    {
        auto& sceneList = sceneListIt.value();
        sceneList.removeAll(scene);
        if(sceneList.empty())
        {
            m_subscribers.erase(sceneListIt);
        }
    }

    if(m_subscribers.empty())
    {
        uninstallHooks();
        m_hooksInstalled = false;
    }
}

SceneHooks::~SceneHooks()
{
    uninstallHooks();
}

void SceneHooks::addQObject(QObject* object)
{
    // When the hooks get called, the objects are not
    //yet fully constructed. We post them in a queue and
    //process them at a later time.
    QMutexLocker lock{ &m_mutex };
    m_objectQueue.insert(object);

    if(m_objectQueue.size() == 1)
    {
        QMetaObject::invokeMethod(this, 
            [this]()
            {
                processObjectQueue();
            }, 
            Qt::QueuedConnection
        );
    }
}

void SceneHooks::removeQObject(QObject* object)
{
    // Ensure we don't process the object if it
    //was already deleted before processObjectQueue() was called.
    QMutexLocker lock{ &m_mutex };
    m_objectQueue.remove(object);
}

void SceneHooks::processObjectQueue()
{
    QMutexLocker lock{ &m_mutex };
    
    for(auto* object : m_objectQueue)
    {
        //TODO: technically, if on another thread, this QObject can still not
        //be yet fully constructed. Maybe process the queue on the invoking thread?
        auto* item = qobject_cast<QQuickItem*>(object);
        if(item)
        {
            auto sceneListIt = m_subscribers.find(item->window());
            if(sceneListIt != m_subscribers.end())
            {
                auto& sceneList = sceneListIt.value();
                for(auto* scene : sceneList)
                {
                    // 2 reasons this is done:
                    //  - Don't call the callback while holding the mutex(will cause deadlock if the callback creates QObjects)
                    //  - Invoke from the scene thread
                    QMetaObject::invokeMethod(
                        scene, 
                        [scene, item]()
                        {
                            scene->addQQuickItemHook(item);
                        }, 
                        Qt::QueuedConnection
                    );
                }
            }
        }
    }
    m_objectQueue.clear();
}

}