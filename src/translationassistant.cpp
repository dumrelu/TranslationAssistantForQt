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
    auto verifiedTranslations = verifyTranslations(textItem, possibleTranslations);
    qDebug() << "Possible translations: " << possibleTranslations;
    qDebug() << "Verified translations: " << verifiedTranslations;
    

    for(const auto& translationID : verifiedTranslations)
    {
        m_translationFiles.translate(translationID, textItem->text() + "X");
    }
    qDebug() << "Updated text: " << textItem->text();

    auto it = m_textItemOverlays.find(textItem);
    if(it != m_textItemOverlays.end())
    {
        Q_ASSERT(*it);
        (*it)->setHighlighted(true);
    }
}

QList<TranslationFiles::TranslationID> TranslationAssistant::verifyTranslations(const QSharedPointer<TextItem> &textItem, QList<TranslationFiles::TranslationID> translations)
{
    QList<TranslationFiles::TranslationID> verifiedTranslations;
    verifiedTranslations.reserve(translations.size());

    const auto tempTranslationFormat = QString{ "temporaryTranslation_%1ID" };
    for(const auto& translationID : translations)
    {
        auto translationData = m_translationFiles.translationData(translationID);
        Q_ASSERT(translationData);

        const auto translationIDAsString = QString::number(translationID);
        const auto tempTranslation = tempTranslationFormat.arg(translationIDAsString);

        translationData->translation = tempTranslation;

        m_pendingTranslator.addManualTranslation(*translationData);
    }

    // Refresh the UI and check the updated text
    m_pendingTranslator.refreshUi();
    const auto updatedText = textItem->text();
    for(const auto& translationID : translations)
    {
        const auto translationIDAsString = QString::number(translationID);
        const auto tempTranslation = tempTranslationFormat.arg(translationIDAsString);

        if(updatedText.contains(tempTranslation))
        {
            verifiedTranslations.append(translationID);
        }
    }

    // Remove the temporary translations
    m_pendingTranslator.resetTranslations();

    return verifiedTranslations;
}

}