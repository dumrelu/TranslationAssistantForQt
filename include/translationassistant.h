#pragma once

#include <QObject>
#include <QQuickWindow>
#include <QAbstractListModel>
#include <QSortFilterProxyModel>

#include "scene.h"
#include "textitemoverlay.h"
#include "translationfiles.h"
#include "pendingtranslator.h"
#include "translationidentifier.h"

namespace ta
{

class TranslationAssistant : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QSortFilterProxyModel* relevantTranslationsModel READ relevantTranslationsModel CONSTANT)
    Q_PROPERTY(QSortFilterProxyModel* pendingTranslationsModel READ pendingTranslationsModel CONSTANT)
    Q_PROPERTY(QColor selectedTextColor READ selectedTextColor NOTIFY selectedTextColorChanged)
    Q_PROPERTY(QColor relevantTextColor READ relevantTextColor NOTIFY relevantTextColorChanged)

public:
    enum Roles
    {
        // Qt Roles user defined
        ID = Qt::UserRole + 1,
        Source, 
        Translation, 
        Context,
        Comment, 
        IsFinished,
        IsPending, 
    };

    /// @brief Create a new TranslationAssistant for the text items in the specified window
    /// @param window 
    /// @param parent 
    explicit TranslationAssistant(QQuickWindow* window, QObject* parent = nullptr);

    /// @brief Register the .ts files that will be edited by the translation assistant
    /// @param fileNames 
    /// @return 
    bool addTranslationSources(const QStringList& tsFileNames);

    /// @brief Highlight all the text items containing the given translation id using
    ///the relevantTranslationColor
    /// @param translationID 
    /// @return 
    Q_INVOKABLE void highlightTranslation(QVariant translationID = {});

    /// @brief Clear all the highlights
    /// @return 
    Q_INVOKABLE void clearHighlights();

    /// @brief Clear the currently selected relevant translations
    /// @return 
    Q_INVOKABLE void clearRelevantTranslations();

    /// @brief Discard all the pending translations
    /// @return 
    Q_INVOKABLE void clearPendingTranslations();

    /// @brief Replace the strategy used to identify the translations used 
    ///by all the text items
    /// @param translationIdentifier 
    void setTranslationIdentifier(std::unique_ptr<TranslationIdentifier> translationIdentifier);

    /// @brief Removes the currently used translation identifier strategy and
    ///returns the instance. Note: if you remove it, you need to replace it
    ///by calling setTranslationIdentifier() with a new instance.
    /// @return 
    std::unique_ptr<TranslationIdentifier> removeTranslationIdentifier();

    // Getters
    QSortFilterProxyModel* relevantTranslationsModel();
    QSortFilterProxyModel* pendingTranslationsModel();
    QColor selectedTextColor() const;
    QColor relevantTextColor() const;

    // QAbstractListModel interface
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;

signals:
    void selectedTextColorChanged();
    void relevantTextColorChanged();

private:
    void onTextItemCreated(QSharedPointer<TextItem> textItem);
    void onTextItemInvalidated(QSharedPointer<TextItem> textItem);
    void onTextItemClicked(QSharedPointer<TextItem> textItem);
    void onTextChanged(QSharedPointer<TextItem> textItem);

    void rebuildModel();
    bool isIndexValid(const QModelIndex& index) const;
    void createUiOverlay();
    bool isTranslationAssistantTextItem(const QSharedPointer<TextItem>& textItem) const;
    TranslationMap identifyTranslations();
    template <typename Predicate>
    void highlightTranslations(const TranslationMap& translationMap, const QColor& color, Predicate condition);
    void highlightTranslation(const TranslationMap& translationMap, const TranslationFiles::TranslationID& translationID);
    void highlightRelevantTranslations(const TranslationMap& translationMap);

    QQuickWindow* m_window = nullptr;
    QQmlEngine* m_qmlEngine = nullptr;

    QSortFilterProxyModel m_relevantTranslationsModel;
    QSortFilterProxyModel m_pendingTranslationsModel;

    TranslationFiles m_translationFiles;
    QList<TranslationFiles::TranslationID> m_allTranslations;
    QHash<TranslationFiles::TranslationID, int> m_allTranslationsIndices;
    QList<TranslationFiles::TranslationID> m_relevantTranslations;

    PendingTranslator m_pendingTranslator;

    std::unique_ptr<TranslationIdentifier> m_translationIdentifier;

    Scene m_scene;
    QHash<QSharedPointer<TextItem>, TextItemOverlay*> m_textItemOverlays;
    QColor m_selectedTextColor = QColor{ 0, 255, 0 };
    QColor m_relevantTextColor = QColor{ 0, 0, 255 };
};

}
