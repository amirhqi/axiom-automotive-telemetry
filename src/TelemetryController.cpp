#include "TelemetryController.h"

#include "TelemetryWorker.h"

#include <QDateTime>
#include <QDir>
#include <QMetaObject>
#include <QSerialPortInfo>
#include <QStandardPaths>

namespace {
constexpr int MaxRawPackets = 200;
constexpr int MaxEventEntries = 120;
constexpr int MaxChartPoints = 120;
}

TelemetryController::TelemetryController(QObject *parent)
    : QObject(parent),
      m_worker(new TelemetryWorker)
{
    qRegisterMetaType<TelemetryFrame>("TelemetryFrame");

    QString logDirectory = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    if (logDirectory.isEmpty())
        logDirectory = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    m_defaultLogPath = QDir(logDirectory).filePath(
        QStringLiteral("automotive_telemetry_%1.csv")
            .arg(QDateTime::currentDateTime().toString(QStringLiteral("yyyyMMdd_HHmmss"))));

    m_worker->moveToThread(&m_workerThread);
    connect(&m_workerThread, &QThread::finished, m_worker, &QObject::deleteLater);

    connect(this, &TelemetryController::requestConnectTcp,
            m_worker, &TelemetryWorker::connectTcp, Qt::QueuedConnection);
    connect(this, &TelemetryController::requestConnectUdp,
            m_worker, &TelemetryWorker::connectUdp, Qt::QueuedConnection);
    connect(this, &TelemetryController::requestConnectSerial,
            m_worker, &TelemetryWorker::connectSerial, Qt::QueuedConnection);
    connect(this, &TelemetryController::requestDisconnect,
            m_worker, &TelemetryWorker::disconnectTransport, Qt::QueuedConnection);
    connect(this, &TelemetryController::requestSetAutoReconnect,
            m_worker, &TelemetryWorker::setAutoReconnect, Qt::QueuedConnection);

    connect(m_worker, &TelemetryWorker::connected, this, [this](const QString &transportName) {
        m_status = QStringLiteral("Connected");
        m_currentTransport = transportName;
        m_lastError.clear();
        appendEvent(QStringLiteral("Connected: %1").arg(transportName));
        emit statusChanged();
        emit currentTransportChanged();
        emit lastErrorChanged();
        emit statisticsChanged();
    });

    connect(m_worker, &TelemetryWorker::disconnected, this, [this] {
        if (m_status != QStringLiteral("Disconnected"))
            appendEvent(QStringLiteral("Transport disconnected"));
    });

    connect(m_worker, &TelemetryWorker::connectionStateChanged, this, [this](const QString &state) {
        if (m_status == state)
            return;
        m_status = state;
        emit statusChanged();
    });

    connect(m_worker, &TelemetryWorker::rawPacketReceived, this, [this](const QString &packet) {
        m_lastPacket = packet;
        appendRawPacket(packet);
        emit lastPacketChanged();
    });

    connect(m_worker, &TelemetryWorker::telemetryReceived, this, [this](const TelemetryFrame &frame) {
        m_frame = frame;
        ++m_validPackets;
        appendChartPoint(frame);

        if (m_logger.isActive()) {
            QString logError;
            if (!m_logger.writeFrame(frame, m_currentTransport, &logError))
                setLastError(QStringLiteral("Logging: %1").arg(logError));
        }

        emit telemetryChanged();
        emit statisticsChanged();
        emit chartDataChanged();
    });

    connect(m_worker, &TelemetryWorker::packetRejected,
            this, [this](const QString &reason, const QString &packet) {
                ++m_rejectedPackets;
                setLastError(QStringLiteral("Packet rejected: %1 | %2").arg(reason, packet));
                appendEvent(QStringLiteral("Rejected packet: %1").arg(reason));
                emit statisticsChanged();
            });

    connect(m_worker, &TelemetryWorker::bytesReceived, this, [this](qint64 count) {
        if (count > 0)
            m_bytesReceived += static_cast<qulonglong>(count);
        emit statisticsChanged();
    });

    connect(m_worker, &TelemetryWorker::errorOccurred, this, [this](const QString &message) {
        setLastError(message);
        appendEvent(QStringLiteral("I/O error: %1").arg(message));
    });

    connect(m_worker, &TelemetryWorker::reconnectScheduled,
            this, [this](int attempt, int delayMs) {
                ++m_reconnectAttempts;
                appendEvent(QStringLiteral("Reconnect attempt %1 scheduled in %2 ms")
                                .arg(attempt)
                                .arg(delayMs));
                emit statisticsChanged();
            });

    m_rateTimer.setInterval(1000);
    connect(&m_rateTimer, &QTimer::timeout, this, [this] {
        const qulonglong current = totalPackets();
        m_packetRate = static_cast<double>(current - m_packetsAtLastRateSample);
        m_packetsAtLastRateSample = current;
        emit statisticsChanged();
    });

    m_workerThread.start();
    m_rateTimer.start();
    refreshSerialPorts();
    emit requestSetAutoReconnect(m_autoReconnect);
    appendEvent(QStringLiteral("Application started"));
}

