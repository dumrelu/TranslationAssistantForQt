#pragma once 

#include <QTest>

#include "translationfiles.h"

class TranslationFilesLoadTest : public QObject
{
    Q_OBJECT
public:
    TranslationFilesLoadTest(QString executableDir)
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
    }

private:
    QString m_executableDir;
};