#include "TelemetryWorker.h"

#include "PacketParser.h"

#include <QHostAddress>
#include <QtGlobal>

namespace {
constexpr qsizetype MaxBufferedBytes = 64 * 1024;
}

TelemetryWorker::TelemetryWorker(QObject *parent)
    : QObject(parent),
      m_reconnectTimer(new QTimer(this))
{
    m_reconnectTimer->setSingleShot(true);
    connect(m_reconnectTimer, &QTimer::timeout, this, &TelemetryWorker::reconnectCurrent);
}

void TelemetryWorker::ensureTcpSocket()
{
    if (m_tcpSocket)
        return;

    m_tcpSocket = new QTcpSocket(this);
    connect(m_tcpSocket, &QTcpSocket::readyRead, this, &TelemetryWorker::onTcpReadyRead);
    connect(m_tcpSocket, &QTcpSocket::connected, this, &TelemetryWorker::onTcpConnected);
    connect(m_tcpSocket, &QTcpSocket::disconnected, this, &TelemetryWorker::onTcpDisconnected);
    connect(m_tcpSocket, &QTcpSocket::errorOccurred, this, &TelemetryWorker::onTcpError);
}

void TelemetryWorker::ensureUdpSocket()
{
    if (m_udpSocket)
        return;

    m_udpSocket = new QUdpSocket(this);
    connect(m_udpSocket, &QUdpSocket::readyRead, this, &TelemetryWorker::onUdpReadyRead);
    connect(m_udpSocket, &QUdpSocket::errorOccurred, this, &TelemetryWorker::onUdpError);
}

void TelemetryWorker::ensureSerialPort()
{
    if (m_serialPort)
        return;

    m_serialPort = new QSerialPort(this);
    connect(m_serialPort, &QSerialPort::readyRead, this, &TelemetryWorker::onSerialReadyRead);
    connect(m_serialPort, &QSerialPort::errorOccurred, this, &TelemetryWorker::onSerialError);
}

void TelemetryWorker::closeCurrentTransport()
{
    if (m_reconnectTimer)
        m_reconnectTimer->stop();

    if (m_tcpSocket && m_tcpSocket->state() != QAbstractSocket::UnconnectedState)
        m_tcpSocket->abort();

    if (m_udpSocket && m_udpSocket->state() != QAbstractSocket::UnconnectedState)
        m_udpSocket->close();

    if (m_serialPort && m_serialPort->isOpen())
        m_serialPort->close();

    m_buffer.clear();
}

void TelemetryWorker::connectTcp(const QString &host, quint16 port)
{
    m_manualDisconnect = true;
    m_transport = Transport::None;
    closeCurrentTransport();

    m_tcpHost = host;
    m_tcpPort = port;
    m_reconnectAttempts = 0;
    m_transport = Transport::Tcp;
    m_manualDisconnect = false;

    ensureTcpSocket();
    emit connectionStateChanged(QStringLiteral("Connecting TCP..."));
    m_tcpSocket->connectToHost(m_tcpHost, m_tcpPort);
}

void TelemetryWorker::connectUdp(const QString &bindAddress, quint16 port)
{
    m_manualDisconnect = true;
    m_transport = Transport::None;
    closeCurrentTransport();

    m_udpBindAddress = bindAddress.trimmed().isEmpty() ? QStringLiteral("0.0.0.0") : bindAddress.trimmed();
    m_udpPort = port;
    m_reconnectAttempts = 0;
    m_transport = Transport::Udp;
    m_manualDisconnect = false;

    ensureUdpSocket();
    const QHostAddress address(m_udpBindAddress);
    if (address.isNull()) {
        emit errorOccurred(QStringLiteral("Invalid UDP bind address: %1").arg(m_udpBindAddress));
        emit connectionStateChanged(QStringLiteral("Disconnected"));
        return;
    }

    emit connectionStateChanged(QStringLiteral("Binding UDP..."));
    if (m_udpSocket->bind(address, m_udpPort, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)) {
        emit connected(currentTransportName());
        emit connectionStateChanged(QStringLiteral("Connected"));
    } else {
        emit errorOccurred(QStringLiteral("UDP bind failed: %1").arg(m_udpSocket->errorString()));
        emit connectionStateChanged(QStringLiteral("Disconnected"));
    }
}

