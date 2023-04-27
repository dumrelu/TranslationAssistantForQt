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
    explicit TextItemOverlay(QSharedPointer<TextItem> textItem);

    void paint(QPainter* painter) override;

signals:
    void textItemClicked(QSharedPointer<TextItem> textItem);

protected:
    void mousePressEvent(QMouseEvent* event) override;

private:
    QSharedPointer<TextItem> m_textItem;
};

}