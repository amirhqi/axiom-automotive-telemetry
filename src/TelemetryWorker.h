#pragma once

#include "TelemetryFrame.h"

#include <QObject>
#include <QSerialPort>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QTimer>

class TelemetryWorker : public QObject
{
    Q_OBJECT

public:
    explicit TelemetryWorker(QObject *parent = nullptr);

public slots:
    void connectTcp(const QString &host, quint16 port);
    void connectUdp(const QString &bindAddress, quint16 port);
    void connectSerial(const QString &portName, qint32 baudRate);
    void disconnectTransport();
    void setAutoReconnect(bool enabled);

signals:
    void connected(const QString &transportName);
    void disconnected();
    void connectionStateChanged(const QString &state);
    void telemetryReceived(const TelemetryFrame &frame);
    void rawPacketReceived(const QString &packet);
    void packetRejected(const QString &reason, const QString &packet);
    void bytesReceived(qint64 count);
    void errorOccurred(const QString &message);
    void reconnectScheduled(int attempt, int delayMs);

private slots:
    void onTcpReadyRead();
    void onTcpConnected();
    void onTcpDisconnected();
    void onTcpError(QAbstractSocket::SocketError error);
    void onUdpReadyRead();
    void onUdpError(QAbstractSocket::SocketError error);
    void onSerialReadyRead();
    void onSerialError(QSerialPort::SerialPortError error);
    void reconnectCurrent();

private:
    enum class Transport { None, Tcp, Udp, Serial };

    void ensureTcpSocket();
    void ensureUdpSocket();
    void ensureSerialPort();
    void closeCurrentTransport();
    void consumeData(const QByteArray &data);
    void consumeDatagram(const QByteArray &data);
    void scheduleReconnect();
    QString currentTransportName() const;

    QTcpSocket *m_tcpSocket = nullptr;
    QUdpSocket *m_udpSocket = nullptr;
    QSerialPort *m_serialPort = nullptr;
    QTimer *m_reconnectTimer = nullptr;

    Transport m_transport = Transport::None;
    QByteArray m_buffer;

    QString m_tcpHost;
    quint16 m_tcpPort = 0;
    QString m_udpBindAddress = QStringLiteral("0.0.0.0");
    quint16 m_udpPort = 0;
    QString m_serialPortName;
    qint32 m_baudRate = 115200;

    bool m_autoReconnect = true;
    bool m_manualDisconnect = false;
    int m_reconnectAttempts = 0;
};
