#pragma once

#include <QObject>
#include <QQuickWindow>
#include <QAbstractListModel>

#include "scene.h"
#include "textitemoverlay.h"
#include "translationfiles.h"
#include "pendingtranslator.h"

namespace ta
{

class TranslationAssistant : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles
    {
        // Qt Roles user defined
        Source = Qt::UserRole + 1,
        Translation, 
        Context,
    };

    explicit TranslationAssistant(QQuickWindow* window, QObject* parent = nullptr);

    /// @brief Adds a translation file that will be edited by the TranslationAssistant
    /// @param filename 
    /// @return 
    bool addTranslationFile(const QString& filename);

    // QAbstractListModel interface
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;

private:
    void onTextItemCreated(QSharedPointer<TextItem> textItem);
    void onTextItemInvalidated(QSharedPointer<TextItem> textItem);
    void onTextItemClicked(QSharedPointer<TextItem> textItem);
    void onTextChanged(QSharedPointer<TextItem> textItem);

    void updateHighlight(TextItemOverlay* overlay, const QSharedPointer<TextItem>& selectedTextItem);
    void updateHighlights(const QSharedPointer<TextItem>& selectedTextItem);

    void createUiOverlay();
    void buildModel();

    // Check to see which of the translation from the given list are used for the given text item.
    QList<TranslationFiles::TranslationID> verifyTranslations(const QSharedPointer<TextItem>& textItem, QList<TranslationFiles::TranslationID> translations);

    QQuickWindow* m_window;
    QQmlEngine* m_qmlEngine;

    Scene m_scene;
    QHash<QSharedPointer<TextItem>, TextItemOverlay*> m_textItemOverlays;
    TranslationFiles m_translationFiles;
    PendingTranslator m_pendingTranslator;

    QList<TranslationFiles::TranslationID> m_allTranslations;
    QList<TranslationFiles::TranslationID> m_possibleTranslations;
    QList<TranslationFiles::TranslationID> m_verifiedTranslations;

    QColor m_selectedTextColor = QColor{ 0, 255, 0 };
    QColor m_relatedTextColor = QColor{ 0, 0, 255 };
};

}
