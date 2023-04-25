cmake_minimum_required(VERSION 3.14)

project(TranslationAssistant VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Quick LinguistTools Xml)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Quick LinguistTools Xml)

set(TRANSLATIONASSISTANT_SOURCES
    include/translationassistant.h src/translationassistant.cpp
    include/scene.h src/scene.cpp
    include/scenehooks.h src/scenehooks.cpp
    include/textitem.h src/textitem.cpp
    include/textitemhandler.h src/textitemhandler.cpp
    include/textitemoverlay.h src/textitemoverlay.cpp
    include/translationfiles.h src/translationfiles.cpp
    include/pendingtranslator.h src/pendingtranslator.cpp
)

add_library(TranslationAssistant ${TRANSLATIONASSISTANT_SOURCES})

target_include_directories(TranslationAssistant PUBLIC include/)

target_compile_definitions(TranslationAssistant
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(TranslationAssistant
  PUBLIC Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick Qt${QT_VERSION_MAJOR}::Xml)
target_link_libraries(TranslationAssistant
  PRIVATE Qt${QT_VERSION_MAJOR}::CorePrivate)

# Enable warnings
if(MSVC)
  target_compile_options(TranslationAssistant PRIVATE /W4 /WX)
else()
  target_compile_options(TranslationAssistant PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

add_subdirectory(aut)
#TODO: command line option
add_subdirectory(test)
