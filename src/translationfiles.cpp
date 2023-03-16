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
        if(!contextNode.isElement())
        {
            //TODO: parseContext() but don't change internal state yet?
            qDebug() << contextNode.toElement().tagName();
        }
    }

    return true;
}

bool TranslationFiles::parseContext(QDomElement contextElement)
{
    Q_UNUSED(contextElement);
    return false;
}

}