#include "translationassistant.h"

#include <QQmlEngine>
#include <QQmlContext>
#include <QDebug>

namespace ta
{

namespace
{

// Function that returns the QQmlContext for a QQuickItem
QQmlContext* qmlContextForItem(QQuickItem* item)
{
    QQmlContext* context = QQmlEngine::contextForObject(item);
    if(context)
    {
        return context;
    }

    QQuickItem* parent = item->parentItem();
    if(parent)
    {
        return qmlContextForItem(parent);
    }

    return nullptr;
}

}

TranslationAssistant::TranslationAssistant(QQuickWindow *window, QObject *parent)
    : QObject{ parent }
    , m_scene{ window }
{
    Q_ASSERT(window);

    connect(
        &m_scene, &Scene::textItemCreated,
        this, &TranslationAssistant::onTextItemCreated
    );

    m_scene.start();
}

void TranslationAssistant::onTextItemCreated(QSharedPointer<TextItem> textItem)
{
    connect(
        &m_scene, &Scene::textItemInvalidated,
        this, &TranslationAssistant::onTextItemInvalidated
    );

    auto* overlay = new TextItemOverlay{ textItem };
    connect(
        overlay, &TextItemOverlay::textItemClicked,
        this, &TranslationAssistant::onTextItemClicked
    );

    m_textItemOverlays.insert(textItem, overlay);
}

void TranslationAssistant::onTextItemInvalidated(QSharedPointer<TextItem> textItem)
{
    m_textItemOverlays.remove(textItem);
}

void TranslationAssistant::onTextItemClicked(QSharedPointer<TextItem> textItem)
{
    qDebug() << "Clicked on item: " << textItem->text();

    auto* context = qmlContextForItem(textItem->item());
    if(context)
    {
        qDebug() << "Context: " << context->baseUrl();
        return;
    }

    auto it = m_textItemOverlays.find(textItem);
    if(it != m_textItemOverlays.end())
    {
        Q_ASSERT(*it);
        (*it)->setHighlighted(true);
    }
}

}