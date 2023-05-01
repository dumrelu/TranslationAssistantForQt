#include "translationassistant.h"

#include <QQmlEngine>
#include <QQmlContext>
#include <QDebug>

namespace ta
{

namespace
{

// Returns QQmlContext for a QQuickItem
QQmlContext* qmlContextForItem(const QQuickItem* item)
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

// Extract the qml filename from a QUrl
QString qmlContextFromUrl(const QUrl& url)
{
    QString filename = url.toString();
    //TODO: More usecases in the future
    filename.remove("qrc:/");
    filename.remove(".qml");
    return filename;
}

// Returns the translation context for a TextItem or an empty string if it cannot be determined
QString translationContext(const QSharedPointer<TextItem>& textItem)
{
    auto* context = qmlContextForItem(textItem->item());
    if(context)
    {
        return qmlContextFromUrl(context->baseUrl());
    }

    return {};
}

// Returns the QQmlEngine for the given QQuickWindow
QQmlEngine* qmlEngineForWindow(QQuickWindow* window)
{
    QQmlContext* context = QQmlEngine::contextForObject(window);
    if(context)
    {
        return context->engine();
    }

    return nullptr;
}

}

TranslationAssistant::TranslationAssistant(QQuickWindow *window, QObject *parent)
    : QObject{ parent }
    , m_scene{ window }
    , m_pendingTranslator{ &m_translationFiles, qmlEngineForWindow(window) }
{
    Q_ASSERT(window);
    Q_ASSERT(qApp);

    connect(
        &m_scene, &Scene::textItemCreated,
        this, &TranslationAssistant::onTextItemCreated
    );

    m_scene.start();

    qApp->installTranslator(&m_pendingTranslator);
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

    const auto context = translationContext(textItem);
    qDebug() << "Context for clicked item: " << context;

    auto possibleTranslations = m_translationFiles.findTranslations(textItem->text(), context);
    //TODO: Validate translations(Instantiate a new PendingTranslator with dummy translations)
    qDebug() << "Translations for clicked item: " << possibleTranslations;

    for(const auto& translationID : possibleTranslations)
    {
        m_translationFiles.translate(translationID, textItem->text() + "X");
    }

    auto it = m_textItemOverlays.find(textItem);
    if(it != m_textItemOverlays.end())
    {
        Q_ASSERT(*it);
        (*it)->setHighlighted(true);
    }
}

}