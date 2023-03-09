#pragma once

#include <QObject>
#include <QQuickItem>
#include <QQuickWindow>
#include <QHash>
#include <QTimer>

#include <unordered_set>

#include "textitem.h"
#include "textitemhandler.h"

namespace qta
{

class SceneHooks;

class Scene : public QObject 
{
    Q_OBJECT

public:
    explicit Scene(QQuickWindow* window);
    ~Scene();

    /// @brief Returns the window associated with this scene
    QQuickWindow* window() { return m_window; }

    /// @brief Start processing the scene described by the 
    ///rootObject. Until this method is called, no signals
    //will fire and some methods won't work properly due to
    //lack of data.
    void start();

    /// @brief Stop processing
    /**
     * Signals will no longer fire. All TextItems will be destroyed.
    */
    void stop();

signals:
    /// @brief Emitted when a text item is created or when a text item's
    ///text changes
    void textChanged(QSharedPointer<TextItem> textItem);

    /// @brief Emitted when the textItem becomes invalid
    void textItemInvalidated(QSharedPointer<TextItem> textItem);

private:
    // Hook callback
    friend class SceneHooks;
    void addQQuickItemHook(QQuickItem* item);

    // For clicks in the QQuickWindow
    bool eventFilter(QObject* obj, QEvent* event) override;

    // TextItem creation
    void createTextItemsForSubtree(QQuickItem* root);
    void createTextItemIfRequired(QQuickItem* item);
    void insertTextItems(QQuickItem* item, QList<QSharedPointer<TextItem>> textItems);

    QQuickWindow* m_window = nullptr;
    QHash<QQuickItem*, QList<QSharedPointer<TextItem>>> m_textItems;
    QList<QSharedPointer<TextItemHandler>> m_textItemHandlers;
};

}