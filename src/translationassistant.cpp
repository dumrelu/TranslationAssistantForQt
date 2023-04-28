#include "translationassistant.h"

#include <QDebug>

namespace ta
{

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

    auto it = m_textItemOverlays.find(textItem);
    if(it != m_textItemOverlays.end())
    {
        Q_ASSERT(*it);
        (*it)->setHighlighted(true);
    }
}

}