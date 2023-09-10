#pragma once

#include <QHash>
#include <QQmlEngine>

#include "translationfiles.h"
#include "textitem.h"
#include "pendingtranslator.h"

namespace ta
{

using TranslationMap = QHash<QSharedPointer<TextItem>, QList<TranslationFiles::TranslationID>>;

/// @brief Interface for different methods of identifying 
///what translations(from the given list) are used by each text item(from the given list).
class TranslationIdentifier
{
public:
    virtual ~TranslationIdentifier() = default;
    virtual TranslationMap identify(QList<QSharedPointer<TextItem>> textItems, QList<TranslationFiles::TranslationID> translations) = 0;
};


/// @brief A strategy that identifies translations by using the retranslate function of the qml engine
/**
 *  1. Create a temporary translator that translates all the available translations to a 
 * string containing the translationID(e.g. "myString" will get translated to "translationID_100")
 *  2. Install the translator and retranslate the UI(my calling qmlEngine->retranslate())
 *  3. Iterate through all TextItems and see if they contain any translationIDs, as defined at step #1.
 *  4. Uninstall the temporary translator and retranslate the UI to get back to normal
*/
class RetranslateTranslationIdentifier : public TranslationIdentifier
{
public:
    RetranslateTranslationIdentifier(TranslationFiles* translationFiles, QQmlEngine* qmlEngine);

    TranslationMap identify(QList<QSharedPointer<TextItem>> textItems, QList<TranslationFiles::TranslationID> translations) override;

protected:
    void installTemporaryTranslations(QList<TranslationFiles::TranslationID> translations);
    TranslationMap identifyTranslations(QList<QSharedPointer<TextItem>> textItems);
    void removeTemporaryTranslations();

private:
    QQmlEngine* m_qmlEngine = nullptr;
    QString m_tempTranslationFormat = QStringLiteral("i_%1_d");

    TranslationFiles* m_translationFiles = nullptr;
    //TODO: Create a different translator, don't make PendingTranslator do 2 things
    PendingTranslator m_temporaryTranslator;
};

}