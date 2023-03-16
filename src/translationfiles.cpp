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

    auto tsNodes = document.elementsByTagName("TS");
    if(tsNodes.isEmpty())
    {
        qWarning() << "No <TS> node";
        return false;
    }

    auto tsNode = tsNodes.at(0);
    if(!tsNode.isElement())
    {
        return false;
    }

    auto contextNodes = tsNode.toElement().elementsByTagName("context");
    for(auto i = 0; i < contextNodes.size(); ++i)
    {
        auto contextNode = contextNodes.at(i);
        if(contextNode.isElement())
        {
            parseContext(contextNode.toElement());
        }
    }

    return true;
}

void TranslationFiles::parseContext(QDomElement contextElement)
{
    auto getTextByTag = [](const QDomElement& message, QString tag)
    {
        auto elements = message.elementsByTagName(tag);
        if(elements.isEmpty())
        {
            return QString{};
        }
        
        return elements.at(0).toElement().text();
    };

    auto messageNodes = contextElement.elementsByTagName("message");
    for(auto i = 0; i < messageNodes.size(); ++i)
    {
        auto messageNode = messageNodes.at(i);
        if(messageNode.isElement())
        {
            auto messageElement = messageNode.toElement();
            qDebug() << "source:" << getTextByTag(messageElement, "source");
            qDebug() << "translation:" << getTextByTag(messageElement, "translation");
        }
    }
}

}