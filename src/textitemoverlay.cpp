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
    //TODO: connect signals for position

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
    Q_UNUSED(painter);
    Q_UNUSED(textItem);
    if(!textItem || !textItem->isValid())
    {
        return;
    }

    qDebug() << "Should draw overlay for " << textItem->text();
}

}