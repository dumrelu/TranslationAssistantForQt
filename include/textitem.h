#pragma once

#include <QObject>
#include <QQuickItem>
#include <QEnableSharedFromThis>

namespace qta
{

/// @brief Interface to interact with QQuickItems that hold text
class TextItem : public QObject, public QEnableSharedFromThis<TextItem>
{
    Q_OBJECT
public:
    explicit TextItem(QQuickItem* item);
    virtual ~TextItem() = default;

    /// @brief Returns true if the underlying QQuickItem is
    ///still valid.
    /// @return 
    bool isValid() const { return m_item != nullptr; }

    /// @brief Retruns true if the item is valid and visible
    /// @return 
    bool isVisible() const;

    /// @brief Returns the underlying QQuickItem
    /**
     *  If the item got destroyed, returns nullptr
    */
    QQuickItem* item() { return m_item; };
    const QQuickItem* item() const { return m_item; }

    /// @brief Returns the current text of the underlying item
    /**
     *  If the text item is no longer valid, an empty string is returned
    */
    QString text();

signals:
    /// @brief Emitted when the underlaying text changed.
    /**
     *  Note: Subclasses should emit this signal when the text changes
    */
    void textChanged();

    /// @brief Emitted when the text item becomes invalid(isValid() == false)
    void invalidated();

protected:
    /// @brief Subclass specific logic for extractig the 
    ///text.
    virtual QString doGetText() = 0;

    QQuickItem* m_item = nullptr;
};

}