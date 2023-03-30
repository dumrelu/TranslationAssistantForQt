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
    explicit PendingTranslator(TranslationFiles* tf, QQmlEngine* engine);

    // QTranslator interface
    bool isEmpty() const override;
    QString translate(const char *context, const char *sourceText, const char *disambiguation, int n) const override;

private:
    QString translationKey(const TranslationFiles::TranslationData& translationData) const;
    QString translationKey(const QString& context, const QString& source, const QString& comment) const;
    void resetTranslations();
    void refreshUi();
    void onTranslationDataChanged(TranslationFiles::TranslationID id);

    TranslationFiles* m_translationFiles = nullptr;
    QQmlEngine* m_engine = nullptr;
    QHash<QString /*context+source+disambiguation*/, QList<TranslationFiles::TranslationData>> m_translations;
};

}