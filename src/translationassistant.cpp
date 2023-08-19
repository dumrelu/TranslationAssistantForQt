#include "translationassistant.h"

#include <QQmlComponent>
#include <QQmlEngine>
#include <QQmlContext>
#include <QDebug>


void initialize_qrc() 
{ 
    Q_INIT_RESOURCE(translation_assistant);
}

namespace
{

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

namespace ta
{

TranslationAssistant::TranslationAssistant(QQuickWindow *window, QObject *parent)
    : QAbstractListModel{ parent }
    , m_window{ window }
    , m_qmlEngine{ qmlEngineForWindow(window) }
    , m_pendingTranslator{ &m_translationFiles, m_qmlEngine }
    , m_translationIdentifier{ std::make_unique<RetranslateTranslationIdentifier>(&m_translationFiles, m_qmlEngine) }
    , m_scene{ window }
{
    Q_ASSERT(window);
    Q_ASSERT(qApp);

    connect(
        &m_translationFiles, &TranslationFiles::translationDataChanged, 
        [this](TranslationFiles::TranslationID id)
        {
            auto it = m_allTranslationsIndices.find(id);
            if(it != m_allTranslationsIndices.end())
            {
                const auto index = it.value();
                emit dataChanged(createIndex(index, 0), createIndex(index, 0));
            }
        }
    );
    
    createUiOverlay();

    connect(
        &m_scene, &Scene::textItemCreated,
        this, &TranslationAssistant::onTextItemCreated
    );
    connect(
        &m_scene, &Scene::textItemInvalidated,
        this, &TranslationAssistant::onTextItemInvalidated
    );
    connect(
        &m_scene, &Scene::textChanged,
        this, &TranslationAssistant::onTextChanged
    );
    m_scene.start();

    qApp->installTranslator(&m_pendingTranslator);

    m_relevantTranslationsModel.setSourceModel(this);
    m_relevantTranslationsModel.setFilterRole(static_cast<int>(Roles::ID));

    m_pendingTranslationsModel.setSourceModel(this);
    m_pendingTranslationsModel.setFilterRole(static_cast<int>(Roles::IsPending));
    m_pendingTranslationsModel.setFilterFixedString("true");
}

bool TranslationAssistant::addTranslationSources(const QStringList &tsFileNames)
{
    auto ret = true;
    for(const auto& tsFileName : tsFileNames)
    {
        ret &= m_translationFiles.loadTranslationFile(tsFileName);
    }

    rebuildModel();

    return ret;
}

void TranslationAssistant::highlightTranslation(QVariant translationIDVariant)
{
    auto translationID = TranslationFiles::INVALID_ID;
    if(translationIDVariant.canConvert<TranslationFiles::TranslationID>())
    {
        translationID = translationIDVariant.value<TranslationFiles::TranslationID>();
    }

    if(translationID == TranslationFiles::INVALID_ID)
    {
        return;
    }

    auto translationMap = identifyTranslations();
    highlightTranslation(translationMap, translationID);
}

void TranslationAssistant::clearHighlights()
{
    for(const auto& textItemOverlay : m_textItemOverlays)
    {
        textItemOverlay->setHighlighted(false);
    }
}

void TranslationAssistant::clearRelevantTranslations()
{
    m_relevantTranslations.clear();
    m_relevantTranslationsModel.setFilterRegularExpression("");

    clearHighlights();
}

void TranslationAssistant::clearPendingTranslations()
{
    m_translationFiles.clearPendingTranslations();
}

void TranslationAssistant::setTranslationIdentifier(std::unique_ptr<TranslationIdentifier> translationIdentifier)
{
    Q_ASSERT(translationIdentifier);
    m_translationIdentifier = std::move(translationIdentifier);
}

std::unique_ptr<TranslationIdentifier> TranslationAssistant::removeTranslationIdentifier()
{
    return std::move(m_translationIdentifier);
}

QSortFilterProxyModel *TranslationAssistant::relevantTranslationsModel()
{
    return &m_relevantTranslationsModel;
}

QSortFilterProxyModel *TranslationAssistant::pendingTranslationsModel()
{
    return &m_pendingTranslationsModel;
}

QColor TranslationAssistant::selectedTextColor() const
{
    return m_selectedTextColor;
}

QColor TranslationAssistant::relevantTextColor() const
{
    return m_relevantTextColor;
}

QHash<int, QByteArray> TranslationAssistant::roleNames() const
{
    return {
        { ID, "id" },
        { Source, "source" },
        { Translation, "translation" },
        { Context, "context" },
        { Comment, "comment" },
        { IsFinished, "isFinished" },
        { IsPending, "isPending" },
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
    case Roles::Comment:
        return optTranslationData->comment;
    case Roles::IsFinished:
        return optTranslationData->translationType == "";
    case Roles::IsPending:
        return optTranslationData->isPending;
    }

    qWarning() << "Role not handled in TranslationAssistant::data()" << role;
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

    auto ret = false;
    if(role == static_cast<int>(Roles::Translation))
    {
        const auto translation = value.toString();
        if(translation.isEmpty() || optTranslationData->translation == translation)
        {
            return false;
        }

        ret = m_translationFiles.translate(translationID, translation);
    }
    else if(role == static_cast<int>(Roles::IsFinished))
    {
        const auto isFinished = value.toBool();
        const QString newTranslationType = isFinished ? QStringLiteral("") : QStringLiteral("unfinished");
        if(optTranslationData->translationType == newTranslationType)
        {
            return false;
        }

        optTranslationData->translationType = newTranslationType;
        ret = m_translationFiles.updateTranslationData(std::move(*optTranslationData));
    }

    return ret;
}

void TranslationAssistant::onTextItemCreated(QSharedPointer<TextItem> textItem)
{
    Q_ASSERT(textItem);

    if(isTranslationAssistantTextItem(textItem))
    {
        return;
    }

    auto* overlay = new TextItemOverlay{ textItem };
    connect(
        overlay, &TextItemOverlay::textItemClicked,
        this, &TranslationAssistant::onTextItemClicked
    );

    m_textItemOverlays.insert(textItem, overlay);
}

void TranslationAssistant::onTextItemInvalidated(QSharedPointer<TextItem> textItem)
{
    Q_ASSERT(textItem);

    m_textItemOverlays.remove(textItem);
}

void TranslationAssistant::onTextItemClicked(QSharedPointer<TextItem> textItem)
{
    Q_ASSERT(textItem);

    qDebug() << "Text item clicked: " << textItem->text();
    
    m_relevantTranslations.clear();

    // Identify and highlight the relevant translations
    auto translationMap = identifyTranslations();
    if(auto it = translationMap.find(textItem); it != translationMap.end())
    {
        m_relevantTranslations = std::move(it.value());
        highlightRelevantTranslations(translationMap);
    }

    // Highlight the selected text item
    if(auto it = m_textItemOverlays.find(textItem); it != m_textItemOverlays.end())
    {
        it.value()->setHighlightColor(m_selectedTextColor);
        it.value()->setHighlighted(true);
    }

    // Update the regex for the relevantTranslationsModel
    QString regex = QStringLiteral("^");
    for(int i = 0; i < m_relevantTranslations.size(); ++i)
    {
        regex += QString::number(m_relevantTranslations[i]);
        if(i < m_relevantTranslations.size() - 1)
        {
            regex += "|";
        }
    }
    regex += QStringLiteral("$");
    m_relevantTranslationsModel.setFilterRegularExpression(regex);
}

void TranslationAssistant::onTextChanged(QSharedPointer<TextItem> textItem)
{
    Q_ASSERT(textItem);

    // If it doesn't have an overlay, it means we should ignore this
    //text item because it's part of the TranslationAssistant UI
    auto it = m_textItemOverlays.find(textItem);
    if(it == m_textItemOverlays.end())
    {
        return;
    }

    qDebug() << "Text changed: " << textItem->text();
}

void TranslationAssistant::rebuildModel()
{
    beginResetModel();

    m_allTranslations = m_translationFiles.allTranslationIDs();
    m_relevantTranslations.clear();

    // By default, sort alphabetically by context and then source text
    std::stable_sort(
        m_allTranslations.begin(), m_allTranslations.end(), 
        [this](const auto& lhs, const auto& rhs) 
        {
            const auto lhsData = m_translationFiles.translationData(lhs);
            const auto rhsData = m_translationFiles.translationData(rhs);
            
            //TODO: handle null data
            Q_ASSERT(lhsData);
            Q_ASSERT(rhsData);
            
            std::pair<const QString&, const QString&> lhsKey{ lhsData->context, lhsData->source };
            std::pair<const QString&, const QString&> rhsKey{ rhsData->context, rhsData->source };
            return lhsKey < rhsKey;
        }
    );

    // Update the indices map used for fast translation id to index lookups
    m_allTranslationsIndices.clear();
    for(auto i = 0; i < m_allTranslations.size(); ++i)
    {
        m_allTranslationsIndices.insert(m_allTranslations[i], i);
    }

    endResetModel();
}

bool TranslationAssistant::isIndexValid(const QModelIndex &index) const
{
    return index.isValid() &&
        index.row() >= 0 &&
        index.row() < static_cast<int>(m_allTranslations.size());
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
    overlay->setZ(std::numeric_limits<qreal>::max());

    overlay->setParentItem(m_window->contentItem());
    overlay->setParent(this);
}

bool TranslationAssistant::isTranslationAssistantTextItem(const QSharedPointer<TextItem> &textItem) const
{
    if(!textItem)
    {
        return false;
    }

    // Navigate up the parentItem chain and try to find if this
    //textItem is a child of the TranslationAssistant overlay. We identify
    //the overlay by checking if its QObject::parent is the 
    //TranslationAssistant instance.
    auto* item = textItem->item();
    while(item)
    {
        if(item->parent() == this)
        {
            return true;
        }
        item = item->parentItem();
    }

    return false;
}

TranslationMap TranslationAssistant::identifyTranslations()
{
    if(!m_translationIdentifier)
    {
        qWarning() << "No translation identifier set. Can't identify translations";
        return {};
    }

    return m_translationIdentifier->identify(m_textItemOverlays.keys(), m_allTranslations);
}

template <typename Predicate>
void TranslationAssistant::highlightTranslations(const TranslationMap &translationMap, const QColor& color, Predicate condition)
{
    for(auto it = m_textItemOverlays.begin(); it != m_textItemOverlays.end(); ++it)
    {
        const auto& textItem = it.key();
        const auto& overlay = it.value();

        const auto translations = translationMap[textItem];
        const bool isRelevant = condition(translations);

        overlay->setHighlightColor(color);
        overlay->setHighlighted(isRelevant);
    }
}

void TranslationAssistant::highlightTranslation(const TranslationMap &translationMap, const TranslationFiles::TranslationID &translationID)
{
    highlightTranslations(translationMap, m_selectedTextColor, 
        [&translationID](const QList<TranslationFiles::TranslationID>& translations)
        {
            return translations.contains(translationID);
        }
    );
}

void TranslationAssistant::highlightRelevantTranslations(const TranslationMap &translationMap)
{
    QSet<TranslationFiles::TranslationID> relevantTranslations{ m_relevantTranslations.begin(), m_relevantTranslations.end() };
    highlightTranslations(translationMap, m_relevantTextColor, 
        [&relevantTranslations](const QList<TranslationFiles::TranslationID>& translations)
        {
            if(translations.isEmpty())
            {
                return false;
            }

            //TODO: probably not efficient to convert to a set every time
            QSet<TranslationFiles::TranslationID> translationsSet{ translations.begin(), translations.end() };
            return translationsSet.intersect(relevantTranslations).size() > 0;
        }
    );
}

}