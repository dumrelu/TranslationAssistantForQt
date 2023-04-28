#include "textitemoverlay.h"

#include <QPainter>

#include <limits>

namespace ta
{

TextItemOverlay::TextItemOverlay(QSharedPointer<TextItem> textItem, bool highlighted)
    : m_textItem{ std::move(textItem) }
    , m_highlighted{ highlighted }
{
    Q_ASSERT(m_textItem);

    setParent(m_textItem->item());
    setParentItem(m_textItem->item());

    setAntialiasing(true);
    setZ(std::numeric_limits<qreal>::max());
    setAcceptedMouseButtons(Qt::AllButtons);

    qvariant_cast<QObject*>(property("anchors"))->setProperty("fill", QVariant::fromValue(parentItem()));
}

void TextItemOverlay::setHighlighted(bool highlighted)
{
    if (m_highlighted == highlighted)
    {
        return;
    }

    m_highlighted = highlighted;
    update();
}

void TextItemOverlay::setHighlightColor(const QColor &color)
{
    m_highlightColor = color;
    
    if(m_highlighted)
    {
        update();
    }
}

void TextItemOverlay::paint(QPainter *painter)
{
    if(m_highlighted)
    {
        painter->setBrush(Qt::NoBrush);
        painter->setPen(m_highlightColor);

        painter->drawRoundedRect(boundingRect(), 10, 10);
    }
}

void TextItemOverlay::mousePressEvent(QMouseEvent *event)
{
    // TODO: have a look at the modifiers
    emit textItemClicked(m_textItem);

    // Propagate the mouse event further
    event->ignore();
}

}