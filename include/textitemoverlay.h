#pragma once

#include <QQuickPaintedItem>
#include <QQuickWindow>

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
};

}