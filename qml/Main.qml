import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import AutomotiveTelemetry 1.0
import "components"

ApplicationWindow {
    id: root
    width: 1500
    height: 900
    minimumWidth: 1080
    minimumHeight: 680
    visible: false
    title: "AXIOM Automotive Telemetry Workbench - v2.6.1"
    color: "#0f1215"

    readonly property color cBg: "#0f1215"
    readonly property color cChrome: "#11161b"
    readonly property color cPanel: "#171b20"
    readonly property color cPanel2: "#1a1f25"
    readonly property color cBorder: "#353c45"
    readonly property color cBorderSoft: "#2b323a"
    readonly property color cText: "#d8dde3"
    readonly property color cMuted: "#7f8b98"
    readonly property color cAccent: "#3297d3"
    readonly property color cGood: "#43b581"
    readonly property color cWarn: "#d9a441"
    readonly property color cBad: "#d05b5b"
    readonly property bool connected: Telemetry.status === "Connected"


    StartupSplash {
        id: startupSplash
        onFinished: {
            root.visible = true
            root.raise()
            root.requestActivate()
        }
    }
    palette.window: cBg
    palette.windowText: cText
    palette.base: "#12171c"
    palette.alternateBase: cPanel
    palette.text: cText
    palette.button: "#22282f"
    palette.buttonText: cText
    palette.highlight: cAccent
    palette.highlightedText: "#ffffff"
    palette.mid: cBorder
    palette.dark: "#0b0e11"
    palette.light: "#48515b"
    palette.placeholderText: "#68737f"

    component NavButton: Button {
        id: control
        property bool active: false
        leftPadding: 14
        rightPadding: 10
        topPadding: 0
        bottomPadding: 0
        implicitHeight: 38
        flat: true

        contentItem: Label {
            text: control.text
            color: control.active ? "#f2f5f7" : "#96a1ad"
            font.pixelSize: 10
            font.weight: control.active ? Font.DemiBold : Font.Normal
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignLeft
        }

        background: Rectangle {
            color: control.active ? "#1c252d" : (control.hovered ? "#181e24" : "transparent")
            border.color: control.active ? "#33424f" : "transparent"
            border.width: 1

            Rectangle {
                width: 3
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                color: control.active ? root.cAccent : "transparent"
            }
        }
    }

    component ToolButtonCompact: Button {
        id: control
        implicitHeight: 30
        leftPadding: 12
        rightPadding: 12
        font.pixelSize: 10
        background: Rectangle {
            radius: 2
            color: control.down ? "#2d3741" : (control.hovered ? "#29323a" : "#22282f")
            border.color: control.highlighted ? root.cAccent : "#414a54"
            border.width: 1
        }
        contentItem: Label {
            text: control.text
            color: control.enabled ? "#dce2e8" : "#626c77"
            font: control.font
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }

    header: Rectangle {
        implicitHeight: 42
        color: "#0b0f12"
        border.color: "#2a3037"
        border.width: 1

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 14
            anchors.rightMargin: 14
            spacing: 12

            Rectangle {
                width: 3
                height: 18
                color: root.cAccent
            }

            Label {
                text: "LIVE ACQUISITION"
                color: "#b8c2cc"
                font.pixelSize: 10
                font.weight: Font.DemiBold
            }

            Rectangle {
                width: 1
                height: 16
                color: "#303841"
            }

            Label {
                text: "TELEMETRY / TRACE / DIAGNOSTICS"
                color: "#65717d"
                font.pixelSize: 8
            }

            Item { Layout.fillWidth: true }

            Rectangle {
                implicitWidth: Math.max(112, connectionStatus.implicitWidth + 28)
                implicitHeight: 24
                radius: 2
                color: root.connected ? "#16261f" : "#241a1b"
                border.color: root.connected ? "#326b51" : "#604044"

                Row {
                    anchors.centerIn: parent
                    spacing: 7
                    Rectangle {
                        width: 7
                        height: 7
                        radius: 4
                        color: root.connected ? root.cGood : root.cBad
                    }
                    Label {
                        id: connectionStatus
                        text: Telemetry.status.toUpperCase()
                        color: root.connected ? "#8bd1ad" : "#d99494"
                        font.pixelSize: 9
                        font.weight: Font.DemiBold
                    }
                }
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 48
            color: root.cChrome
            border.color: root.cBorderSoft
            border.width: 1

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 12
                anchors.rightMargin: 12
                spacing: 8

                Label {
                    text: "INTERFACE"
                    color: root.cMuted
                    font.pixelSize: 8
                    font.weight: Font.DemiBold
                }

                ComboBox {
                    id: transportBox
                    model: ["TCP", "UDP", "Serial"]
                    Layout.preferredWidth: 94
                    Layout.preferredHeight: 30
                    font.pixelSize: 10
                    onCurrentIndexChanged: {
                        if (currentIndex === 2)
                            Telemetry.refreshSerialPorts()
                        else if (currentIndex === 1) {
                            hostField.text = "127.0.0.1"
                            portField.value = 5001
                        } else {
                            hostField.text = "127.0.0.1"
                            portField.value = 5000
                        }
                    }
                }

                Label {
                    visible: transportBox.currentIndex < 2
                    text: transportBox.currentIndex === 1 ? "BIND" : "HOST"
                    color: root.cMuted
                    font.pixelSize: 8
                }

                TextField {
                    id: hostField
                    visible: transportBox.currentIndex < 2
                    text: "127.0.0.1"
                    Layout.preferredWidth: 145
                    Layout.preferredHeight: 30
                    font.pixelSize: 10
                    selectByMouse: true
                }

                Label {
                    visible: transportBox.currentIndex < 2
                    text: "PORT"
                    color: root.cMuted
                    font.pixelSize: 8
                }

                SpinBox {
                    id: portField
                    visible: transportBox.currentIndex < 2
                    from: 1
                    to: 65535
                    value: 5000
                    editable: true
                    Layout.preferredWidth: 96
                    Layout.preferredHeight: 30
                    font.pixelSize: 10
                }

                ComboBox {
                    id: serialPortBox
                    visible: transportBox.currentIndex === 2
                    model: Telemetry.serialPorts
                    displayText: count > 0 ? currentText : "No ports"
                    Layout.preferredWidth: 132
                    Layout.preferredHeight: 30
                    font.pixelSize: 10
                }

                ToolButtonCompact {
                    visible: transportBox.currentIndex === 2
                    text: "Refresh"
                    onClicked: Telemetry.refreshSerialPorts()
                }

                ComboBox {
                    id: baudBox
                    visible: transportBox.currentIndex === 2
                    model: [9600, 19200, 38400, 57600, 115200, 230400]
                    currentIndex: 4
                    Layout.preferredWidth: 112
                    Layout.preferredHeight: 30
                    font.pixelSize: 10
                }

                Rectangle { width: 1; height: 24; color: "#343b44" }

                ToolButtonCompact {
                    text: "Connect"
                    highlighted: true
                    onClicked: {
                        if (transportBox.currentIndex === 0)
                            Telemetry.connectTcp(hostField.text, portField.value)
                        else if (transportBox.currentIndex === 1)
                            Telemetry.connectUdp(hostField.text, portField.value)
                        else
                            Telemetry.connectSerial(serialPortBox.currentText, Number(baudBox.currentText))
                    }
                }

                ToolButtonCompact {
                    text: "Disconnect"
                    onClicked: Telemetry.disconnectTransport()
                }

                CheckBox {
                    visible: transportBox.currentIndex !== 1
                    text: "Auto reconnect"
                    checked: Telemetry.autoReconnect
                    font.pixelSize: 9
                    onToggled: Telemetry.autoReconnect = checked
                }

                Item { Layout.fillWidth: true }

                ColumnLayout {
                    spacing: 0
                    Label {
                        text: "TRANSPORT"
                        color: root.cMuted
                        font.pixelSize: 8
                    }
                    Label {
                        text: Telemetry.currentTransport
                        color: "#c8d0d8"
                        font.pixelSize: 9
                        font.family: "monospace"
                    }
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            Rectangle {
                Layout.preferredWidth: 174
                Layout.fillHeight: true
                color: "#11161b"
                border.color: root.cBorderSoft
                border.width: 1

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 0

                    Label {
                        Layout.fillWidth: true
                        Layout.leftMargin: 14
                        Layout.topMargin: 14
                        Layout.bottomMargin: 8
                        text: "WORKSPACE"
                        color: "#65717d"
                        font.pixelSize: 8
                        font.weight: Font.DemiBold
                    }

                    NavButton {
                        Layout.fillWidth: true
                        text: "LIVE MONITOR"
                        active: workspace.currentIndex === 0
                        onClicked: workspace.currentIndex = 0
                    }
                    NavButton {
                        Layout.fillWidth: true
                        text: "TRACE"
                        active: workspace.currentIndex === 1
                        onClicked: workspace.currentIndex = 1
                    }
                    NavButton {
                        Layout.fillWidth: true
                        text: "DIAGNOSTICS"
                        active: workspace.currentIndex === 2
                        onClicked: workspace.currentIndex = 2
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.topMargin: 12
                        height: 1
                        color: root.cBorderSoft
                    }

                    Label {
                        Layout.fillWidth: true
                        Layout.leftMargin: 14
                        Layout.topMargin: 12
                        Layout.bottomMargin: 8
                        text: "SESSION"
                        color: "#65717d"
                        font.pixelSize: 8
                        font.weight: Font.DemiBold
                    }

                    GridLayout {
                        Layout.fillWidth: true
                        Layout.leftMargin: 14
                        Layout.rightMargin: 14
                        columns: 2
                        columnSpacing: 6
                        rowSpacing: 7

                        Label { text: "Rate"; color: root.cMuted; font.pixelSize: 9 }
                        Label { text: Telemetry.packetRate.toFixed(0) + " pkt/s"; color: root.cText; font.pixelSize: 9; font.family: "monospace"; Layout.alignment: Qt.AlignRight }
                        Label { text: "Valid"; color: root.cMuted; font.pixelSize: 9 }
                        Label { text: Telemetry.validPackets.toString(); color: root.cGood; font.pixelSize: 9; font.family: "monospace"; Layout.alignment: Qt.AlignRight }
                        Label { text: "Rejected"; color: root.cMuted; font.pixelSize: 9 }
                        Label { text: Telemetry.rejectedPackets.toString(); color: Telemetry.rejectedPackets > 0 ? root.cBad : root.cText; font.pixelSize: 9; font.family: "monospace"; Layout.alignment: Qt.AlignRight }
                        Label { text: "RX"; color: root.cMuted; font.pixelSize: 9 }
                        Label { text: Telemetry.bytesReceived.toString() + " B"; color: root.cText; font.pixelSize: 9; font.family: "monospace"; Layout.alignment: Qt.AlignRight }
                    }

                    Item { Layout.fillHeight: true }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.margins: 10
                        implicitHeight: systemInfo.implicitHeight + 18
                        color: "#0d1115"
                        border.color: "#2c333b"
                        border.width: 1

                        ColumnLayout {
                            id: systemInfo
                            anchors.fill: parent
                            anchors.margins: 9
                            spacing: 4
                            Label { text: "RUNTIME"; color: "#65717d"; font.pixelSize: 8; font.weight: Font.DemiBold }
                            Label { text: "Qt 6 / C++17"; color: "#9ca7b2"; font.pixelSize: 9 }
                            Label { text: "Worker I/O thread"; color: "#9ca7b2"; font.pixelSize: 9 }
                            Label { text: "TCP / UDP / Serial"; color: "#9ca7b2"; font.pixelSize: 9 }
                        }
                    }
                }
            }

            StackLayout {
                id: workspace
                Layout.fillWidth: true
                Layout.fillHeight: true
                currentIndex: 0

                ScrollView {
                    id: liveScroll
                    clip: true
                    contentWidth: availableWidth
                    contentHeight: liveColumn.implicitHeight + 20
                    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

                    ColumnLayout {
                        id: liveColumn
                        width: Math.max(0, liveScroll.availableWidth)
                        spacing: 10

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 42
                            color: root.cPanel2
                            border.color: root.cBorder
                            border.width: 1

                            RowLayout {
                                anchors.fill: parent
                                anchors.leftMargin: 12
                                anchors.rightMargin: 12
                                spacing: 10
                                Label { text: "MEASUREMENT / LIVE CHANNELS"; color: root.cText; font.pixelSize: 10; font.weight: Font.DemiBold }
                                Item { Layout.fillWidth: true }
                                Label { text: "History: 120 samples"; color: root.cMuted; font.pixelSize: 9 }
                                Label { text: "|"; color: "#434b54"; font.pixelSize: 9 }
                                Label { text: "Frames: " + Telemetry.totalPackets; color: root.cMuted; font.pixelSize: 9 }
                            }
                        }

                        GridLayout {
                            Layout.fillWidth: true
                            Layout.leftMargin: 10
                            Layout.rightMargin: 10
                            columns: liveScroll.availableWidth >= 1060 ? 4 : (liveScroll.availableWidth >= 560 ? 2 : 1)
                            columnSpacing: 8
                            rowSpacing: 8

                            MetricCard { label: "ENGINE SPEED"; value: Telemetry.rpm.toString(); unit: "rpm"; subtitle: "Validated frame"; accent: "#3a9ad9"; Layout.fillWidth: true; Layout.preferredHeight: 90 }
                            MetricCard { label: "VEHICLE SPEED"; value: Telemetry.speed.toString(); unit: "km/h"; subtitle: "Range 0..350"; accent: "#4aa5d8"; Layout.fillWidth: true; Layout.preferredHeight: 90 }
                            MetricCard { label: "COOLANT TEMP"; value: Telemetry.temperature.toFixed(1); unit: "deg C"; subtitle: "Range -50..180"; accent: Telemetry.temperature > 105 ? root.cWarn : "#6aa6c8"; Layout.fillWidth: true; Layout.preferredHeight: 90 }
                            MetricCard { label: "BATTERY VOLTAGE"; value: Telemetry.battery.toFixed(2); unit: "V"; subtitle: "Range 0..30"; accent: Telemetry.battery > 0 && Telemetry.battery < 11.8 ? root.cWarn : "#5da987"; Layout.fillWidth: true; Layout.preferredHeight: 90 }
                        }

                        GridLayout {
                            Layout.fillWidth: true
                            Layout.leftMargin: 10
                            Layout.rightMargin: 10
                            Layout.bottomMargin: 10
                            columns: liveScroll.availableWidth >= 760 ? 2 : 1
                            columnSpacing: 8
                            rowSpacing: 8

                            TrendChart { title: "Engine Speed"; unit: "rpm"; values: Telemetry.rpmSeries; minValue: 0; maxValue: 6000; traceColor: "#3a9ad9"; Layout.fillWidth: true; Layout.preferredHeight: 235 }
                            TrendChart { title: "Vehicle Speed"; unit: "km/h"; values: Telemetry.speedSeries; minValue: 0; maxValue: 180; traceColor: "#65b1d7"; Layout.fillWidth: true; Layout.preferredHeight: 235 }
                            TrendChart { title: "Coolant Temperature"; unit: "deg C"; values: Telemetry.temperatureSeries; minValue: 50; maxValue: 130; traceColor: "#d3a349"; Layout.fillWidth: true; Layout.preferredHeight: 235 }
                            TrendChart { title: "Battery Voltage"; unit: "V"; values: Telemetry.batterySeries; minValue: 10; maxValue: 16; traceColor: "#61b18c"; Layout.fillWidth: true; Layout.preferredHeight: 235 }
                        }
                    }
                }

                SplitView {
                    orientation: Qt.Horizontal

                    Rectangle {
                        SplitView.fillHeight: true
                        SplitView.preferredWidth: 840
                        color: "#101418"
                        border.color: root.cBorder
                        border.width: 1

                        ColumnLayout {
                            anchors.fill: parent
                            spacing: 0

                            Rectangle {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 34
                                color: root.cPanel2
                                border.color: root.cBorder
                                border.width: 1
                                RowLayout {
                                    anchors.fill: parent
                                    anchors.leftMargin: 10
                                    anchors.rightMargin: 10
                                    Label { text: "RAW FRAME STREAM"; color: root.cText; font.pixelSize: 10; font.weight: Font.DemiBold }
                                    Item { Layout.fillWidth: true }
                                    Label { text: Telemetry.rawPackets.length + " buffered"; color: root.cMuted; font.pixelSize: 9 }
                                }
                            }

                            ScrollView {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                TextArea {
                                    readOnly: true
                                    selectByMouse: true
                                    text: Telemetry.rawPackets.join("\n")
                                    color: "#b8c6d3"
                                    selectionColor: "#284d64"
                                    font.family: "monospace"
                                    font.pixelSize: 10
                                    wrapMode: TextEdit.NoWrap
                                    leftPadding: 10
                                    rightPadding: 10
                                    topPadding: 8
                                    bottomPadding: 8
                                    background: Rectangle { color: "#0b0f12" }
                                }
                            }
                        }
                    }

                    Rectangle {
                        SplitView.fillHeight: true
                        SplitView.fillWidth: true
                        color: "#101418"
                        border.color: root.cBorder
                        border.width: 1

                        ColumnLayout {
                            anchors.fill: parent
                            spacing: 0

                            Rectangle {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 34
                                color: root.cPanel2
                                border.color: root.cBorder
                                border.width: 1
                                RowLayout {
                                    anchors.fill: parent
                                    anchors.leftMargin: 10
                                    anchors.rightMargin: 10
                                    Label { text: "EVENT / ERROR LOG"; color: root.cText; font.pixelSize: 10; font.weight: Font.DemiBold }
                                    Item { Layout.fillWidth: true }
                                    ToolButtonCompact { text: "Clear"; implicitHeight: 24; onClicked: Telemetry.clearHistory() }
                                }
                            }

                            ScrollView {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                TextArea {
                                    readOnly: true
                                    selectByMouse: true
                                    text: Telemetry.eventLog.join("\n")
                                    color: "#aeb9c3"
                                    font.family: "monospace"
                                    font.pixelSize: 10
                                    wrapMode: TextEdit.Wrap
                                    leftPadding: 10
                                    rightPadding: 10
                                    topPadding: 8
                                    bottomPadding: 8
                                    background: Rectangle { color: "#0b0f12" }
                                }
                            }
                        }
                    }
                }

                ScrollView {
                    id: diagScroll
                    clip: true
                    contentWidth: availableWidth
                    contentHeight: diagColumn.implicitHeight + 20
                    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

                    ColumnLayout {
                        id: diagColumn
                        width: Math.max(0, diagScroll.availableWidth)
                        spacing: 10

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 42
                            color: root.cPanel2
                            border.color: root.cBorder
                            border.width: 1
                            RowLayout {
                                anchors.fill: parent
                                anchors.leftMargin: 12
                                anchors.rightMargin: 12
                                Label { text: "SYSTEM DIAGNOSTICS"; color: root.cText; font.pixelSize: 10; font.weight: Font.DemiBold }
                                Item { Layout.fillWidth: true }
                                Label { text: Telemetry.lastError.length > 0 ? "FAULT PRESENT" : "NO ACTIVE FAULT"; color: Telemetry.lastError.length > 0 ? root.cBad : root.cGood; font.pixelSize: 9; font.weight: Font.DemiBold }
                            }
                        }

                        GridLayout {
                            Layout.fillWidth: true
                            Layout.leftMargin: 10
                            Layout.rightMargin: 10
                            columns: diagScroll.availableWidth >= 1050 ? 3 : (diagScroll.availableWidth >= 620 ? 2 : 1)
                            columnSpacing: 8
                            rowSpacing: 8

                            MetricCard { label: "TOTAL FRAMES"; value: Telemetry.totalPackets.toString(); subtitle: "Accepted + rejected"; accent: root.cAccent; Layout.fillWidth: true; Layout.preferredHeight: 90 }
                            MetricCard { label: "VALID FRAMES"; value: Telemetry.validPackets.toString(); subtitle: "Parser accepted"; accent: root.cGood; Layout.fillWidth: true; Layout.preferredHeight: 90 }
                            MetricCard { label: "REJECTED FRAMES"; value: Telemetry.rejectedPackets.toString(); subtitle: "Validation failures"; accent: Telemetry.rejectedPackets > 0 ? root.cBad : "#6e7985"; Layout.fillWidth: true; Layout.preferredHeight: 90 }
                            MetricCard { label: "PACKET RATE"; value: Telemetry.packetRate.toFixed(0); unit: "pkt/s"; subtitle: "Current receive rate"; accent: "#569fc7"; Layout.fillWidth: true; Layout.preferredHeight: 90 }
                            MetricCard { label: "RX BYTES"; value: Telemetry.bytesReceived.toString(); unit: "B"; subtitle: "Session total"; accent: "#569fc7"; Layout.fillWidth: true; Layout.preferredHeight: 90 }
                            MetricCard { label: "RECONNECTS"; value: Telemetry.reconnectAttempts.toString(); subtitle: "Automatic recovery"; accent: Telemetry.reconnectAttempts > 0 ? root.cWarn : "#6e7985"; Layout.fillWidth: true; Layout.preferredHeight: 90 }
                        }

                        SplitView {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 260
                            Layout.leftMargin: 10
                            Layout.rightMargin: 10
                            orientation: Qt.Horizontal

                            Rectangle {
                                SplitView.preferredWidth: 720
                                SplitView.fillHeight: true
                                color: root.cPanel
                                border.color: root.cBorder
                                border.width: 1

                                ColumnLayout {
                                    anchors.fill: parent
                                    anchors.margins: 12
                                    spacing: 8

                                    Label { text: "CSV SESSION LOGGER"; color: root.cText; font.pixelSize: 10; font.weight: Font.DemiBold }
                                    Label { text: "Record validated telemetry frames for offline analysis."; color: root.cMuted; font.pixelSize: 9 }

                                    TextField {
                                        id: logPathField
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: 30
                                        text: Telemetry.defaultLogPath
                                        selectByMouse: true
                                        font.pixelSize: 9
                                    }

                                    RowLayout {
                                        Layout.fillWidth: true
                                        ToolButtonCompact { text: "Start logging"; enabled: !Telemetry.loggingActive; onClicked: Telemetry.startLogging(logPathField.text) }
                                        ToolButtonCompact { text: "Stop"; enabled: Telemetry.loggingActive; onClicked: Telemetry.stopLogging() }
                                        Item { Layout.fillWidth: true }
                                    }

                                    Label {
                                        Layout.fillWidth: true
                                        text: Telemetry.loggingActive ? "ACTIVE: " + Telemetry.logPath : "LOGGER IDLE"
                                        color: Telemetry.loggingActive ? root.cGood : root.cMuted
                                        font.pixelSize: 9
                                        font.family: "monospace"
                                        elide: Text.ElideMiddle
                                    }

                                    Item { Layout.fillHeight: true }
                                }
                            }

                            Rectangle {
                                SplitView.fillWidth: true
                                SplitView.fillHeight: true
                                color: root.cPanel
                                border.color: root.cBorder
                                border.width: 1

                                ColumnLayout {
                                    anchors.fill: parent
                                    anchors.margins: 12
                                    spacing: 8

                                    RowLayout {
                                        Layout.fillWidth: true
                                        Label { text: "SESSION HEALTH"; color: root.cText; font.pixelSize: 10; font.weight: Font.DemiBold }
                                        Item { Layout.fillWidth: true }
                                        ToolButtonCompact { text: "Clear session"; onClicked: Telemetry.clearHistory() }
                                    }

                                    Rectangle { Layout.fillWidth: true; height: 1; color: root.cBorderSoft }

                                    Label { text: "Last error"; color: root.cMuted; font.pixelSize: 8 }
                                    Label {
                                        Layout.fillWidth: true
                                        text: Telemetry.lastError.length > 0 ? Telemetry.lastError : "No current errors"
                                        color: Telemetry.lastError.length > 0 ? root.cBad : root.cGood
                                        font.pixelSize: 9
                                        wrapMode: Text.Wrap
                                    }

                                    Label { text: "Last frame"; color: root.cMuted; font.pixelSize: 8 }
                                    Label {
                                        Layout.fillWidth: true
                                        text: Telemetry.lastPacket.length > 0 ? Telemetry.lastPacket : "None"
                                        color: "#9ba6b1"
                                        font.pixelSize: 9
                                        font.family: "monospace"
                                        wrapMode: Text.WrapAnywhere
                                    }

                                    Item { Layout.fillHeight: true }
                                }
                            }
                        }
                    }
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 24
            color: "#0b0f12"
            border.color: "#2a3037"
            border.width: 1

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 10
                anchors.rightMargin: 10
                spacing: 12

                Rectangle {
                    width: 6
                    height: 6
                    radius: 3
                    color: root.connected ? root.cGood : root.cBad
                }
                Label { text: Telemetry.status; color: "#9aa5b0"; font.pixelSize: 8 }
                Label { text: "|"; color: "#444c55"; font.pixelSize: 8 }
                Label { text: "Valid " + Telemetry.validPackets; color: "#7e8994"; font.pixelSize: 8 }
                Label { text: "Reject " + Telemetry.rejectedPackets; color: "#7e8994"; font.pixelSize: 8 }
                Label { text: "Rate " + Telemetry.packetRate.toFixed(0) + " pkt/s"; color: "#7e8994"; font.pixelSize: 8 }
                Item { Layout.fillWidth: true }
                Label { text: "Worker I/O | bounded history | automated tests"; color: "#59646f"; font.pixelSize: 8 }
            }
        }
    }
}