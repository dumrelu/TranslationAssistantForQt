#pragma once

#include <QObject>
#include <QDomDocument>

namespace ta
{

class TranslationFiles : public QObject
{
    Q_OBJECT
public:

    /// @brief Loads the given .ts file
    /// @param tsFilePath A path to an existing .ts file
    /// @return true if file was loaded
    bool loadTranslationFile(QString tsFilePath);

private:
};

}