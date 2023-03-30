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
    TranslationFiles* m_translationFiles = nullptr;
    QQmlEngine* m_engine = nullptr;
};

}