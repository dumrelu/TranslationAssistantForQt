#pragma once

#include <QQuickPaintedItem>
#include <QQuickWindow>
#include <QSharedPointer>

#include "textitem.h"

namespace qta
{

/// @brief Draws overlays for one or more TextItems
class TextItemOverlay : public QQuickPaintedItem
{
    Q_OBJECT
public:
    explicit TextItemOverlay(QQuickWindow* window);

    void paint(QPainter* painter) override;

    /// @brief Draw an overlay over the specified TextItem
    /// @param textItem 
    /// @return True if overlay added, false otherwise(e.g. overlay already added)
    /**
     *  Note: If the item becomes invalid, the overlay will automatically be
     * removed
    */
    bool addOverlayFor(QSharedPointer<TextItem> textItem);

    /// @brief Remove the overlay drawn over the specified TextItem
    /// @param textItem 
    /// @return True if overlay was removed, false otherwise(e.g. no overlay was drawn for the item)
    bool removeOverlayFor(QSharedPointer<TextItem> textItem);

    /// @brief Remove all the drawn overlays
    void removeAllOverlays();

private:
    void textItemInvalidated(QSharedPointer<TextItem> textItem);
    void drawOverlay(QPainter* painter, const QSharedPointer<TextItem>& textItem) const;

    QSet<QSharedPointer<TextItem>> m_textItems;
};

}