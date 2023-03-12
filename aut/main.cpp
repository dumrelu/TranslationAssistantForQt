#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QLocale>
#include <QTranslator>

#include "translationassistant.h"

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
        
        auto* window = qobject_cast<QQuickWindow*>(engine.rootObjects()[0]);
        auto* scene = new qta::Scene{ window };
        auto* overlay = new qta::TextItemOverlay(window);
        
        QObject::connect(scene, &qta::Scene::textChanged, [overlay](QSharedPointer<qta::TextItem> textItem)
            {
                const auto text = textItem->text();
                qDebug() << "Text changed: " << text;

                if(text.startsWith("Text") || text == "ListViewText#2_changed")
                {
                    overlay->addOverlayFor(textItem);
                }
            }
        );
        QObject::connect(scene, &qta::Scene::textItemInvalidated, [](QSharedPointer<qta::TextItem> textItem)
            {
                Q_UNUSED(textItem);
                qDebug() << "Text item invalidated";
            }
        );
        
        scene->start();
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
