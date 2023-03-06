#pragma once

#include <QObject>
#include <QQuickWindow>
#include <QQuickItem>

namespace qta
{


/// @brief Wrapper class for QQuickItem instances that
///represent text
class TextItem {
public:

    QQuickItem* item();
    QString text();

protected:
    friend class Scene;

    explicit TextItem(QQuickItem* item);

private:
    QQuickItem* m_item = nullptr;
};

class Scene : public QObject {
    Q_OBJECT

public:
    explicit Scene(QObject* rootObject);

    void start();

signals:
    void textItemClicked(TextItem* textItem);
    void textChanged(TextItem* textItem);
    void textItemDestroyed(TextItem* textItem);
private:
    QObject* m_rootObject = nullptr;
};

}