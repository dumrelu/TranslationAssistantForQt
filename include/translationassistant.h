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

    // QAbstractListModel interface
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;

private:
    void rebuildModel();
    bool isIndexValid(const QModelIndex& index) const;
    void createUiOverlay();

    QQuickWindow* m_window = nullptr;
    QQmlEngine* m_qmlEngine = nullptr;

    TranslationFiles m_translationFiles;
    QList<TranslationFiles::TranslationID> m_allTranslations;
    QList<TranslationFiles::TranslationID> m_selectedTranslations;

    // Scene m_scene;
    // QHash<QSharedPointer<TextItem>, TextItemOverlay*> m_textItemOverlays;
};

}