void TelemetryWorker::connectSerial(const QString &portName, qint32 baudRate)
{
    m_manualDisconnect = true;
    m_transport = Transport::None;
    closeCurrentTransport();

    m_serialPortName = portName;
    m_baudRate = baudRate;
    m_reconnectAttempts = 0;
    m_transport = Transport::Serial;
    m_manualDisconnect = false;

    ensureSerialPort();
    m_serialPort->setPortName(m_serialPortName);
    m_serialPort->setBaudRate(m_baudRate);
    m_serialPort->setDataBits(QSerialPort::Data8);
    m_serialPort->setParity(QSerialPort::NoParity);
    m_serialPort->setStopBits(QSerialPort::OneStop);
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);

    emit connectionStateChanged(QStringLiteral("Opening serial port..."));
    if (m_serialPort->open(QIODevice::ReadOnly)) {
        m_reconnectAttempts = 0;
        emit connected(currentTransportName());
        emit connectionStateChanged(QStringLiteral("Connected"));
    } else {
        emit errorOccurred(QStringLiteral("Serial open failed: %1").arg(m_serialPort->errorString()));
        emit connectionStateChanged(QStringLiteral("Disconnected"));
        scheduleReconnect();
    }
}

void TelemetryWorker::disconnectTransport()
{
    m_manualDisconnect = true;
    m_transport = Transport::None;
    closeCurrentTransport();
    m_reconnectAttempts = 0;
    emit disconnected();
    emit connectionStateChanged(QStringLiteral("Disconnected"));
    m_manualDisconnect = false;
}

void TelemetryWorker::setAutoReconnect(bool enabled)
{
    m_autoReconnect = enabled;
    if (!enabled && m_reconnectTimer)
        m_reconnectTimer->stop();
}

void TelemetryWorker::onTcpReadyRead()
{
    if (m_tcpSocket)
        consumeData(m_tcpSocket->readAll());
}

void TelemetryWorker::onTcpConnected()
{
    if (m_transport != Transport::Tcp)
        return;

    m_reconnectAttempts = 0;
    emit connected(currentTransportName());
    emit connectionStateChanged(QStringLiteral("Connected"));
}

void TelemetryWorker::onTcpDisconnected()
{
    if (m_transport != Transport::Tcp || m_manualDisconnect)
        return;

    emit disconnected();
    emit connectionStateChanged(QStringLiteral("Disconnected"));
    scheduleReconnect();
}

void TelemetryWorker::onTcpError(QAbstractSocket::SocketError)
{
    if (m_transport != Transport::Tcp || m_manualDisconnect)
        return;

    if (m_tcpSocket)
        emit errorOccurred(QStringLiteral("TCP: %1").arg(m_tcpSocket->errorString()));

    if (m_tcpSocket && m_tcpSocket->state() == QAbstractSocket::UnconnectedState)
        scheduleReconnect();
}

void TelemetryWorker::onUdpReadyRead()
{
    if (!m_udpSocket || m_transport != Transport::Udp)
        return;

    while (m_udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(static_cast<qsizetype>(m_udpSocket->pendingDatagramSize()));
        const qint64 received = m_udpSocket->readDatagram(datagram.data(), datagram.size());
        if (received > 0) {
            datagram.resize(static_cast<qsizetype>(received));
            emit bytesReceived(received);
            consumeDatagram(datagram);
        }
    }
}

void TelemetryWorker::onUdpError(QAbstractSocket::SocketError)
{
    if (m_transport != Transport::Udp || m_manualDisconnect || !m_udpSocket)
        return;
    emit errorOccurred(QStringLiteral("UDP: %1").arg(m_udpSocket->errorString()));
}

void TelemetryWorker::onSerialReadyRead()
{
    if (m_serialPort)
        consumeData(m_serialPort->readAll());
}

