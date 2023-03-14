
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test)

enable_testing(true)

add_executable(
    translation_assistant_tests 
    test_main.cpp
    TranslationFilesLoadTest.h
)
add_test(NAME translation_assistant_tests COMMAND translation_assistant_tests)

target_link_libraries(translation_assistant_tests 
    PRIVATE TranslationAssistant Qt${QT_VERSION_MAJOR}::Test)