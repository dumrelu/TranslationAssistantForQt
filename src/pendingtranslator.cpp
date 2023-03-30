#include "pendingtranslator.h"

#include <QGuiApplication>
#include <QQmlEngine>
namespace ta
{

PendingTranslator::PendingTranslator(TranslationFiles* tf, QQmlEngine *engine)
    : m_translationFiles{ tf }
    , m_engine{ engine }
{
    Q_ASSERT(tf);
    Q_ASSERT(engine);
    Q_ASSERT(qApp);

    //TODO: connect signals
}

QString PendingTranslator::translationKey(const TranslationFiles::TranslationData &translationData) const
{
    return translationKey(translationData.context, translationData.source, translationData.comment);
}

QString PendingTranslator::translationKey(const QString &context, const QString &source, const QString &comment) const
{
    // TODO: Optimization: maybe use a hash here so we don't store huge strings?
    return context + source + comment;
}

void PendingTranslator::resetTranslations()
{

}

}