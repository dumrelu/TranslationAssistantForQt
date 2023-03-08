#include "scenehooks.h"

#include <iostream>

#include <QMutexLocker>
#include <private/qhooks_p.h>

namespace qta
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

    if(!m_subscribers.contains(scene))
    {
        m_subscribers.insert(scene);
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

    //TODO: uninstall hooks if no more subscribers

    m_subscribers.remove(scene);
}

void SceneHooks::addQObject(QObject* object)
{
    Q_UNUSED(object);
    std::clog << __PRETTY_FUNCTION__ << std::endl;
}

void SceneHooks::removeQObject(QObject* object)
{
    Q_UNUSED(object);
    std::clog << __PRETTY_FUNCTION__ << std::endl;
}

}