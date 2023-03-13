#include "textitemoverlay.h"

#include <QPainter>

#include <limits>

namespace ta
{

TextItemOverlay::TextItemOverlay(QQuickWindow *window)
{
    Q_ASSERT(window);

    auto* contentItem = window->contentItem();
    Q_ASSERT(contentItem);
    setParentItem(window->contentItem());

    setSize(window->size());
    setZ(std::numeric_limits<qreal>::max());

    setAntialiasing(true);

    auto resizeCallback = [this, window]() { setSize(window->size()); };
    connect(window, &QQuickWindow::widthChanged, this, resizeCallback);
    connect(window, &QQuickWindow::heightChanged, this, resizeCallback);
    connect(window, &QQuickWindow::beforeRendering, this, &QQuickItem::update);
}

void TextItemOverlay::paint(QPainter *painter)
{
    painter->setBrush(Qt::NoBrush);
    painter->setPen(Qt::white);

    for(const auto& textItem : m_textItems)
    {
        drawOverlay(painter, textItem);
    }
}

bool TextItemOverlay::addOverlayFor(QSharedPointer<TextItem> textItem)
{
    if(!textItem || !textItem->isValid() || m_textItems.contains(textItem))
    {
        return false;
    }

    auto* textItemRawPointer = textItem.get();
    connect(
        textItemRawPointer, &TextItem::invalidated, 
        this, [this, textItemRawPointer]()
        {
            textItemInvalidated(textItemRawPointer->sharedFromThis());
        }
    );

    m_textItems.insert(std::move(textItem));

    return true;
}

bool TextItemOverlay::removeOverlayFor(QSharedPointer<TextItem> textItem)
{
    if(!textItem || !m_textItems.contains(textItem))
    {
        return false;
    }

    disconnect(textItem.get(), nullptr, this, nullptr);

    m_textItems.remove(textItem);

    return true;
}

void TextItemOverlay::textItemInvalidated(QSharedPointer<TextItem> textItem)
{
    removeOverlayFor(std::move(textItem));
}

void TextItemOverlay::drawOverlay(QPainter *painter, const QSharedPointer<TextItem>& textItem) const
{
    if(!textItem->isVisible())
    {
        return;
    }

    const auto textItemBoundingRect = mapRectFromItem(textItem->item(), textItem->item()->boundingRect());
    painter->drawRoundedRect(textItemBoundingRect, 10, 10);
}

}