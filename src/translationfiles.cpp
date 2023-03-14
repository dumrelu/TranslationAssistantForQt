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

    return true;
}

}