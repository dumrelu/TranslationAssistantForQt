#pragma once 

#include <QTest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

#include "translationfiles.h"

class TranslationFilesTest : public QObject
{
    Q_OBJECT
public:
    TranslationFilesTest(QString executableDir)
        : m_executableDir{ std::move(executableDir) }
    {
        qDebug() << "Executable dir" << m_executableDir;
    }

private slots:
    void initTestCase()
    {
        QVERIFY(QFile::exists(m_executableDir + "/simple_file.ts"));
        QVERIFY(QFile::exists(m_executableDir + "/simple_file_bad_extention.notts"));
        QVERIFY(QFile::exists(m_executableDir + "/bad_xml.ts"));
        QVERIFY(QFile::exists(m_executableDir + "/find_test.ts"));
    }

    void testBadPath()
    {
        ta::TranslationFiles tf;
        QVERIFY(tf.loadTranslationFile("/invalid/to/invalid/file.ts") == false);
    }

    void testFileWithNoTsExtention()
    {
        ta::TranslationFiles tf;
        QVERIFY(tf.loadTranslationFile(m_executableDir + "/simple_file_bad_extention.notts") == false);
    }

    void testBadXml()
    {
        ta::TranslationFiles tf;
        QVERIFY(tf.loadTranslationFile(m_executableDir + "/bad_xml.ts") == false);
    }

    void testSimpleXml()
    {
        ta::TranslationFiles tf;
        QVERIFY(tf.loadTranslationFile(m_executableDir + "/simple_file.ts"));
        
        auto state = getState(tf);
        QVERIFY(state.contains("translations"));
        
        auto translations = state["translations"].toArray();
        // Should be 5 when supporting multiple translations
        QCOMPARE(translations.size(), 4);

        auto obj1 = findTranslation(translations, "MyQmlType", "Text2");
        QVERIFY(!obj1.isEmpty());
        QCOMPARE(obj1["tsFilePath"].toString(), m_executableDir + "/simple_file.ts");
        QCOMPARE(obj1["translation"].toString(), "Text2_translated");
        QCOMPARE(obj1["translationType"].toString(), "");

        auto obj2 = findTranslation(translations, "MyQmlType", "Text2_changed");
        QVERIFY(!obj2.isEmpty());
        QCOMPARE(obj2["tsFilePath"].toString(), m_executableDir + "/simple_file.ts");
        QCOMPARE(obj2["translation"].toString(), "Text2_changed_translated");
        QCOMPARE(obj2["translationType"].toString(), "unfinished");

        auto obj3 = findTranslation(translations, "main", "Text1");
        QVERIFY(!obj3.isEmpty());
        QCOMPARE(obj3["tsFilePath"].toString(), m_executableDir + "/simple_file.ts");
        QCOMPARE(obj3["translation"].toString(), "");
        QCOMPARE(obj3["translationType"].toString(), "unfinished");

        auto obj4 = findTranslation(translations, "main", "Text4");
        QVERIFY(!obj4.isEmpty());
        QCOMPARE(obj4["tsFilePath"].toString(), m_executableDir + "/simple_file.ts");
        QCOMPARE(obj4["translation"].toString(), "");
        QCOMPARE(obj4["translationType"].toString(), "unfinished");
    }

    void testTranslationData()
    {
        ta::TranslationFiles tf;
        QVERIFY(tf.loadTranslationFile(m_executableDir + "/simple_file.ts"));

        auto validTranslationData = tf.translationData(0);
        QVERIFY(validTranslationData);
        QCOMPARE(validTranslationData->source, "Text2");

        auto invalidTranslationData = tf.translationData(999);
        QVERIFY(!invalidTranslationData);
    }

    // TODO: partial matches test, markers test
    void testFindTranslationsSimple()
    {
        ta::TranslationFiles tf;
        QVERIFY(tf.loadTranslationFile(m_executableDir + "/find_test.ts"));

        // Find by source text
        auto matchesBySource = tf.findTranslations("FirstSimpleText", "Simple");
        QCOMPARE(matchesBySource.size(), 1);
        QCOMPARE(tf.translationData(matchesBySource[0])->source, "FirstSimpleText");

        // Find by translated text
        auto matchesByTranslation = tf.findTranslations("SecondSimpleTranslation", "Simple");
        QCOMPARE(matchesByTranslation.size(), 1);
        QCOMPARE(tf.translationData(matchesByTranslation[0])->translation, "SecondSimpleTranslation");
    }

    void testDifferentContext()
    {
        ta::TranslationFiles tf;
        QVERIFY(tf.loadTranslationFile(m_executableDir + "/find_test.ts"));

        auto results = tf.findTranslations("FirstSimpleText", "Simple");
        QCOMPARE(results.size(), 1);
        QCOMPARE("FirstSimpleText", tf.translationData(results[0])->source);
        QCOMPARE("FirstSimpleTranslation", tf.translationData(results[0])->translation);
        QCOMPARE("Simple", tf.translationData(results[0])->context);

        results = tf.findTranslations("FirstSimpleText", "Simple2");
        QCOMPARE(results.size(), 1);
        QCOMPARE("FirstSimpleText", tf.translationData(results[0])->source);
        QCOMPARE("Simple2ContextFirstSimpleTextTranslation", tf.translationData(results[0])->translation);
        QCOMPARE("Simple2", tf.translationData(results[0])->context);

        results = tf.findTranslations("FirstSimpleText");
        QCOMPARE(results.size(), 2);
        QVERIFY(tf.translationData(results[0])->context == "Simple" || tf.translationData(results[0])->context == "Simple2");
        QVERIFY(tf.translationData(results[1])->context == "Simple" || tf.translationData(results[1])->context == "Simple2");
        QVERIFY(tf.translationData(results[0])->context != tf.translationData(results[1])->context);
    }

private:
    QJsonObject getState(const ta::TranslationFiles& tf)
    {
        QString rawJason;
        QDebug stream{ &rawJason };
        stream << tf;

        auto document = QJsonDocument::fromJson(rawJason.toUtf8());
        return document.object();
    }

    QJsonObject findTranslation(const QJsonArray& array, QString context, QString source)
    {
        auto it = std::find_if(
            array.cbegin(), array.cend(), 
            [&context, &source](const QJsonValue& value)
            {
                auto obj = value.toObject();
                return obj["context"].toString() == context 
                    && obj["source"].toString() == source;
            }
        );

        if(it != array.cend())
        {
            return it->toObject();
        }
        return {};
    }

    QString m_executableDir;
};