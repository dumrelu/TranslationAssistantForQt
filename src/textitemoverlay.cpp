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
        
    painter->setBrush(Qt::NoBrush);
    painter->setPen(Qt::white);
    painter->setRenderHint(QPainter::Antialiasing);

    for(const auto& textItem : m_textItems.keys())
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
    QList<QMetaObject::Connection> connections;
    connections.push_back(connect(
        textItemRawPointer, &TextItem::invalidated, 
        this, [this, textItemRawPointer]()
        {
            textItemInvalidated(textItemRawPointer->sharedFromThis());
        }
    ));
    connections.push_back(connect(textItem->item(), &QQuickItem::xChanged, this, [this](){ update(); }));
    connections.push_back(connect(textItem->item(), &QQuickItem::yChanged, this, [this](){ update(); }));
    connections.push_back(connect(textItem->item(), &QQuickItem::widthChanged, this, [this](){ update(); }));
    connections.push_back(connect(textItem->item(), &QQuickItem::heightChanged, this, [this](){ update(); }));
    connections.push_back(connect(textItem->item(), &QQuickItem::visibleChanged, this, [this](){ update(); }));

    m_textItems.insert(textItem, std::move(connections));

    update();

    return true;
}

bool TextItemOverlay::removeOverlayFor(QSharedPointer<TextItem> textItem)
{
    if(!textItem || !m_textItems.contains(textItem))
    {
        return false;
    }

    for(const auto& connection : m_textItems[textItem])
    {
        disconnect(connection);
    }

    m_textItems.remove(textItem);

    update();

    return true;
}

void TextItemOverlay::textItemInvalidated(QSharedPointer<TextItem> textItem)
{
    removeOverlayFor(std::move(textItem));
}

void TextItemOverlay::drawOverlay(QPainter *painter, const QSharedPointer<TextItem>& textItem) const
{
    if(!textItem || !textItem->isValid() || !textItem->item()->isVisible())
    {
        return;
    }

    const auto textItemBoundingRect = mapRectFromItem(textItem->item(), textItem->item()->boundingRect());
    painter->drawRoundedRect(textItemBoundingRect, 10, 10);
}

}