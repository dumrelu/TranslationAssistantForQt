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
    : QAbstractListModel{ parent }
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

    //TODO: Proxy models for verified translations, untranslated, etc.
    
    m_verifiedTranslationsModel.setSourceModel(this);
    m_verifiedTranslationsModel.setFilterRole(static_cast<int>(Roles::ID));
}

bool TranslationAssistant::addTranslationFile(const QString &filename)
{
    auto ret = m_translationFiles.loadTranslationFile(filename);

    if(ret)
    {
        // TODO: Optimization: don't rebuild the model every time
        buildModel();
    }

    return ret;
}

Q_INVOKABLE bool TranslationAssistant::translationClicked(QVariant translationIDVariant)
{
    auto translationID = TranslationFiles::INVALID_ID;
    if(translationIDVariant.canConvert<TranslationFiles::TranslationID>())
    {
        translationID = translationIDVariant.value<TranslationFiles::TranslationID>();
    }

    m_verifiedTranslations.clear();

    if(translationID != TranslationFiles::INVALID_ID)
    {
        m_verifiedTranslations.push_back(translationID);
    }

    updateHighlights(nullptr);

    return true;
}

QSortFilterProxyModel *TranslationAssistant::verifiedTranslationsModel()
{
    return &m_verifiedTranslationsModel;
}

QString TranslationAssistant::selectedTranslationText() const
{
    return m_selectedTranslationText;
}

QColor TranslationAssistant::selectedTextColor() const
{
    return m_selectedTextColor;
}

QColor TranslationAssistant::relatedTextColor() const
{
    return m_relatedTextColor;
}

void TranslationAssistant::setSelectedTranslationText(QString selectedTranslationText)
{
    if (m_selectedTranslationText == selectedTranslationText)
    {
        return;
    }

    m_selectedTranslationText = std::move(selectedTranslationText);
    emit selectedTranslationTextChanged();
}

QHash<int, QByteArray> TranslationAssistant::roleNames() const
{
    return {
        { static_cast<int>(Roles::ID), "id" },
        { static_cast<int>(Roles::Source), "source" },
        { static_cast<int>(Roles::Translation), "translation" },
        { static_cast<int>(Roles::Context), "context" },
        { static_cast<int>(Roles::TranslationType), "translationType" }
    };
}

int TranslationAssistant::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return static_cast<int>(m_allTranslations.size());
}

QVariant TranslationAssistant::data(const QModelIndex &index, int role) const
{
    if(!isIndexValid(index))
    {
        return {};
    }

    const auto translationID = m_allTranslations[index.row()];
    const auto optTranslationData = m_translationFiles.translationData(translationID);
    if(!optTranslationData)
    {
        return {};
    }

    switch(static_cast<Roles>(role))
    {
    case Roles::ID:
        return translationID;
    case Roles::Source:
        return optTranslationData->source;
    case Roles::Translation:
        return optTranslationData->translation;
    case Roles::Context:
        return optTranslationData->context;
    case Roles::TranslationType:
        return optTranslationData->translationType;
    }

    return {};
}

bool TranslationAssistant::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!isIndexValid(index))
    {
        return false;
    }

    const auto translationID = m_allTranslations[index.row()];
    auto optTranslationData = m_translationFiles.translationData(translationID);
    if(!optTranslationData)
    {
        return false;
    }

    if(role == static_cast<int>(Roles::Translation))
    {
        const auto translation = value.toString();
        if(translation.isEmpty() || optTranslationData->translation == translation)
        {
            return false;
        }

        return m_translationFiles.translate(translationID, translation);
    }
    else if(role == static_cast<int>(Roles::TranslationType))
    {
        const auto translationType = value.toString();
        if(optTranslationData->translationType == translationType)
        {
            return false;
        }

        optTranslationData->translationType = translationType;
        return m_translationFiles.updateTranslationData(std::move(*optTranslationData));
    }

    return false;
}

