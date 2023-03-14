#pragma once 

#include <QTest>

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
    }

    void testHelloWorld()
    {
        QVERIFY(false);
    }

private:
    QString m_executableDir;
};