#include "TranslationFilesLoadTest.h"

template <typename T>
void execute_test(int argc, char* argv[])
{
    T test;
    QTest::qExec(&test, argc, argv);
}

int main(int argc, char* argv[])
{
    execute_test<TranslationFilesLoadTest>(argc, argv); 
}