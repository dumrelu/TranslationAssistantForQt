#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QLocale>
#include <QTranslator>

#include "qtranslationassistant.h"

int main(int argc, char *argv[])
{
    qta::hello_world();

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "ApplicationUnderTest_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            app.installTranslator(&translator);
            break;
        }
    }

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url,&engine](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
        
        auto* scene = new qta::Scene{ qobject_cast<QQuickWindow*>(engine.rootObjects()[0]) };
        
        QObject::connect(scene, &qta::Scene::textChanged, [](qta::TextItem* textItem)
            {
                qDebug() << "Text changed: " << textItem->item();
            }
        );
        
        scene->start();
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
