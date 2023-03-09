#include "textitemoverlay.h"

#include <QPainter>

#include <limits>

namespace qta
{

TextItemOverlay::TextItemOverlay(QQuickWindow *window)
{
    Q_ASSERT(window);

    auto* contentItem = window->contentItem();
    Q_ASSERT(contentItem);
    setParentItem(window->contentItem());

    setSize(window->size());
    setZ(std::numeric_limits<qreal>::max());

    auto updateCallback = [this, window]()
        {
            setSize(window->size());
            update();
        };
    connect(window, &QQuickWindow::widthChanged, this, updateCallback);
    connect(window, &QQuickWindow::heightChanged, this, updateCallback);
}

void TextItemOverlay::paint(QPainter *painter)
{
    qDebug() << __PRETTY_FUNCTION__;
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