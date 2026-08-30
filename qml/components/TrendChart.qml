import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root

    property string title: "Signal"
    property string unit: ""
    property var values: []
    property real minValue: 0
    property real maxValue: 100
    property color traceColor: "#39a7df"

    implicitWidth: 420
    implicitHeight: 235
    radius: 3
    color: "#12161a"
    border.color: "#343b44"
    border.width: 1
    clip: true

    onValuesChanged: plot.requestPaint()
    onMinValueChanged: plot.requestPaint()
    onMaxValueChanged: plot.requestPaint()
    onWidthChanged: plot.requestPaint()
    onHeightChanged: plot.requestPaint()

    Rectangle {
        id: headerBar
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        height: 32
        color: "#1a1f25"
        border.color: "#343b44"
        border.width: 1

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 10
            anchors.rightMargin: 10
            spacing: 8

            Label {
                Layout.fillWidth: true
                text: root.title
                color: "#d8dde3"
                font.pixelSize: 10
                font.weight: Font.DemiBold
                elide: Text.ElideRight
            }

            Label {
                text: root.values && root.values.length > 0
                      ? Number(root.values[root.values.length - 1]).toFixed(1) + " " + root.unit
                      : "-- " + root.unit
                color: root.traceColor
                font.pixelSize: 10
                font.family: "monospace"
            }
        }
    }

    Item {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: headerBar.bottom
        anchors.bottom: parent.bottom
        anchors.margins: 8

        Label {
            anchors.left: parent.left
            anchors.top: parent.top
            text: Number(root.maxValue).toFixed(0)
            color: "#68737f"
            font.pixelSize: 8
            font.family: "monospace"
        }

        Label {
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            text: Number((root.minValue + root.maxValue) / 2).toFixed(0)
            color: "#68737f"
            font.pixelSize: 8
            font.family: "monospace"
        }

        Label {
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            text: Number(root.minValue).toFixed(0)
            color: "#68737f"
            font.pixelSize: 8
            font.family: "monospace"
        }

        Canvas {
            id: plot
            anchors.left: parent.left
            anchors.leftMargin: 32
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom

            onPaint: {
                const ctx = getContext("2d")
                ctx.clearRect(0, 0, width, height)

                if (width < 2 || height < 2)
                    return

                ctx.lineWidth = 1
                ctx.strokeStyle = "#283039"

                for (let yIndex = 0; yIndex <= 4; ++yIndex) {
                    const y = yIndex * height / 4
                    ctx.beginPath()
                    ctx.moveTo(0, y)
                    ctx.lineTo(width, y)
                    ctx.stroke()
                }

                for (let xIndex = 0; xIndex <= 8; ++xIndex) {
                    const x = xIndex * width / 8
                    ctx.beginPath()
                    ctx.moveTo(x, 0)
                    ctx.lineTo(x, height)
                    ctx.stroke()
                }

                if (!root.values || root.values.length < 2)
                    return

                const range = Math.max(0.0001, root.maxValue - root.minValue)
                const count = root.values.length

                ctx.strokeStyle = root.traceColor
                ctx.lineWidth = 1.6
                ctx.beginPath()

                for (let i = 0; i < count; ++i) {
                    const raw = Number(root.values[i])
                    const normalized = Math.max(0, Math.min(1, (raw - root.minValue) / range))
                    const x = count > 1 ? i * width / (count - 1) : 0
                    const y = height - normalized * height
                    if (i === 0)
                        ctx.moveTo(x, y)
                    else
                        ctx.lineTo(x, y)
                }
                ctx.stroke()
            }
        }
    }
}