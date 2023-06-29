#include "pendingtranslator.h"

#include <QGuiApplication>
#include <QQmlEngine>
namespace ta
{

PendingTranslator::PendingTranslator(TranslationFiles* tf, QQmlEngine *engine, bool autoRefresh)
    : m_translationFiles{ tf }
    , m_engine{ engine }
{
    Q_ASSERT(tf);
    Q_ASSERT(engine);
    Q_ASSERT(qApp);

    if(autoRefresh)
    {
        connect(
            m_translationFiles, &TranslationFiles::translationDataChanged, 
            this, &PendingTranslator::onTranslationDataChanged  
        );

        resetTranslations();
    }
}

bool PendingTranslator::isEmpty() const
{
    return m_translations.isEmpty();
}

QString PendingTranslator::translate(const char *context, const char *sourceText, const char *disambiguation, int n) const
{
    if(n != -1)
    {
        // TODO: support for n != 1
        return sourceText;
    }
    
    const QString qContext = context;
    const QString qSource = sourceText;
    const QString qComment = disambiguation;

    auto translationIt = m_translations.find(translationKey(qContext, qSource, qComment));
    if(translationIt != m_translations.end())
    {
        const auto& translationDataList = *translationIt;
        for(const auto& translationData : translationDataList)
        {
            if(translationData.context == qContext && translationData.source == qSource
                && translationData.comment == qComment)
            {
                return translationData.translation;
            }
        }
    }

    return {};
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

void PendingTranslator::addManualTranslation(const TranslationFiles::TranslationData &translationData)
{
    const auto key = translationKey(translationData);
    m_translations[key].clear();
    m_translations[key].push_back(translationData);
}

void PendingTranslator::clearTranslations()
{
    m_translations.clear();
}

void PendingTranslator::resetTranslations()
{
    m_translations.clear();

    for(auto&& translationData : m_translationFiles->pendingTranslations())
    {
        m_translations[translationKey(translationData)].push_back(std::move(translationData));
    }

    refreshUi();
}

void PendingTranslator::refreshUi()
{
    if(qApp->removeTranslator(this))
    {
        qApp->installTranslator(this);
        m_engine->retranslate();
    }
}

void PendingTranslator::onTranslationDataChanged(TranslationFiles::TranslationID id)
{
    // TODO: Optimization: don't reset everything
    Q_UNUSED(id);
    resetTranslations();
}

}