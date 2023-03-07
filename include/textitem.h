#pragma once

#include <QObject>
#include <QQuickItem>

namespace qta
{

/// @brief Interface to interact with QQuickItems that hold text
class TextItem : public QObject 
{
    Q_OBJECT
public:
    explicit TextItem(QQuickItem* item);
    
    virtual ~TextItem() = default;

    /// @brief Returns the actual QQuickItem associated with
    ///the TextItem
    QQuickItem* item();

    /// @brief Returns the extracted text from the QQuickItem
    virtual QString text() = 0;

signals:
    /// @brief Implementation should emit this signal when
    ///the text changes
    void textChanged();

protected:
    QQuickItem* m_item = nullptr;
};

}