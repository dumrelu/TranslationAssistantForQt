#pragma once

#include <QObject>
#include <QDebug>
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

    /// @brief Print debug information about the current state of the class 
    friend QDebug operator<<(QDebug debug, const TranslationFiles& translationFiles);

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
    QString translationBySourceTextKey(const TranslationData& translationData) const;
    QString translationByTranslatedTextKey(const TranslationData& translationData) const;
    void parseContext(QDomElement contextNode, QString tsFilePath);

    TranslationID m_translationIDCounter = 0;
    QHash<TranslationID, TranslationData> m_translations;
    
    // Data structures to speed up lookups
    QSet<TranslationID> m_translationsWithMarkers;
    QHash<QString/*context + source*/, TranslationID> m_translationsBySourceText;
    QHash<QString/*context + translation*/, TranslationID> m_translationsByTranslatedText;

    // Translations changed by the user that haven't been yet written to the 
    //original .ts file
    QList<TranslationData> m_pendingChanges;
};

}