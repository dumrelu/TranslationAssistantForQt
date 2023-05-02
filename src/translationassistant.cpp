#include "translationassistant.h"

#include <QQmlComponent>
#include <QQmlEngine>
#include <QQmlContext>
#include <QDebug>

void initialize_qrc() 
{ 
    Q_INIT_RESOURCE(translation_assistant);
}

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
    //TODO: Fix in the future
    if(filename.startsWith("qrc:/translation_assistant"))
    {
        return {};
    }

    //TODO: More usecases in the future
    filename.remove("qrc:/");
    filename.remove(".qml");
    filename = filename.split("/").last();
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
    , m_window{ window }
    , m_qmlEngine{ qmlEngineForWindow(window) }
    , m_scene{ window }
    , m_pendingTranslator{ &m_translationFiles, m_qmlEngine }
{
    Q_ASSERT(window);
    Q_ASSERT(qApp);

    connect(
        &m_scene, &Scene::textItemCreated,
        this, &TranslationAssistant::onTextItemCreated
    );

    connect(
        &m_scene, &Scene::textItemInvalidated,
        this, &TranslationAssistant::onTextItemInvalidated
    );

    m_scene.start();

    qApp->installTranslator(&m_pendingTranslator);

    createUiOverlay();
}

void TranslationAssistant::onTextItemCreated(QSharedPointer<TextItem> textItem)
{
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
    if(context.isEmpty())
    {
        return;
    }

    //TODO: Update the model as well
    m_possibleTranslations = m_translationFiles.findTranslations(textItem->text(), context);
    m_verifiedTranslations = verifyTranslations(textItem, m_possibleTranslations);
    qDebug() << "Possible translations: " << m_possibleTranslations;
    qDebug() << "Verified translations: " << m_verifiedTranslations;

    updateHighlights(textItem);
}

void TranslationAssistant::onTextChanged(QSharedPointer<TextItem> textItem)
{
    Q_UNUSED(textItem);
    //TODO: Highlight
}

void TranslationAssistant::updateHighlights(const QSharedPointer<TextItem>& selectedTextItem)
{
    for(const auto& overlay : m_textItemOverlays)
    {
        if(overlay->textItem() == selectedTextItem)
        {
            overlay->setHighlightColor(m_selectedTextColor);
            overlay->setHighlighted(true);
        }
        //TODO: This is inneficient. Do this in bulk
        else if(!verifyTranslations(overlay->textItem(), m_verifiedTranslations).isEmpty())
        {
            overlay->setHighlightColor(m_relatedTextColor);
            overlay->setHighlighted(true);
        }
        else 
        {
            overlay->setHighlighted(false);
        }
    }
}

void TranslationAssistant::createUiOverlay()
{
    initialize_qrc();

    QQmlComponent component{ m_qmlEngine, QUrl{ "qrc:/translation_assistant/TranslationAssistant.qml" } };
    if(component.status() != QQmlComponent::Status::Ready)
    {
        qWarning() << "Failed to create TranslationAssistant.qml component" << component.errorString();
        return;
    }
    
    auto* overlay = qobject_cast<QQuickItem*>(component.create());
    if(!overlay)
    {
        qWarning() << "Failed to create TranslationAssistant.qml instance";
        return;
    }

    overlay->setParentItem(m_window->contentItem());
    overlay->setParent(this);
}

QList<TranslationFiles::TranslationID> TranslationAssistant::verifyTranslations(const QSharedPointer<TextItem>& textItem, QList<TranslationFiles::TranslationID> translations)
{
    QList<TranslationFiles::TranslationID> verifiedTranslations;
    verifiedTranslations.reserve(translations.size());

    const auto tempTranslationFormat = QString{ "temporaryTranslation_%1ID" };
    for(const auto& translationID : translations)
    {
        auto translationData = m_translationFiles.translationData(translationID);
        if(!translationData)
        {
            continue;
        }

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