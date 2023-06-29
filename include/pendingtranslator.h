#pragma once

#include "translationfiles.h"

#include <QTranslator>

class QQmlEngine;

namespace ta
{

/// @brief QTranslator implementation to expose the changed
///translations to the applications.
/**
 *  The PendingTranslator will automatically refresh when new changes
 * are made.
*/
class PendingTranslator : public QTranslator
{
    Q_OBJECT
public:
    /// @brief 
    /// @param tf 
    /// @param engine Calls engine->retranslate() on changes
    PendingTranslator(TranslationFiles* tf, QQmlEngine* engine, bool autoRefresh = true);

    // QTranslator interface
    bool isEmpty() const override;
    QString translate(const char *context, const char *sourceText, const char *disambiguation, int n) const override;

    /// @brief Manually adds a translation to the pending translations.
    /// @param translationData
    /**
     *  Note: Manual translations will be lost on the next update.
    */
    void addManualTranslation(const TranslationFiles::TranslationData& translationData);
    
    /// @brief Clears all the pending translations.
    void clearTranslations();

    /// @brief Resets the pending translations to the current values
    ///from TranslationFiles and calls refreshUi().
    /**
     *  Note: this is also called automatically when the TranslationFiles changes.
    */
    void resetTranslations();
    
    /// @brief Refreshes the UI by calling engine->retranslate()
    void refreshUi();

private:
    QString translationKey(const TranslationFiles::TranslationData& translationData) const;
    QString translationKey(const QString& context, const QString& source, const QString& comment) const;
    void onTranslationDataChanged(TranslationFiles::TranslationID id);

    TranslationFiles* m_translationFiles = nullptr;
    QQmlEngine* m_engine = nullptr;
    QHash<QString /*context+source+disambiguation*/, QList<TranslationFiles::TranslationData>> m_translations;
};

}