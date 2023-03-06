cmake_minimum_required(VERSION 3.14)

project(QTranslationAssistant VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Quick LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Quick LinguistTools)

set(QTRANSLATIONASSISTANT_SOURCES
    include/qtranslationassistant.h src/qtranslationassistant.cpp
)

add_library(QTranslationAssistant ${QTRANSLATIONASSISTANT_SOURCES})

target_include_directories(QTranslationAssistant PUBLIC include/)

target_compile_definitions(QTranslationAssistant
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(QTranslationAssistant
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick)

add_subdirectory(aut)
