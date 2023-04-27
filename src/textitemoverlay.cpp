#include "textitemoverlay.h"

#include <QPainter>

#include <limits>

namespace ta
{

TextItemOverlay::TextItemOverlay(QSharedPointer<TextItem> textItem)
    : m_textItem{ std::move(textItem) }
{
    Q_ASSERT(m_textItem);

    setParent(m_textItem->item());
    setParentItem(m_textItem->item());

    setAntialiasing(true);
    setZ(std::numeric_limits<qreal>::max());
    setAcceptedMouseButtons(Qt::AllButtons);

    qvariant_cast<QObject*>(property("anchors"))->setProperty("fill", QVariant::fromValue(parentItem()));
}

void TextItemOverlay::paint(QPainter *painter)
{
    painter->setBrush(Qt::NoBrush);
    painter->setPen(Qt::white);

    // TODO: Only draw when textItem highlighted
    painter->drawRoundedRect(boundingRect(), 10, 10);
}

void TextItemOverlay::mousePressEvent(QMouseEvent *event)
{
    qWarning() << "ItemClicked!" << event;

    emit textItemClicked(m_textItem);

    // Propagate the mouse event further
    event->ignore();
}

}