TelemetryController::~TelemetryController()
{
    m_logger.stop();
    if (m_workerThread.isRunning() && m_worker) {
        QMetaObject::invokeMethod(m_worker, "disconnectTransport", Qt::BlockingQueuedConnection);
        m_workerThread.quit();
        m_workerThread.wait();
    }
}

void TelemetryController::connectTcp(const QString &host, int port)
{
    const QString trimmedHost = host.trimmed();
    if (trimmedHost.isEmpty() || port <= 0 || port > 65535) {
        setLastError(QStringLiteral("Invalid TCP host or port"));
        return;
    }

    m_status = QStringLiteral("Connecting TCP...");
    m_currentTransport = QStringLiteral("TCP %1:%2").arg(trimmedHost).arg(port);
    appendEvent(QStringLiteral("Connecting to %1").arg(m_currentTransport));
    emit statusChanged();
    emit currentTransportChanged();
    emit statisticsChanged();
    emit requestConnectTcp(trimmedHost, static_cast<quint16>(port));
}

void TelemetryController::connectUdp(const QString &bindAddress, int port)
{
    const QString trimmedAddress = bindAddress.trimmed().isEmpty() ? QStringLiteral("0.0.0.0") : bindAddress.trimmed();
    if (port <= 0 || port > 65535) {
        setLastError(QStringLiteral("Invalid UDP bind port"));
        return;
    }

    m_status = QStringLiteral("Binding UDP...");
    m_currentTransport = QStringLiteral("UDP %1:%2").arg(trimmedAddress).arg(port);
    appendEvent(QStringLiteral("Binding %1").arg(m_currentTransport));
    emit statusChanged();
    emit currentTransportChanged();
    emit statisticsChanged();
    emit requestConnectUdp(trimmedAddress, static_cast<quint16>(port));
}

void TelemetryController::connectSerial(const QString &portName, int baudRate)
{
    const QString trimmedPort = portName.trimmed();
    if (trimmedPort.isEmpty() || baudRate <= 0) {
        setLastError(QStringLiteral("Invalid serial port or baud rate"));
        return;
    }

    m_status = QStringLiteral("Opening serial port...");
    m_currentTransport = QStringLiteral("Serial %1 @ %2").arg(trimmedPort).arg(baudRate);
    appendEvent(QStringLiteral("Opening %1").arg(m_currentTransport));
    emit statusChanged();
    emit currentTransportChanged();
    emit statisticsChanged();
    emit requestConnectSerial(trimmedPort, static_cast<qint32>(baudRate));
}

void TelemetryController::disconnectTransport()
{
    appendEvent(QStringLiteral("Manual disconnect requested"));
    m_status = QStringLiteral("Disconnected");
    emit statusChanged();
    emit statisticsChanged();
    emit requestDisconnect();
}

