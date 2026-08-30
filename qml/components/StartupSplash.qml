import QtQuick
import QtQuick.Window
import QtQuick.Controls

Window {
    id: splash
    width: 720
    height: 420
    visible: true
    color: "transparent"
    flags: Qt.SplashScreen | Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint
    modality: Qt.ApplicationModal

    signal finished()

    property real progressValue: 0
    property string phaseText: "STARTING APPLICATION"
    property bool canSkip: false

    function finishSplash() {
        if (!visible)
            return
        exitAnimation.start()
    }

    Rectangle {
        id: shell
        anchors.fill: parent
        radius: 10
        color: "#0A0F13"
        border.color: "#26343E"
        border.width: 1
        opacity: 0

        Rectangle {
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: 4
            radius: 2
            color: "#2FA7D8"
        }

        // Subtle technical grid.
        Canvas {
            anchors.fill: parent
            opacity: 0.12
            onPaint: {
                var ctx = getContext("2d")
                ctx.clearRect(0, 0, width, height)
                ctx.strokeStyle = "#567180"
                ctx.lineWidth = 1
                for (var x = 38; x < width; x += 38) {
                    ctx.beginPath()
                    ctx.moveTo(x, 0)
                    ctx.lineTo(x, height)
                    ctx.stroke()
                }
                for (var y = 38; y < height; y += 38) {
                    ctx.beginPath()
                    ctx.moveTo(0, y)
                    ctx.lineTo(width, y)
                    ctx.stroke()
                }
            }
        }

        // Moving scan line: low-key, not a gaming effect.
        Rectangle {
            id: scanLine
            x: 5
            width: parent.width - 10
            height: 1
            color: "#55BFE9"
            opacity: 0.16
            y: 40

            SequentialAnimation on y {
                loops: Animation.Infinite
                NumberAnimation {
                    from: 42
                    to: shell.height - 42
                    duration: 1900
                    easing.type: Easing.InOutSine
                }
                NumberAnimation {
                    from: shell.height - 42
                    to: 42
                    duration: 1900
                    easing.type: Easing.InOutSine
                }
            }
        }

        Item {
            anchors.fill: parent
            anchors.margins: 38

            Row {
                id: brandRow
                anchors.left: parent.left
                anchors.top: parent.top
                spacing: 17

                Rectangle {
                    width: 58
                    height: 58
                    radius: 8
                    color: "#101A21"
                    border.color: "#315263"

                    Text {
                        anchors.centerIn: parent
                        text: "A"
                        color: "#E7F6FC"
                        font.pixelSize: 31
                        font.weight: Font.DemiBold
                    }

                    Canvas {
                        anchors.fill: parent
                        anchors.margins: 12
                        onPaint: {
                            var ctx = getContext("2d")
                            ctx.strokeStyle = "#2FA7D8"
                            ctx.lineWidth = 2
                            ctx.beginPath()
                            ctx.moveTo(4, 29)
                            ctx.lineTo(11, 29)
                            ctx.lineTo(15, 19)
                            ctx.lineTo(20, 35)
                            ctx.lineTo(25, 25)
                            ctx.lineTo(34, 25)
                            ctx.stroke()
                        }
                    }
                }

                Column {
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 4

                    Text {
                        text: "AXIOM"
                        color: "#F1F5F7"
                        font.pixelSize: 25
                        font.weight: Font.DemiBold
                        font.letterSpacing: 2.1
                    }

                    Text {
                        text: "AUTOMOTIVE TELEMETRY WORKBENCH"
                        color: "#80939F"
                        font.pixelSize: 10
                        font.weight: Font.Medium
                        font.letterSpacing: 0.8
                    }
                }
            }

            Text {
                anchors.right: parent.right
                anchors.top: parent.top
                text: "v2.6.2"
                color: "#5F717C"
                font.pixelSize: 9
                font.family: "Consolas"
            }

            Column {
                id: startupBlock
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: brandRow.bottom
                anchors.topMargin: 72
                spacing: 12

                Row {
                    width: parent.width

                    Text {
                        width: parent.width - percentText.width
                        text: splash.phaseText
                        color: "#B8C5CC"
                        font.pixelSize: 10
                        font.weight: Font.DemiBold
                        font.letterSpacing: 0.55
                    }

                    Text {
                        id: percentText
                        text: Math.round(splash.progressValue) + "%"
                        color: "#758791"
                        font.pixelSize: 9
                        font.family: "Consolas"
                    }
                }

                Rectangle {
                    id: progressTrack
                    width: parent.width
                    height: 5
                    radius: 2
                    color: "#172128"
                    border.color: "#24333B"

                    Rectangle {
                        width: Math.max(2, progressTrack.width * splash.progressValue / 100)
                        height: parent.height
                        radius: 2
                        color: splash.progressValue >= 99 ? "#5CB88A" : "#2FA7D8"
                    }
                }

                Row {
                    width: parent.width
                    spacing: 8

                    Repeater {
                        model: [
                            { label: "CORE", threshold: 18 },
                            { label: "I/O", threshold: 40 },
                            { label: "PARSER", threshold: 64 },
                            { label: "WORKSPACE", threshold: 84 },
                            { label: "READY", threshold: 99 }
                        ]

                        delegate: Rectangle {
                            width: (startupBlock.width - 32) / 5
                            height: 27
                            radius: 4
                            color: splash.progressValue >= modelData.threshold ? "#11242C" : "#10171C"
                            border.color: splash.progressValue >= modelData.threshold ? "#2C748F" : "#25323A"

                            Text {
                                anchors.centerIn: parent
                                text: modelData.label
                                color: splash.progressValue >= modelData.threshold ? "#9DDDF3" : "#5F707A"
                                font.pixelSize: 8
                                font.family: "Consolas"
                                font.weight: Font.Medium
                            }
                        }
                    }
                }
            }

            Rectangle {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: footer.top
                anchors.bottomMargin: 17
                height: 1
                color: "#202C33"
            }

            Row {
                id: footer
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom

                Text {
                    width: parent.width * 0.55
                    text: "C++17  /  Qt 6  /  ASYNCHRONOUS I/O"
                    color: "#596B76"
                    font.pixelSize: 8
                    font.family: "Consolas"
                }

                Text {
                    width: parent.width * 0.45
                    horizontalAlignment: Text.AlignRight
                    text: "AXIOM  •  Amirhossein Haghi"
                    color: "#7D8F99"
                    font.pixelSize: 9
                }
            }
        }

        MouseArea {
            anchors.fill: parent
            enabled: splash.canSkip
            cursorShape: Qt.PointingHandCursor
            onClicked: splash.finishSplash()
        }
    }

    SequentialAnimation {
        id: enterAnimation
        running: true

        ParallelAnimation {
            NumberAnimation {
                target: shell
                property: "opacity"
                from: 0
                to: 1
                duration: 230
                easing.type: Easing.OutCubic
            }
            NumberAnimation {
                target: shell
                property: "scale"
                from: 0.985
                to: 1
                duration: 260
                easing.type: Easing.OutCubic
            }
        }

        ScriptAction {
            script: bootSequence.start()
        }
    }

    SequentialAnimation {
        id: bootSequence

        ScriptAction {
            script: {
                splash.phaseText = "STARTING APPLICATION"
                splash.canSkip = false
            }
        }
        NumberAnimation {
            target: splash
            property: "progressValue"
            from: 0
            to: 22
            duration: 260
            easing.type: Easing.OutCubic
        }

        ScriptAction { script: splash.phaseText = "PREPARING TRANSPORT SERVICES" }
        NumberAnimation {
            target: splash
            property: "progressValue"
            to: 47
            duration: 300
            easing.type: Easing.InOutCubic
        }

        ScriptAction {
            script: {
                splash.phaseText = "INITIALIZING TELEMETRY PIPELINE"
                splash.canSkip = true
            }
        }
        NumberAnimation {
            target: splash
            property: "progressValue"
            to: 71
            duration: 300
            easing.type: Easing.InOutCubic
        }

        ScriptAction { script: splash.phaseText = "PREPARING ENGINEERING WORKSPACE" }
        NumberAnimation {
            target: splash
            property: "progressValue"
            to: 93
            duration: 300
            easing.type: Easing.OutCubic
        }

        ScriptAction { script: splash.phaseText = "READY" }
        NumberAnimation {
            target: splash
            property: "progressValue"
            to: 100
            duration: 180
            easing.type: Easing.OutCubic
        }

        PauseAnimation { duration: 210 }
        ScriptAction { script: splash.finishSplash() }
    }

    SequentialAnimation {
        id: exitAnimation

        ParallelAnimation {
            NumberAnimation {
                target: shell
                property: "opacity"
                to: 0
                duration: 180
                easing.type: Easing.InCubic
            }
            NumberAnimation {
                target: shell
                property: "scale"
                to: 1.012
                duration: 180
                easing.type: Easing.InCubic
            }
        }

        ScriptAction {
            script: {
                splash.visible = false
                splash.finished()
                splash.close()
            }
        }
    }
}