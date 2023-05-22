#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QLocale>
#include <QTranslator>

#include "translationassistant.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    // QTranslator translator;
    // QStringList uiLanguages = QLocale::system().uiLanguages();
    // for (const QString &locale : uiLanguages) {
    //     const QString baseName = "ApplicationUnderTest_" + QLocale(locale).name();
    //     if (translator.load(":/i18n/" + baseName)) {
    //         app.installTranslator(&translator);
    //         break;
    //     }
    // }

    QTranslator translator;
    const auto translationFile = QString{ QCoreApplication::applicationDirPath() + "/ApplicationUnderTest_ro_RO.qm" };
    if(translator.load(translationFile))
    {
        app.installTranslator(&translator);
    }
    else
    {
        qDebug() << "Could not load translations" << translationFile;
        // return 1;
    }

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url,&engine](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
        
        qDebug() << engine.rootObjects();
        auto* window = qobject_cast<QQuickWindow*>(engine.rootObjects()[0]);
        auto* translationAssistant = new ta::TranslationAssistant{ window };
        Q_UNUSED(translationAssistant);

        auto translationLoaded = translationAssistant->addTranslationFile(
            QCoreApplication::applicationDirPath() + "/ApplicationUnderTest_ro_RO.ts"
        );
        if(!translationLoaded)
        {
            qCritical() << "Could not load .ts file";
        }

    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
