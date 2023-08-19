#include "translationidentifier.h"

#include <QRegularExpression>

namespace ta
{

RetranslateTranslationIdentifier::RetranslateTranslationIdentifier(TranslationFiles* translationFiles, QQmlEngine *qmlEngine)
    : m_qmlEngine{ qmlEngine }
    , m_translationFiles{ translationFiles }
    , m_temporaryTranslator{ m_translationFiles, m_qmlEngine, false }
{
    Q_ASSERT(m_qmlEngine);
    Q_ASSERT(m_translationFiles);
}

TranslationMap RetranslateTranslationIdentifier::identify(QList<QSharedPointer<TextItem>> textItems, QList<TranslationFiles::TranslationID> translations)
{
    installTemporaryTranslations(translations);
    auto identifiedTranslations = identifyTranslations(textItems);
    removeTemporaryTranslations();
    
    return identifiedTranslations;
}

void RetranslateTranslationIdentifier::installTemporaryTranslations(QList<TranslationFiles::TranslationID> translations)
{
    // Generate placeholder translations containing the translation ID for
    //all the available translations
    for(const auto translationID : translations)
    {
        auto translationData = m_translationFiles->translationData(translationID);
        if(!translationData)
        {
            continue;
        }

        const auto translationIDAsString = QString::number(translationID);
        const auto tempTranslation = m_tempTranslationFormat.arg(translationIDAsString);

        translationData->translation = tempTranslation;

        m_temporaryTranslator.addManualTranslation(*translationData);
    }

    // Refresh the UI to use the placeholder translations
    qApp->installTranslator(&m_temporaryTranslator);
    m_qmlEngine->retranslate();
}

TranslationMap RetranslateTranslationIdentifier::identifyTranslations(QList<QSharedPointer<TextItem>> textItems)
{
    TranslationMap identifiedTranslations;

    // Using the placeholders, identify which translations are used by each item
    QRegularExpression translationIDRegex{ m_tempTranslationFormat.arg("(\\d+)") };
    for(const auto& textItem : textItems)
    {
        //TODO: Possible optimization to check if visible or not(but might break some edge cases)
        const auto text = textItem->text();
        QList<TranslationFiles::TranslationID> translations;
        
        auto matchIt = translationIDRegex.globalMatch(text);
        while(matchIt.hasNext())
        {
            const auto match = matchIt.next();
            const auto idAsStr = match.captured(1);
            const auto id = idAsStr.toInt();

            translations.push_back(static_cast<TranslationFiles::TranslationID>(id));
        }

        identifiedTranslations.insert(textItem, std::move(translations));
    }

    return identifiedTranslations;
}

void RetranslateTranslationIdentifier::removeTemporaryTranslations()
{
    // Remove the placeholder translations
    qApp->removeTranslator(&m_temporaryTranslator);
    m_qmlEngine->retranslate();
}

}