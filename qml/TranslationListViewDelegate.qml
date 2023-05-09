import QtQuick 2.8
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.2

Rectangle {
    id: root
    
    height: columnLayout.height + columnLayout.anchors.topMargin * 2

    color: "transparent"

    border.color: "black"
    border.width: 2
    radius: 5

    ColumnLayout {
        id: columnLayout

        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.right: parent.right
        anchors.rightMargin: 10

        RowLayout {
            id: titleBar

            Layout.fillWidth: true
            
            spacing: 20

            Label {
                id: titleLabel

                Layout.fillWidth: true

                elide: Label.ElideRight
                font.pixelSize: Qt.application.font.pixelSize * 1.5
                text: model.translation ? model.translation : model.source
            }
            
            CheckBox {
                id: translationCheckBox

                checked: false  //TODO
            }
        }

        
    }
}