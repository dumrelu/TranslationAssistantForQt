#pragma once

#include <QObject>
#include <QQuickWindow>
#include <QAbstractListModel>
#include <QSortFilterProxyModel>
#include <QRegularExpression>

#include "scene.h"
#include "textitemoverlay.h"
#include "translationfiles.h"
#include "pendingtranslator.h"

namespace ta
{

class TranslationAssistant : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QSortFilterProxyModel* verifiedTranslationsModel READ verifiedTranslationsModel CONSTANT)
    Q_PROPERTY(QSortFilterProxyModel* pendingTranslationsModel READ pendingTranslationsModel CONSTANT)
    Q_PROPERTY(QString selectedText READ selectedText WRITE setSelectedText NOTIFY selectedTextChanged)
    Q_PROPERTY(QColor selectedTextColor READ selectedTextColor CONSTANT)
    Q_PROPERTY(QColor relatedTextColor READ relatedTextColor CONSTANT)

public:
    enum Roles
    {
        // Qt Roles user defined
        ID = Qt::UserRole + 1,
        Source, 
        Translation, 
        Context,
        TranslationType,
        IsPending, 
    };

    explicit TranslationAssistant(QQuickWindow* window, QObject* parent = nullptr);

    /// @brief Adds a translation file that will be edited by the TranslationAssistant
    /// @param filename 
    /// @return 
    bool addTranslationFile(const QString& filename);

    /// @brief Will highlight all the text items using the given translation
    /// @param translationID 
    /// @return 
    Q_INVOKABLE bool translationClicked(QVariant translationID = {});

    QSortFilterProxyModel* verifiedTranslationsModel();
    QSortFilterProxyModel* pendingTranslationsModel();
    QString selectedText() const;
    QColor selectedTextColor() const;
    QColor relatedTextColor() const;

    void setSelectedText(QString selectedText);
    Q_INVOKABLE void clearSelectedText();

    // QAbstractListModel interface
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;

signals:
    void selectedTextChanged();

private:
    void onTextItemCreated(QSharedPointer<TextItem> textItem);
    void onTextItemInvalidated(QSharedPointer<TextItem> textItem);
    void onTextItemClicked(QSharedPointer<TextItem> textItem);
    void onTextChanged(QSharedPointer<TextItem> textItem);

    void updateHighlight(TextItemOverlay* overlay, const QSharedPointer<TextItem>& selectedTextItem);
    void updateHighlights(const QSharedPointer<TextItem>& selectedTextItem);

    void installTemporaryTranslator();
    void removeTemporaryTranslator();

    void createUiOverlay();
    void createTemporaryTranslator();
    void buildModel();

    bool isIndexValid(const QModelIndex& index) const;

    // Using the temporary translator, get the translations for the given text item
    /**
     *  Installs the temporary translator if necessary.
    */
    QList<TranslationFiles::TranslationID> getTranslationsForTextItem(const QSharedPointer<TextItem>& textItem);

    QSortFilterProxyModel m_verifiedTranslationsModel;
    QSortFilterProxyModel m_pendingTranslationsModel;

    QQuickWindow* m_window = nullptr;
    QQmlEngine* m_qmlEngine = nullptr;

    Scene m_scene;
    QHash<QSharedPointer<TextItem>, TextItemOverlay*> m_textItemOverlays;
    TranslationFiles m_translationFiles;
    PendingTranslator m_pendingTranslator;
    //TODO: This should be split into a different class because it is not really a pending translator.
    PendingTranslator m_temporaryTranslator;
    bool m_temporaryTranslatorInstalled = false;
    const QString m_tempTranslationFormat = QStringLiteral("i_%1_d");
    QRegularExpression m_tempTranslationRegex;

    QList<TranslationFiles::TranslationID> m_allTranslations;
    QList<TranslationFiles::TranslationID> m_verifiedTranslations;
    QString m_selectedText;

    QColor m_selectedTextColor = QColor{ 0, 255, 0 };
    QColor m_relatedTextColor = QColor{ 0, 0, 255 };
};

}
