#pragma once

#include <QObject>
#include <QDebug>
#include <QDomElement>
#include <QHash>
#include <QSet>

#include <optional>

namespace ta
{

class TranslationFiles : public QObject
{
    Q_OBJECT
public:
    using TranslationID = int;
    static constexpr TranslationID INVALID_ID = -1;

    /// @brief Represents a single entry from a .ts file
    struct TranslationData
    {
        TranslationID id = INVALID_ID;
        QString context;
        QString tsFilePath;
        QString source;
        QString translation;
        QString translationType;
        QString comment;
        bool hasMarkers = false;
        bool isPending = false;
    };

    /// @brief Loads the given .ts file
    /// @param tsFilePath A path to an existing .ts file
    /// @return true if file was loaded
    bool loadTranslationFile(QString tsFilePath);

    /// @brief Finds all the potentially relevant translations for the given input text
    /// @param text Text from the source or translated string
    /// @param context If no context provided, search through all translations
    /// @return Empty list if no matches
    QList<TranslationID> findTranslations(QString text, QString context = {});

    /// @brief Update the translation for the given translation id
    /// @param id 
    /// @param translation 
    /// @return 
    /**
     *  Note: Use the commit() method to write the translations to
     * the .ts file.
    */
    bool translate(TranslationID id, QString translation);

    /// @brief Returns the TranslationData for the given id(if any)
    /// @param id 
    /// @return 
    std::optional<TranslationData> translationData(TranslationID id) const;

    /// @brief Returns a list of all the changed TranslationData
    /// @return 
    QList<TranslationData> pendingTranslations() const;

    /// @brief Print debug information about the current state of the class 
    friend QDebug operator<<(QDebug debug, const TranslationFiles& translationFiles);

signals:
    /// @brief Emitted when any TranslationData changes(e.g. calling translate)
    /// @param id 
    void translationDataChanged(TranslationID id);

private:
    void addTranslation(TranslationData translationData);
    void parseContext(QDomElement contextNode, QString tsFilePath);
    bool isMatch(const TranslationData& translationData, const QString& text, const QString& context) const;
    bool isMatch(const QString& translation, const QString& text) const;
    QStringList splitMarkerString(const QString& markerString) const;

    TranslationID m_translationIDCounter = 0;
    QHash<TranslationID, TranslationData> m_translations;

    // Data structures for optimization
    QHash<QString /*context*/, QList<TranslationID>> m_translationsByContext; 

    // Translations changed by the user that haven't been yet written to the 
    //original .ts file
    QHash<TranslationID, TranslationData> m_pendingTranslations;
};

}