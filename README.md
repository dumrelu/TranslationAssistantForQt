# TranslationAssistant

When it will work: When bindings for text properties are working. So text: x + qsTr("Text"), not text = x + qsTr("Text") (use Qt.binding instead). When you have up-to-date translation files

Can be extended with different strategies to customize for you specific use case:
    - text item extraction
    - translation identifiers(e.g. customization for work, minimum requirements)
    - Could work for Qt C++ with appropriate retranslation handlings
    - etc

Minimum version: TBD(probably first version that supports QQmlEngine::retranslate, Qt 5.10)

# TODO
- Rename to TranslationAssistantForQml(maybe support C++ in the future)
- Can we use .qm files?
- Install directivess
- Save changes + test
- Find simpler ways to integrate in app
    - e.g. Another qml whose purpose is to only instantiate it. User will only instantiate a single QML type
    - Maybe something where no user interaction required
- Documentation + integration guide
- TODO comments from code
- Use onTextChanged to update the highlights(maybe using a QTimer so no spam, with max 1 update/s)
    - Note: ignore onTextChanged cause by temporary translator
- Suggested translations via DeepL or Google Translate