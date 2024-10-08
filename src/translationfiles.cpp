#include "translationfiles.h"

#include <QDebug>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QFile>

#include <algorithm>
#include <cctype>

namespace ta
{

QList<TranslationFiles::TranslationID> TranslationFiles::allTranslationIDs() const
{
    return m_translations.keys();
}

bool TranslationFiles::loadTranslationFile(QString tsFilePath)
{
    if(!tsFilePath.endsWith(".ts"))
    {
        qWarning() << "File" << tsFilePath << "is not a .ts file";
        return false;
    }

    QFile xmlFile{ tsFilePath };
    if(!xmlFile.open(QFile::ReadOnly))
    {
        qWarning() << "Could not open file" << tsFilePath;
        return false;
    }

    QDomDocument document{};
    if(!document.setContent(&xmlFile))
    {
        qWarning() << "Failed to parse" << tsFilePath;
        return false;
    }
    xmlFile.close();

    auto tsNode = document.firstChildElement("TS");
    if(tsNode.isNull())
    {
        qWarning() << "No <TS> node";
        return false;
    }

    auto contextNode = tsNode.firstChildElement("context");
    while(!contextNode.isNull())
    {
        parseContext(contextNode, tsFilePath);
        contextNode = contextNode.nextSiblingElement("context");
    }

    return true;
}

// TODO: Currently using a basic unoptimized implementation. Will need to address in the future
QList<TranslationFiles::TranslationID> TranslationFiles::findTranslations(QString text, QString context)
{
    QList<TranslationID> matches;

    if(text.isEmpty())
    {
        return matches;
    }

    for(const auto& translationData : m_pendingTranslations)
    {
        if(isMatch(translationData, text, context))
        {
            matches.push_back(translationData.id);
        }
    }

    for(const auto& translationData : m_translations)
    {
        if(m_pendingTranslations.contains(translationData.id))
        {
            continue;
        }

        if(isMatch(translationData, text, context))
        {
            matches.push_back(translationData.id);
        }
    }

    return matches;
}

bool TranslationFiles::translate(TranslationID id, QString translation)
{
    auto optTranslationData = translationData(id);
    if(!optTranslationData)
    {
        return false;
    }

    optTranslationData->translation = std::move(translation);
    return updateTranslationData(std::move(*optTranslationData));
}

bool TranslationFiles::updateTranslationData(TranslationData translationData)
{
    const auto id = translationData.id;
    if(!m_translations.contains(id))
    {
        return false;
    }

    // TODO: If pending == current, then don't consider it pending anymore
    translationData.isPending = true;
    m_pendingTranslations.insert(id, std::move(translationData));

    emit translationDataChanged(id);

    return true;
}

std::optional<TranslationFiles::TranslationData> TranslationFiles::translationData(TranslationID id) const
{
    auto pendingIt = m_pendingTranslations.find(id);
    if(pendingIt != m_pendingTranslations.end())
    {
        return *pendingIt;
    }

    auto it = m_translations.find(id);
    if(it != m_translations.end())
    {
        return *it;
    }
    return {};
}

QList<TranslationFiles::TranslationData> TranslationFiles::pendingTranslations() const
{
    return m_pendingTranslations.values();
}

void TranslationFiles::clearPendingTranslations()
{
    auto pendingTranslationIDs = m_pendingTranslations.keys();
    m_pendingTranslations.clear();

    for(const auto& id : pendingTranslationIDs)
    {
        emit translationDataChanged(id);
    }
}

void TranslationFiles::addTranslation(TranslationData translationData)
{
    Q_ASSERT(!translationData.context.isEmpty());
    Q_ASSERT(!translationData.source.isEmpty());

    if(translationData.id == INVALID_ID)
    {
        translationData.id = m_translationIDCounter++;
    }
    else
    {
        //TODO: handle reinsertions(which may be used when commiting pending changes)
        qDebug() << __PRETTY_FUNCTION__ << "TODO: readding the same translation";
    }

    translationData.hasMarkers = translationData.source.contains("%");

    m_translationsByContext[translationData.context].push_back(translationData.id);
    m_translations.insert(translationData.id, std::move(translationData));
}

void TranslationFiles::parseContext(QDomElement contextNode, QString tsFilePath)
{
    QString context = contextNode.firstChildElement("name").text();

    for(auto messageNode = contextNode.firstChildElement("message"); 
        !messageNode.isNull(); 
        messageNode = messageNode.nextSiblingElement("message")
    )
    {
        TranslationData translationData;

        auto translationNode = messageNode.firstChildElement("translation");
        if(translationNode.childNodes().size() > 1)
        {
            //TODO: multiple translations
            qWarning() << "Multiple translations not supported yet";
            continue;
        }

        translationData.context = context;
        translationData.tsFilePath = tsFilePath;
        translationData.source = messageNode.firstChildElement("source").text();
        translationData.translation = translationNode.text();
        translationData.translationType = translationNode.attribute("type");
        translationData.comment = messageNode.firstChildElement("comment").text();

        addTranslation(std::move(translationData));
    }
}

bool TranslationFiles::isMatch(const TranslationData& translationData, const QString& text, const QString& context) const
{
    if(!context.isEmpty() && translationData.context != context)
    {
        return false;
    }

    if(translationData.hasMarkers)
    {
        auto allSubstringsMatch = [&text, this](const QStringList& subStrings)
        {
            if(subStrings.isEmpty())
            {
                return false;
            }

            return std::all_of(
                subStrings.cbegin(), subStrings.cend(), 
                [&text, this](const QString& subString)
                {
                    return isMatch(subString, text);
                }
            );
        };

        return allSubstringsMatch(splitMarkerString(translationData.source))
            || allSubstringsMatch(splitMarkerString(translationData.translation));
    }
    else
    {
        return isMatch(translationData.source, text) || isMatch(translationData.translation, text); 
    }
}

bool TranslationFiles::isMatch(const QString& translation, const QString& text) const
{
    return !translation.isEmpty() && text.contains(translation);
}

QStringList TranslationFiles::splitMarkerString(const QString& markerString) const
{
    auto subStrings = markerString.split("%", Qt::SkipEmptyParts);
    Q_ASSERT(subStrings.size() > 0);

    // Markers are defined via %DD(D is a digit, e.g. %99)
    // Remove the trailing digits after the "%" and the white space
    for(auto& subString : subStrings)
    {
        for(auto i = 0; i < subString.size(); ++i)
        {
            if(!subString[i].isDigit())
            {
                if(i > 0)
                {
                    subString = subString.remove(0, i);
                }

                break;
            }
        }

        subString = subString.trimmed();
    }

    subStrings.removeAll({});

    return subStrings;
}

QDebug operator<<(QDebug debug, const TranslationFiles& translationFiles)
{
    debug << "{";

    debug << "\"translations\": [";
    bool first = true;
    for(const auto id : translationFiles.m_translations.keys())
    {
        if(first)
        {
            debug << "{";
            first = false;
        }
        else
        {
            debug << ",{";
        }

        const auto optTranslationData = translationFiles.translationData(id);
        if(!optTranslationData)
        {
            continue;
        }

        debug << "\"context\":" << optTranslationData->context;
        debug << ",\"tsFilePath\":" << optTranslationData->tsFilePath;
        debug << ",\"source\":" << optTranslationData->source;
        debug << ",\"translation\":" << optTranslationData->translation;
        debug << ",\"translationType\":" << optTranslationData->translationType;
        debug << ",\"comment\":" << optTranslationData->comment;
        debug << ",\"isPending\":" << optTranslationData->isPending;
        debug << "}";
    }
    debug << "]";

    debug << "}";
    return debug;
}

}