void TranslationAssistant::onTextItemCreated(QSharedPointer<TextItem> textItem)
{
    // Ignore items from the translation assistant itself
    if(textItem && textItem->item())
    {
        if(auto* context = qmlContextForItem(textItem->item()); context)
        {
            const auto url = context->baseUrl().toString();
            if(url.startsWith("qrc:/translation_assistant"))
            {
                return;
            }
        }
    }

    auto* overlay = new TextItemOverlay{ textItem };
    connect(
        overlay, &TextItemOverlay::textItemClicked,
        this, &TranslationAssistant::onTextItemClicked
    );
    updateHighlight(overlay, nullptr);

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

    auto possibleTranslations = m_translationFiles.findTranslations(textItem->text(), context);
    m_verifiedTranslations = verifyTranslations(textItem, possibleTranslations);
    qDebug() << "Possible translations: " << possibleTranslations;
    qDebug() << "Verified translations: " << m_verifiedTranslations;

    QString regex = QStringLiteral("^");
    for(int i = 0; i < m_verifiedTranslations.size(); ++i)
    {
        regex += QString::number(m_verifiedTranslations[i]);
        if(i < m_verifiedTranslations.size() - 1)
        {
            regex += "|";
        }
    }
    regex += QStringLiteral("$");
    qDebug() << "Regex: " << regex;

    m_verifiedTranslationsModel.setFilterRegularExpression(regex);

    setSelectedTranslationText(textItem->text());
    updateHighlights(textItem);
}

void TranslationAssistant::onTextChanged(QSharedPointer<TextItem> textItem)
{
    auto it = m_textItemOverlays.find(textItem);
    if(it != m_textItemOverlays.end())
    {
        updateHighlight(it.value(), nullptr);
    }
}

void TranslationAssistant::updateHighlight(TextItemOverlay* overlay, const QSharedPointer<TextItem> &selectedTextItem)
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

void TranslationAssistant::updateHighlights(const QSharedPointer<TextItem> &selectedTextItem)
{
    for(const auto& overlay : m_textItemOverlays)
    {
        updateHighlight(overlay, selectedTextItem);
    }
}

void TranslationAssistant::createUiOverlay()
{
    initialize_qrc();

    // Register this class as a qml singleton
    qmlRegisterSingletonInstance<TranslationAssistant>("TranslationAssistant", 1, 0, "TranslationAssistant", this);

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
    overlay->setZ(1000);

    overlay->setParentItem(m_window->contentItem());
    overlay->setParent(this);
}

void TranslationAssistant::buildModel()
{
    beginResetModel();

    m_allTranslations = m_translationFiles.allTranslationIDs();

    // By default, sort by context
    std::stable_sort(
        m_allTranslations.begin(), m_allTranslations.end(),
        [this](const auto& lhs, const auto& rhs)
        {
            const auto lhsData = m_translationFiles.translationData(lhs);
            const auto rhsData = m_translationFiles.translationData(rhs);
            static const QString emptyContext;
            
            return (lhsData ? lhsData->context : emptyContext) < (rhsData ? rhsData->context : emptyContext);
        }
    );

    endResetModel();
}

bool TranslationAssistant::isIndexValid(const QModelIndex &index) const
{
    return index.isValid() &&
        index.row() >= 0 &&
        index.row() < static_cast<int>(m_allTranslations.size());
}

QList<TranslationFiles::TranslationID> TranslationAssistant::verifyTranslations(const QSharedPointer<TextItem>& textItem, QList<TranslationFiles::TranslationID> translations)
{
    const auto text = textItem->text();
    if(text.isEmpty())
    {
        return {};
    }

    auto possibleTranslations = m_translationFiles.findTranslations(text, translationContext(textItem));
    possibleTranslations.erase(
        std::remove_if(
            possibleTranslations.begin(), possibleTranslations.end(),
            [&translations](const auto& translationID)
            {
                return !translations.contains(translationID);
            }
        ),
        possibleTranslations.end()
    );

    if(possibleTranslations.empty())
    {
        return {};
    }

    QList<TranslationFiles::TranslationID> verifiedTranslations;
    verifiedTranslations.reserve(possibleTranslations.size());

    const auto tempTranslationFormat = QString{ "temporaryTranslation_%1ID" };
    for(const auto& translationID : possibleTranslations)
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
    for(const auto& translationID : possibleTranslations)
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