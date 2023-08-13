#include "translationassistant.h"

#include <QQmlComponent>
#include <QQmlEngine>
#include <QQmlContext>
#include <QDebug>
#include <QRegularExpression>


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
    , m_scene{ window }
{
    Q_ASSERT(window);
    Q_ASSERT(qApp);
    
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

    m_relevantTranslations.clear();
    if(translationID != TranslationFiles::INVALID_ID)
    {
        m_relevantTranslations.append(translationID);
    }

    auto translationMap = identifyTranslations();
    highlightRelevantTranslations(translationMap);
}

Q_INVOKABLE void TranslationAssistant::clearHighlights()
{
    for(const auto& textItemOverlay : m_textItemOverlays)
    {
        textItemOverlay->setHighlighted(false);
    }
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

    if(ret)
    {
        emit dataChanged(index, index);
    }

    return ret;
}

void TranslationAssistant::onTextItemCreated(QSharedPointer<TextItem> textItem)
{
    Q_ASSERT(textItem);

    if(isTranslationAssistantTextItem(textItem))
    {
        qDebug() << "Ignoring TranslationAssistant text item:" << textItem->text();
        return;
    }
    qDebug() << "Text item created:" << textItem->text();

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

TranslationAssistant::TranslationMap TranslationAssistant::identifyTranslations()
{
    TranslationMap translationsForTextItems;

    //TODO: Create a TemporaryTranslator instead of using the PendingTranslator
    PendingTranslator temporaryTranslator{ &m_translationFiles, m_qmlEngine, false };
    const QString tempTranslationFormat = QStringLiteral("i_%1_d");

    // Generate placeholder translations containing the translation ID for
    //all the available translations
    for(const auto translationID : m_allTranslations)
    {
        auto translationData = m_translationFiles.translationData(translationID);
        if(!translationData)
        {
            continue;
        }

        const auto translationIDAsString = QString::number(translationID);
        const auto tempTranslation = tempTranslationFormat.arg(translationIDAsString);

        translationData->translation = tempTranslation;

        temporaryTranslator.addManualTranslation(*translationData);
    }

    // Refresh the UI to use the placeholder translations
    qApp->installTranslator(&temporaryTranslator);
    m_qmlEngine->retranslate();

    // Using the placeholders, identify which translations are used by each item
    QRegularExpression translationIDRegex{ tempTranslationFormat.arg("(\\d+)") };
    for(const auto& textItem : m_textItemOverlays.keys())
    {
        const auto text = textItem->text();
        QList<TranslationFiles::TranslationID> translations;
        
        auto matchIt = translationIDRegex.globalMatch(text);
        while(matchIt.hasNext())
        {
            const auto match = matchIt.next();
            const auto idAsStr = match.captured(1);
            const auto id = idAsStr.toInt();

            translations.push_back(static_cast<TranslationFiles::TranslationID>(id));
        }

        translationsForTextItems.insert(textItem, std::move(translations));
    }

    // Remove the placeholder translations
    qApp->removeTranslator(&temporaryTranslator);
    m_qmlEngine->retranslate();

    return translationsForTextItems;
}

void TranslationAssistant::highlightRelevantTranslations(const TranslationMap &translationMap)
{
    QSet<TranslationFiles::TranslationID> relevantTranslations{ m_relevantTranslations.begin(), m_relevantTranslations.end() };
    
    for(auto it = m_textItemOverlays.begin(); it != m_textItemOverlays.end(); ++it)
    {
        const auto& textItem = it.key();
        const auto& overlay = it.value();

        const auto translations = translationMap[textItem];
        //TODO: probably not efficient to convert to a set every time
        QSet<TranslationFiles::TranslationID> translationsSet{ translations.begin(), translations.end() };
        const bool isRelevant = !translations.isEmpty() && translationsSet.intersect(relevantTranslations).size() > 0;

        overlay->setHighlightColor(m_relevantTextColor);
        overlay->setHighlighted(isRelevant);
    }
}

}