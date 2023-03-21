#include "TranslationFilesTest.h"

#include <QFile>

template <typename T, class... Params>
void execute_test(int argc, char* argv[], Params&&... params)
{
    T test{ std::forward<Params>(params)... };
    QTest::qExec(&test, argc, argv);
}

template <typename T>
void execute_test(int argc, char* argv[])
{
    T test{};
    QTest::qExec(&test, argc, argv);
}

int main(int argc, char* argv[])
{
    QFileInfo fileInfo{ QString{ argv[0] } };
    QString executableDir = fileInfo.absoluteDir().absolutePath();

    execute_test<TranslationFilesTest>(argc, argv, executableDir); 
}