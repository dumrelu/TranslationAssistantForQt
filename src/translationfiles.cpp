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
        parseContext(contextNode);
        contextNode = contextNode.nextSiblingElement("context");
    }

    return true;
}

void TranslationFiles::parseContext(QDomElement contextNode)
{
    auto nameNode = contextNode.firstChildElement("name");
    if(!nameNode.isNull())
    {
        qDebug() << "contextName: " << nameNode.text();
    }

    for(auto messageNode = contextNode.firstChildElement("message"); 
        !messageNode.isNull(); 
        messageNode = messageNode.nextSiblingElement("message")
    )
    {
        auto source = messageNode.firstChildElement("source").text();
        auto translationNode = messageNode.firstChildElement("translation");
        if(translationNode.childNodes().size() > 1)
        {
            //TODO: multiple translations
            qWarning() << "Multiple translations not supported yet";
            continue;
        }
        auto translation = translationNode.text();
        auto translationType = translationNode.attribute("type");

        qDebug() << source << translation << translationType;
    }
}

}