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
        QVERIFY(tf.loadTranslationFile(m_executableDir + "/simple_file.ts") == true);
        
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