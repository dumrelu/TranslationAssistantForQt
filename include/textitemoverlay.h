#pragma once

#include <QQuickPaintedItem>
#include <QSharedPointer>

#include "textitem.h"

namespace ta
{

/// @brief An overlay for TextItems that can draw a rounded rectangle
///around the text and signal when a text item is clicked.
class TextItemOverlay : public QQuickPaintedItem
{
    Q_OBJECT
public:
    explicit TextItemOverlay(QSharedPointer<TextItem> textItem, bool highlighted = false);

    void setHighlighted(bool highlighted);
    bool highlighted() const { return m_highlighted; }

    void paint(QPainter* painter) override;

signals:
    void textItemClicked(QSharedPointer<TextItem> textItem);

protected:
    void mousePressEvent(QMouseEvent* event) override;

private:
    QSharedPointer<TextItem> m_textItem;
    bool m_highlighted = false;
};

}