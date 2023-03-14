#include "translationfiles.h"

namespace ta
{

bool TranslationFiles::loadTranslationFile(QString tsFilePath)
{
    if(!tsFilePath.endsWith(".ts"))
    {
        qWarning() << "File" << tsFilePath << "is not a .ts file";
        return false;
    }
    return false;
}

}