#pragma once

#include <QTest>
#include <QDebug>
#include <QQmlEngine>

#include "pendingtranslator.h"

class PendingTranslatorTest : public QObject
{
    Q_OBJECT

public:
    PendingTranslatorTest(QString executableDir)
        : m_executableDir{ std::move(executableDir) }
    {
        
    }

private slots:

    void testPendingTranslator()
    {
        int argc = 0;
        char* argv[] = { nullptr };
        QCoreApplication app{ argc, argv };
        QQmlEngine engine;

        // Instantiate a translation TranslationFiles and load simple_file.ts
        ta::TranslationFiles tf;
        QVERIFY(tf.loadTranslationFile(m_executableDir + "/simple_file.ts"));
        
        auto results = tf.findTranslations("Text1", "main");
        QCOMPARE(results.size(), 1);
        QVERIFY(tf.translate(results[0], "Text1_translated"));

        // Instantiate PendingTranslator
        ta::PendingTranslator pt{ &tf, &engine };
        qApp->installTranslator(&pt);

        // Verify translator is used
        QCOMPARE(QGuiApplication::translate("main", "Text1"), "Text1_translated");

        // Change the translation
        QVERIFY(tf.translate(results[0], "Text1_translated2"));

        // Verity translator is updated
        QCOMPARE(QGuiApplication::translate("main", "Text1"), "Text1_translated2");
    }

private:
    QString m_executableDir;
};