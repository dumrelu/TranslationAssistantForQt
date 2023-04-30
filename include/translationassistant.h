#pragma once

#include <QObject>
#include <QQuickWindow>

#include "scene.h"
#include "textitemoverlay.h"
#include "translationfiles.h"
#include "pendingtranslator.h"

namespace ta
{

class TranslationAssistant : public QObject
{
    Q_OBJECT
public:
    explicit TranslationAssistant(QQuickWindow* window, QObject* parent = nullptr);

    TranslationFiles& translationFiles() { return m_translationFiles; }

private:
    void onTextItemCreated(QSharedPointer<TextItem> textItem);
    void onTextItemInvalidated(QSharedPointer<TextItem> textItem);
    void onTextItemClicked(QSharedPointer<TextItem> textItem);

    Scene m_scene;
    QHash<QSharedPointer<TextItem>, TextItemOverlay*> m_textItemOverlays;
    TranslationFiles m_translationFiles;
    PendingTranslator m_pendingTranslator;
};

}