void TelemetryController::setAutoReconnect(bool enabled)
{
    if (m_autoReconnect == enabled)
        return;

    m_autoReconnect = enabled;
    appendEvent(enabled ? QStringLiteral("Auto reconnect enabled")
                        : QStringLiteral("Auto reconnect disabled"));
    emit autoReconnectChanged();
    emit requestSetAutoReconnect(enabled);
}

void TelemetryController::refreshSerialPorts()
{
    QStringList ports;
    const QList<QSerialPortInfo> available = QSerialPortInfo::availablePorts();
    ports.reserve(available.size());
    for (const QSerialPortInfo &info : available)
        ports.append(info.portName());

    ports.sort(Qt::CaseInsensitive);
    if (ports == m_serialPorts)
        return;

    m_serialPorts = ports;
    emit serialPortsChanged();
}

void TelemetryController::clearHistory()
{
    m_rawPackets.clear();
    m_eventLog.clear();
    m_rpmSeries.clear();
    m_speedSeries.clear();
    m_temperatureSeries.clear();
    m_batterySeries.clear();
    m_lastPacket.clear();
    m_lastError.clear();
    m_validPackets = 0;
    m_rejectedPackets = 0;
    m_bytesReceived = 0;
    m_packetsAtLastRateSample = 0;
    m_packetRate = 0.0;
    m_reconnectAttempts = 0;

    appendEvent(QStringLiteral("Session history cleared"));
    emit rawPacketsChanged();
    emit chartDataChanged();
    emit lastPacketChanged();
    emit lastErrorChanged();
    emit statisticsChanged();
}

bool TelemetryController::startLogging(const QString &path)
{
    const QString target = path.trimmed().isEmpty() ? m_defaultLogPath : path.trimmed();
    QString error;
    if (!m_logger.start(target, &error)) {
        setLastError(QStringLiteral("Could not start CSV logging: %1").arg(error));
        return false;
    }

    appendEvent(QStringLiteral("CSV logging started: %1").arg(m_logger.path()));
    emit loggingChanged();
    return true;
}

void TelemetryController::stopLogging()
{
    if (!m_logger.isActive())
        return;

    const QString oldPath = m_logger.path();
    m_logger.stop();
    appendEvent(QStringLiteral("CSV logging stopped: %1").arg(oldPath));
    emit loggingChanged();
}

void TelemetryController::appendEvent(const QString &message)
{
    const QString timestamp = QDateTime::currentDateTime().toString(QStringLiteral("HH:mm:ss.zzz"));
    m_eventLog.append(QStringLiteral("[%1] %2").arg(timestamp, message));
    while (m_eventLog.size() > MaxEventEntries)
        m_eventLog.removeFirst();
    emit eventLogChanged();
}

void TelemetryController::appendRawPacket(const QString &packet)
{
    const QString timestamp = QDateTime::currentDateTime().toString(QStringLiteral("HH:mm:ss.zzz"));
    m_rawPackets.append(QStringLiteral("[%1] %2").arg(timestamp, packet));
    while (m_rawPackets.size() > MaxRawPackets)
        m_rawPackets.removeFirst();
    emit rawPacketsChanged();
}

void TelemetryController::appendChartPoint(const TelemetryFrame &frame)
{
    appendBounded(m_rpmSeries, frame.rpm, MaxChartPoints);
    appendBounded(m_speedSeries, frame.speed, MaxChartPoints);
    appendBounded(m_temperatureSeries, frame.temperature, MaxChartPoints);
    appendBounded(m_batterySeries, frame.battery, MaxChartPoints);
}

void TelemetryController::setLastError(const QString &message)
{
    m_lastError = message;
    emit lastErrorChanged();
}

void TelemetryController::appendBounded(QVariantList &series, double value, int maxSize)
{
    series.append(value);
    while (series.size() > maxSize)
        series.removeFirst();
}