void TelemetryWorker::onSerialError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::NoError || m_transport != Transport::Serial || m_manualDisconnect)
        return;

    if (!m_serialPort)
        return;

    emit errorOccurred(QStringLiteral("Serial: %1").arg(m_serialPort->errorString()));

    const bool connectionLost = error == QSerialPort::ResourceError
                                || error == QSerialPort::DeviceNotFoundError
                                || error == QSerialPort::PermissionError;
    if (connectionLost) {
        if (m_serialPort->isOpen())
            m_serialPort->close();
        emit disconnected();
        emit connectionStateChanged(QStringLiteral("Disconnected"));
        scheduleReconnect();
    }
}

void TelemetryWorker::consumeData(const QByteArray &data)
{
    if (data.isEmpty())
        return;

    emit bytesReceived(data.size());
    m_buffer += data;

    if (m_buffer.size() > MaxBufferedBytes && !m_buffer.contains('\n')) {
        const QString preview = QString::fromUtf8(m_buffer.left(160));
        m_buffer.clear();
        emit packetRejected(QStringLiteral("Input buffer exceeded 64 KiB without frame delimiter"), preview);
        return;
    }

    while (true) {
        const qsizetype newline = m_buffer.indexOf('\n');
        if (newline < 0)
            break;

        const QByteArray line = m_buffer.left(newline).trimmed();
        m_buffer.remove(0, newline + 1);

        if (line.isEmpty())
            continue;

        const QString raw = QString::fromUtf8(line);
        emit rawPacketReceived(raw);

        TelemetryFrame frame;
        QString parseError;
        if (PacketParser::parse(line, frame, &parseError))
            emit telemetryReceived(frame);
        else
            emit packetRejected(parseError, raw);
    }
}

void TelemetryWorker::consumeDatagram(const QByteArray &data)
{
    QList<QByteArray> frames = data.split('\n');
    for (QByteArray line : frames) {
        line = line.trimmed();
        if (line.isEmpty())
            continue;

        const QString raw = QString::fromUtf8(line);
        emit rawPacketReceived(raw);
        TelemetryFrame frame;
        QString parseError;
        if (PacketParser::parse(line, frame, &parseError))
            emit telemetryReceived(frame);
        else
            emit packetRejected(parseError, raw);
    }
}

void TelemetryWorker::scheduleReconnect()
{
    if (!m_autoReconnect || m_manualDisconnect || m_transport == Transport::None
        || m_transport == Transport::Udp || m_reconnectTimer->isActive())
        return;

    ++m_reconnectAttempts;
    const int exponent = qMin(m_reconnectAttempts - 1, 3);
    const int delayMs = 1000 * (1 << exponent);
    emit reconnectScheduled(m_reconnectAttempts, delayMs);
    emit connectionStateChanged(QStringLiteral("Reconnecting..."));
    m_reconnectTimer->start(delayMs);
}

void TelemetryWorker::reconnectCurrent()
{
    if (m_transport == Transport::Tcp) {
        ensureTcpSocket();
        m_tcpSocket->abort();
        emit connectionStateChanged(QStringLiteral("Connecting TCP..."));
        m_tcpSocket->connectToHost(m_tcpHost, m_tcpPort);
        return;
    }

    if (m_transport == Transport::Serial) {
        ensureSerialPort();
        if (m_serialPort->isOpen())
            m_serialPort->close();
        m_serialPort->setPortName(m_serialPortName);
        m_serialPort->setBaudRate(m_baudRate);
        emit connectionStateChanged(QStringLiteral("Opening serial port..."));
        if (m_serialPort->open(QIODevice::ReadOnly)) {
            m_reconnectAttempts = 0;
            emit connected(currentTransportName());
            emit connectionStateChanged(QStringLiteral("Connected"));
        } else {
            emit errorOccurred(QStringLiteral("Serial reopen failed: %1").arg(m_serialPort->errorString()));
            scheduleReconnect();
        }
    }
}

QString TelemetryWorker::currentTransportName() const
{
    if (m_transport == Transport::Tcp)
        return QStringLiteral("TCP %1:%2").arg(m_tcpHost).arg(m_tcpPort);
    if (m_transport == Transport::Udp)
        return QStringLiteral("UDP %1:%2").arg(m_udpBindAddress).arg(m_udpPort);
    if (m_transport == Transport::Serial)
        return QStringLiteral("Serial %1 @ %2").arg(m_serialPortName).arg(m_baudRate);
    return QStringLiteral("None");
}
