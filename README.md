# TranslationAssistant

# TODO
- Rename to TranslationAssistantForQml(maybe support C++ in the future)
- Install directives
- Save changes + test
- Find simpler ways to integrate in app
    - e.g. Another qml whose purpose is to only instantiate it. User will only instantiate a single QML type
    - Maybe something where no user interaction required
- Documentation + integration guide
- TODO comments from code
- Use onTextChanged to update the highlights(maybe using a QTimer so no spam, with max 1 update/s)
    - Note: ignore onTextChanged cause by temporary translator