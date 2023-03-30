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

}