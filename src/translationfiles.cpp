#include "translationfiles.h"

#include <QDebug>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QFile>

namespace ta
{

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
    Q_UNUSED(text);
    Q_UNUSED(context);

    //TODO: check pending translations first?

    return {};
}

std::optional<TranslationFiles::TranslationData> TranslationFiles::translationData(TranslationID id) const
{
    auto it = m_translations.find(id);
    if(it != m_translations.end())
    {
        return *it;
    }
    return {};
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

QString TranslationFiles::translationBySourceTextKey(const TranslationData& translationData) const
{
    return translationData.context + translationData.source;
}

QString TranslationFiles::translationByTranslatedTextKey(const TranslationData& translationData) const
{
    return translationData.context + translationData.translation;
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

        addTranslation(std::move(translationData));
    }
}

QDebug operator<<(QDebug debug, const TranslationFiles& translationFiles)
{
    debug << "{";

    debug << "\"translations\": [";
    bool first = true;
    for(const auto& translationData : translationFiles.m_translations)
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
        debug << "\"context\":" << translationData.context;
        debug << ",\"tsFilePath\":" << translationData.tsFilePath;
        debug << ",\"source\":" << translationData.source;
        debug << ",\"translation\":" << translationData.translation;
        debug << ",\"translationType\":" << translationData.translationType;
        debug << "}";
    }
    debug << "]";

    debug << "}";
    return debug;
}

}