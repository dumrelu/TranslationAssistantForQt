#include "scene.h"
#include "scenehooks.h"

namespace qta
{

Scene::Scene(QQuickWindow *window)
    : m_window{ window }
{
}

Scene::~Scene()
{
    stop();
}

void Scene::start()
{
    SceneHooks::instance().subscribe(this);
}

void Scene::stop()
{
}

void Scene::addQQuickItemHook(QQuickItem *item)
{
    qDebug() << "QQuickItem added: " << item;
}

bool Scene::eventFilter(QObject *obj, QEvent *event)
{
    return QObject::eventFilter(obj, event);
}

}