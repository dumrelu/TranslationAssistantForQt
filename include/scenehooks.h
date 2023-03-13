#pragma once

#include <QMutex>
#include <QHash>
#include <QTimer>

#include "scene.h"

namespace ta
{

class SceneHooks : public QObject
{
public:
    /// @brief Returns the unique SceneHooks instance 
    static SceneHooks& instance();

    /// @brief Scene will receive notifications for all 
    ///newly created QQuickItems from their window
    /// @param scene 
    void subscribe(Scene* scene);

    /// @brief Unsubscribe from QQuickItem creation notifications
    /// @param scene 
    void unsubscribe(Scene* scene);

private:
    SceneHooks() = default;
    ~SceneHooks();
    SceneHooks(const SceneHooks&) = delete;
    SceneHooks(SceneHooks&&) = delete;
    SceneHooks& operator=(const SceneHooks&) = delete;
    SceneHooks& operator=(SceneHooks&&) = delete;

    static void installHooks();
    static void uninstallHooks();
    static void addQObjectHook(QObject* object);
    static void removeQObjectHook(QObject* object);

    void addQObject(QObject* object);
    void removeQObject(QObject* object);
    void processObjectQueue();

    QMutex m_mutex;
    bool m_hooksInstalled = false;
    QHash<QQuickWindow*, QList<Scene*>> m_subscribers;
    QSet<QObject*> m_objectQueue;
};

}