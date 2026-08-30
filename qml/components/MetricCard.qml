import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root

    property string label: "CHANNEL"
    property string value: "--"
    property string unit: ""
    property string subtitle: ""
    property color accent: "#3a9ad9"

    implicitWidth: 220
    implicitHeight: 90
    radius: 3
    color: "#171b20"
    border.color: "#353c45"
    border.width: 1
    clip: true

    Rectangle {
        width: 3
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        color: root.accent
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.leftMargin: 14
        anchors.rightMargin: 12
        anchors.topMargin: 10
        anchors.bottomMargin: 9
        spacing: 3

        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Label {
                Layout.fillWidth: true
                text: root.label
                color: "#aeb7c2"
                font.pixelSize: 10
                font.weight: Font.DemiBold
                elide: Text.ElideRight
            }

            Rectangle {
                width: 6
                height: 6
                radius: 3
                color: root.accent
                Layout.alignment: Qt.AlignVCenter
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 7

            Label {
                text: root.value
                color: "#f0f3f6"
                font.pixelSize: 26
                font.weight: Font.Medium
                                elide: Text.ElideRight
                Layout.maximumWidth: Math.max(70, parent.width - unitLabel.implicitWidth - 16)
            }

            Label {
                id: unitLabel
                text: root.unit
                color: "#7f8b98"
                font.pixelSize: 10
                Layout.alignment: Qt.AlignBottom
                Layout.bottomMargin: 6
            }

            Item { Layout.fillWidth: true }
        }

        Label {
            Layout.fillWidth: true
            visible: root.subtitle.length > 0
            text: root.subtitle
            color: "#6e7985"
            font.pixelSize: 9
            elide: Text.ElideRight
        }
    }
}