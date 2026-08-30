import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts

Window {
    id: splash
    width: 660
    height: 370
    visible: true
    color: "transparent"
    flags: Qt.SplashScreen | Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint
    modality: Qt.ApplicationModal

    signal finished()

    property real bootProgress: 0

    Rectangle {
        anchors.fill: parent
        radius: 8
        color: "#0b1014"
        border.color: "#283540"
        border.width: 1

        Rectangle {
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: 5
            radius: 2
            color: "#2fa4dc"
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 34
            spacing: 0

            RowLayout {
                Layout.fillWidth: true
                spacing: 16

                Rectangle {
                    width: 54
                    height: 54
                    radius: 7
                    color: "#101b23"
                    border.color: "#2d8ebb"
                    border.width: 1

                    Label {
                        anchors.centerIn: parent
                        text: "A"
                        color: "#e6f4fb"
                        font.pixelSize: 29
                        font.weight: Font.Bold
                    }

                    Rectangle {
                        width: 28
                        height: 3
                        radius: 1
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 9
                        color: "#34b6e9"
                    }
                }

                ColumnLayout {
                    spacing: 3
                    Label {
                        text: "AXIOM"
                        color: "#f0f4f7"
                        font.pixelSize: 24
                        font.weight: Font.DemiBold
                        font.letterSpacing: 2.0
                    }
                    Label {
                        text: "AUTOMOTIVE TELEMETRY WORKBENCH"
                        color: "#8293a0"
                        font.pixelSize: 10
                        font.weight: Font.Medium
                        font.letterSpacing: 0.8
                    }
                }

                Item { Layout.fillWidth: true }

                Label {
                    text: "v2.6.1"
                    color: "#586873"
                    font.pixelSize: 9
                    font.family: "monospace"
                    Layout.alignment: Qt.AlignTop
                }
            }

            Item { Layout.preferredHeight: 56 }

            Label {
                Layout.fillWidth: true
                text: splash.bootProgress < 22 ? "INITIALIZING APPLICATION CORE" :
                      splash.bootProgress < 47 ? "LOADING TRANSPORT SERVICES" :
                      splash.bootProgress < 76 ? "STARTING TELEMETRY PIPELINE" :
                      splash.bootProgress < 96 ? "VERIFYING WORKSPACE" :
                                                 "READY"
                color: "#b7c2ca"
                font.pixelSize: 10
                font.weight: Font.DemiBold
            }

            Item { Layout.preferredHeight: 14 }

            Rectangle {
                id: track
                Layout.fillWidth: true
                Layout.preferredHeight: 5
                color: "#172129"
                border.color: "#24313a"
                border.width: 1

                Rectangle {
                    width: Math.max(2, track.width * splash.bootProgress / 100.0)
                    height: parent.height
                    color: splash.bootProgress >= 96 ? "#43b581" : "#2fa4dc"
                }

                Rectangle {
                    width: 22
                    height: 1
                    y: 2
                    x: Math.max(0, Math.min(track.width - width,
                                           track.width * splash.bootProgress / 100.0 - width / 2))
                    color: "#d4f3ff"
                    opacity: 0.85
                }
            }

            Item { Layout.preferredHeight: 10 }

            RowLayout {
                Layout.fillWidth: true
                Label {
                    text: Math.round(splash.bootProgress) + "%"
                    color: "#6f808c"
                    font.pixelSize: 9
                    font.family: "monospace"
                }

                Item { Layout.fillWidth: true }

                Row {
                    spacing: 5
                    Repeater {
                        model: 5
                        Rectangle {
                            width: 4
                            height: 4
                            radius: 2
                            color: index <= Math.floor(splash.bootProgress / 20)
                                   ? "#2fa4dc" : "#33414b"
                        }
                    }
                }
            }

            Item { Layout.fillHeight: true }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 1
                color: "#202c34"
            }

            Item { Layout.preferredHeight: 15 }

            RowLayout {
                Layout.fillWidth: true

                Label {
                    text: "C++17  /  Qt 6  /  ASYNCHRONOUS I/O"
                    color: "#596a76"
                    font.pixelSize: 8
                    font.family: "monospace"
                }

                Item { Layout.fillWidth: true }

                Label {
                    text: "Designed by Amirhossein Haghi"
                    color: "#7e909c"
                    font.pixelSize: 9
                }
            }
        }
    }

    SequentialAnimation {
        id: bootSequence

        NumberAnimation {
            target: splash
            property: "bootProgress"
            from: 0
            to: 24
            duration: 360
            easing.type: Easing.OutCubic
        }
        PauseAnimation { duration: 90 }
        NumberAnimation {
            target: splash
            property: "bootProgress"
            to: 51
            duration: 360
            easing.type: Easing.InOutCubic
        }
        PauseAnimation { duration: 70 }
        NumberAnimation {
            target: splash
            property: "bootProgress"
            to: 79
            duration: 330
            easing.type: Easing.InOutCubic
        }
        NumberAnimation {
            target: splash
            property: "bootProgress"
            to: 100
            duration: 300
            easing.type: Easing.OutCubic
        }
        PauseAnimation { duration: 180 }
        ScriptAction {
            script: {
                splash.finished()
                splash.close()
            }
        }
    }

    Component.onCompleted: bootSequence.start()
}