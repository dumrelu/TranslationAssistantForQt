#pragma once

#include <QObject>
#include <QDomElement>
#include <QHash>
#include <QSet>

namespace ta
{

class TranslationFiles : public QObject
{
    Q_OBJECT
public:
    using TranslationID = int;
    static constexpr TranslationID INVALID_ID = -1;

    /// @brief Loads the given .ts file
    /// @param tsFilePath A path to an existing .ts file
    /// @return true if file was loaded
    bool loadTranslationFile(QString tsFilePath);

private:
    struct TranslationData
    {
        TranslationID id = INVALID_ID;
        QString context;
        QString tsFilePath;
        QString source;
        QString translation;
        QString translationType;
        bool hasMarkers = false;
    };

    void addTranslation(TranslationData translationData);
    void parseContext(QDomElement contextNode, QString tsFilePath);

    QHash<TranslationID, TranslationData> m_translations;
    
    // Data structures to speed up lookups
    QSet<TranslationID> m_translationsWithMarkers;
    QHash<QString/*context + source*/, TranslationID> m_translationsBySourceText;
    QHash<QString/*context + translation*/, TranslationID> m_translationsByTranslatedText;

    QList<TranslationData> m_pendingChanges;
};

}