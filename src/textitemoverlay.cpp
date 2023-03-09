#include "textitemoverlay.h"

#include <QPainter>

namespace qta
{

TextItemOverlay::TextItemOverlay(QQuickWindow *window)
{
    Q_ASSERT(window);

    //TODO
    setWidth(200);
    setHeight(200);
    setZ(999999); //TODO: traits::max()

    setParentItem(window->contentItem());
}

void TextItemOverlay::paint(QPainter *painter)
{
    // QBrush brush(QColor("#FFFFFF"), Qt::CrossPattern);

    // painter->setBrush(brush);
    // painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::NoBrush);
    painter->setPen(Qt::white);
    painter->setRenderHint(QPainter::Antialiasing);

    QSizeF itemSize = size();
    painter->drawRoundedRect(0, 0, itemSize.width(), itemSize.height() - 10, 10, 10);
}

}