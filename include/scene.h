#pragma once

#include <QObject>
#include <QQuickItem>
#include <QSet>

namespace qta
{


/// @brief Wrapper class for QQuickItem instances that
///represent text
class TextItem : public QObject {
public:
    QQuickItem* item();
    QString text();

protected:
    friend class Scene;

    explicit TextItem(QObject* parent, QQuickItem* item);

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
    void findWindow();
    void findAllTextItems(QObject* obj);
    bool itemContainsText(QQuickItem* item) const;
    

    QObject* m_rootObject = nullptr;
    QQuickWindow* m_window = nullptr;
    QSet<TextItem*> m_textItems;
};

}