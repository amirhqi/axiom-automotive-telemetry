#include <QtTest>

#include "TelemetryWorker.h"

#include <QHostAddress>
#include <QSignalSpy>
#include <QUdpSocket>

class UdpIntegrationTests : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void receivesValidDatagram();
    void rejectsMalformedDatagram();
};

void UdpIntegrationTests::initTestCase()
{
    qRegisterMetaType<TelemetryFrame>("TelemetryFrame");
}

void UdpIntegrationTests::receivesValidDatagram()
{
    QUdpSocket probe;
    QVERIFY(probe.bind(QHostAddress::LocalHost, 0));
    const quint16 port = probe.localPort();
    probe.close();

    TelemetryWorker worker;
    QSignalSpy connectedSpy(&worker, &TelemetryWorker::connected);
    QSignalSpy frameSpy(&worker, &TelemetryWorker::telemetryReceived);
    worker.connectUdp(QStringLiteral("127.0.0.1"), port);
    QTRY_COMPARE_WITH_TIMEOUT(connectedSpy.count(), 1, 1000);

    QUdpSocket sender;
    const QByteArray payload("RPM=2600,SPEED=90,TEMP=92.0,BATTERY=13.8\n");
    QCOMPARE(sender.writeDatagram(payload, QHostAddress::LocalHost, port), qint64(payload.size()));
    QTRY_COMPARE_WITH_TIMEOUT(frameSpy.count(), 1, 1000);

    const TelemetryFrame f = qvariant_cast<TelemetryFrame>(frameSpy.takeFirst().at(0));
    QCOMPARE(f.rpm, 2600);
    QCOMPARE(f.speed, 90);
    QCOMPARE(f.temperature, 92.0);
    QCOMPARE(f.battery, 13.8);
    worker.disconnectTransport();
}

void UdpIntegrationTests::rejectsMalformedDatagram()
{
    QUdpSocket probe;
    QVERIFY(probe.bind(QHostAddress::LocalHost, 0));
    const quint16 port = probe.localPort();
    probe.close();

    TelemetryWorker worker;
    QSignalSpy rejectedSpy(&worker, &TelemetryWorker::packetRejected);
    worker.connectUdp(QStringLiteral("127.0.0.1"), port);

    QUdpSocket sender;
    const QByteArray payload("RPM=oops,SPEED=90,TEMP=92.0,BATTERY=13.8\n");
    sender.writeDatagram(payload, QHostAddress::LocalHost, port);
    QTRY_COMPARE_WITH_TIMEOUT(rejectedSpy.count(), 1, 1000);
    worker.disconnectTransport();
}

QTEST_GUILESS_MAIN(UdpIntegrationTests)
#include "tst_udpintegration.moc"

