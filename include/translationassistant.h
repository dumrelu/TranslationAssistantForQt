#pragma once

#include <QObject>
#include <QQuickWindow>
#include <QAbstractListModel>
#include <QSortFilterProxyModel>

#include "scene.h"
#include "textitemoverlay.h"
#include "translationfiles.h"
#include "pendingtranslator.h"

namespace ta
{

class TranslationAssistant : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QColor selectedTextColor READ selectedTextColor NOTIFY selectedTextColorChanged)

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

    // Getters
    QColor selectedTextColor() const;

    // QAbstractListModel interface
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;

signals:
    void selectedTextColorChanged();

private:
    using TranslationMap = QHash<QSharedPointer<TextItem>, QList<TranslationFiles::TranslationID>>;

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

    TranslationFiles m_translationFiles;
    QList<TranslationFiles::TranslationID> m_allTranslations;
    QList<TranslationFiles::TranslationID> m_relevantTranslations;

    PendingTranslator m_pendingTranslator;

    Scene m_scene;
    QHash<QSharedPointer<TextItem>, TextItemOverlay*> m_textItemOverlays;
    QColor m_selectedTextColor = QColor{ 0, 255, 0 };
    QColor m_relevantTextColor = QColor{ 0, 0, 255 };
};

}
