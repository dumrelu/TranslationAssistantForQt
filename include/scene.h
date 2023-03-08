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

class Scene : public QObject 
{
    Q_OBJECT

public:
    explicit Scene(QQuickWindow* window);
    ~Scene();

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

private:
    // For clicks in the QQuickWindow
    bool eventFilter(QObject* obj, QEvent* event) override;

    QQuickWindow* m_window = nullptr;
};

}