#pragma once

#include "CsvLogger.h"
#include "TelemetryFrame.h"

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QVariantList>

class TelemetryWorker;

class TelemetryController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int rpm READ rpm NOTIFY telemetryChanged)
    Q_PROPERTY(int speed READ speed NOTIFY telemetryChanged)
    Q_PROPERTY(double temperature READ temperature NOTIFY telemetryChanged)
    Q_PROPERTY(double battery READ battery NOTIFY telemetryChanged)

    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString currentTransport READ currentTransport NOTIFY currentTransportChanged)
    Q_PROPERTY(QString lastPacket READ lastPacket NOTIFY lastPacketChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY lastErrorChanged)

    Q_PROPERTY(qulonglong totalPackets READ totalPackets NOTIFY statisticsChanged)
    Q_PROPERTY(qulonglong validPackets READ validPackets NOTIFY statisticsChanged)
    Q_PROPERTY(qulonglong rejectedPackets READ rejectedPackets NOTIFY statisticsChanged)
    Q_PROPERTY(qulonglong bytesReceived READ bytesReceived NOTIFY statisticsChanged)
    Q_PROPERTY(double packetRate READ packetRate NOTIFY statisticsChanged)
    Q_PROPERTY(int reconnectAttempts READ reconnectAttempts NOTIFY statisticsChanged)

    Q_PROPERTY(bool autoReconnect READ autoReconnect WRITE setAutoReconnect NOTIFY autoReconnectChanged)
    Q_PROPERTY(bool loggingActive READ loggingActive NOTIFY loggingChanged)
    Q_PROPERTY(QString logPath READ logPath NOTIFY loggingChanged)
    Q_PROPERTY(QString defaultLogPath READ defaultLogPath CONSTANT)

    Q_PROPERTY(QStringList serialPorts READ serialPorts NOTIFY serialPortsChanged)
    Q_PROPERTY(QStringList rawPackets READ rawPackets NOTIFY rawPacketsChanged)
    Q_PROPERTY(QStringList eventLog READ eventLog NOTIFY eventLogChanged)

    Q_PROPERTY(QVariantList rpmSeries READ rpmSeries NOTIFY chartDataChanged)
    Q_PROPERTY(QVariantList speedSeries READ speedSeries NOTIFY chartDataChanged)
    Q_PROPERTY(QVariantList temperatureSeries READ temperatureSeries NOTIFY chartDataChanged)
    Q_PROPERTY(QVariantList batterySeries READ batterySeries NOTIFY chartDataChanged)

public:
    explicit TelemetryController(QObject *parent = nullptr);
    ~TelemetryController() override;

    int rpm() const { return m_frame.rpm; }
    int speed() const { return m_frame.speed; }
    double temperature() const { return m_frame.temperature; }
    double battery() const { return m_frame.battery; }

    QString status() const { return m_status; }
    QString currentTransport() const { return m_currentTransport; }
    QString lastPacket() const { return m_lastPacket; }
    QString lastError() const { return m_lastError; }

    qulonglong totalPackets() const { return m_validPackets + m_rejectedPackets; }
    qulonglong validPackets() const { return m_validPackets; }
    qulonglong rejectedPackets() const { return m_rejectedPackets; }
    qulonglong bytesReceived() const { return m_bytesReceived; }
    double packetRate() const { return m_packetRate; }
    int reconnectAttempts() const { return m_reconnectAttempts; }

    bool autoReconnect() const { return m_autoReconnect; }
    bool loggingActive() const { return m_logger.isActive(); }
    QString logPath() const { return m_logger.path(); }
    QString defaultLogPath() const { return m_defaultLogPath; }

    QStringList serialPorts() const { return m_serialPorts; }
    QStringList rawPackets() const { return m_rawPackets; }
    QStringList eventLog() const { return m_eventLog; }

    QVariantList rpmSeries() const { return m_rpmSeries; }
    QVariantList speedSeries() const { return m_speedSeries; }
    QVariantList temperatureSeries() const { return m_temperatureSeries; }
    QVariantList batterySeries() const { return m_batterySeries; }

    Q_INVOKABLE void connectTcp(const QString &host, int port);
    Q_INVOKABLE void connectUdp(const QString &bindAddress, int port);
    Q_INVOKABLE void connectSerial(const QString &portName, int baudRate);
    Q_INVOKABLE void disconnectTransport();
    Q_INVOKABLE void refreshSerialPorts();
    Q_INVOKABLE void clearHistory();
    Q_INVOKABLE bool startLogging(const QString &path);
    Q_INVOKABLE void stopLogging();

public slots:
    void setAutoReconnect(bool enabled);

signals:
    void telemetryChanged();
    void statusChanged();
    void currentTransportChanged();
    void lastPacketChanged();
    void lastErrorChanged();
    void statisticsChanged();
    void autoReconnectChanged();
    void loggingChanged();
    void serialPortsChanged();
    void rawPacketsChanged();
    void eventLogChanged();
    void chartDataChanged();

    void requestConnectTcp(const QString &host, quint16 port);
    void requestConnectUdp(const QString &bindAddress, quint16 port);
    void requestConnectSerial(const QString &portName, qint32 baudRate);
    void requestDisconnect();
    void requestSetAutoReconnect(bool enabled);

private:
    void appendEvent(const QString &message);
    void appendRawPacket(const QString &packet);
    void appendChartPoint(const TelemetryFrame &frame);
    void setLastError(const QString &message);
    static void appendBounded(QVariantList &series, double value, int maxSize);

    TelemetryFrame m_frame;
    QString m_status = QStringLiteral("Disconnected");
    QString m_currentTransport = QStringLiteral("None");
    QString m_lastPacket;
    QString m_lastError;

    qulonglong m_validPackets = 0;
    qulonglong m_rejectedPackets = 0;
    qulonglong m_bytesReceived = 0;
    qulonglong m_packetsAtLastRateSample = 0;
    double m_packetRate = 0.0;
    int m_reconnectAttempts = 0;
    bool m_autoReconnect = true;

    QStringList m_serialPorts;
    QStringList m_rawPackets;
    QStringList m_eventLog;

    QVariantList m_rpmSeries;
    QVariantList m_speedSeries;
    QVariantList m_temperatureSeries;
    QVariantList m_batterySeries;

    CsvLogger m_logger;
    QString m_defaultLogPath;

    QThread m_workerThread;
    TelemetryWorker *m_worker = nullptr;
    QTimer m_rateTimer;
};
