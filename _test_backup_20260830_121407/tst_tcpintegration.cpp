#include <QtTest>

#include "TelemetryWorker.h"

#include <QHostAddress>
#include <QSignalSpy>
#include <QTcpServer>
#include <QTcpSocket>

class TcpIntegrationTests : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void parsesFrameSplitAcrossTcpReads();
    void rejectsMalformedFrameWithoutCrashing();
};

void TcpIntegrationTests::initTestCase()
{
    qRegisterMetaType<TelemetryFrame>("TelemetryFrame");
}

void TcpIntegrationTests::parsesFrameSplitAcrossTcpReads()
{
    QTcpServer server;
    QVERIFY(server.listen(QHostAddress::LocalHost, 0));

    TelemetryWorker worker;
    worker.setAutoReconnect(false);

    QSignalSpy connectedSpy(&worker, &TelemetryWorker::connected);
    QSignalSpy frameSpy(&worker, &TelemetryWorker::telemetryReceived);

    worker.connectTcp(QStringLiteral("127.0.0.1"), server.serverPort());
    QTRY_VERIFY_WITH_TIMEOUT(server.hasPendingConnections(), 2000);
    QTRY_COMPARE_WITH_TIMEOUT(connectedSpy.count(), 1, 2000);

    QTcpSocket *peer = server.nextPendingConnection();
    QVERIFY(peer != nullptr);

    peer->write("RPM=2450,SPEED=82,");
    peer->flush();
    QTest::qWait(40);
    QCOMPARE(frameSpy.count(), 0);

    peer->write("TEMP=91.5,BATTERY=13.9\n");
    peer->flush();
    QTRY_COMPARE_WITH_TIMEOUT(frameSpy.count(), 1, 2000);

    const QList<QVariant> args = frameSpy.takeFirst();
    const TelemetryFrame frame = qvariant_cast<TelemetryFrame>(args.at(0));
    QCOMPARE(frame.rpm, 2450);
    QCOMPARE(frame.speed, 82);
    QCOMPARE(frame.temperature, 91.5);
    QCOMPARE(frame.battery, 13.9);

    worker.disconnectTransport();
}

void TcpIntegrationTests::rejectsMalformedFrameWithoutCrashing()
{
    QTcpServer server;
    QVERIFY(server.listen(QHostAddress::LocalHost, 0));

    TelemetryWorker worker;
    worker.setAutoReconnect(false);

    QSignalSpy rejectedSpy(&worker, &TelemetryWorker::packetRejected);
    QSignalSpy frameSpy(&worker, &TelemetryWorker::telemetryReceived);

    worker.connectTcp(QStringLiteral("127.0.0.1"), server.serverPort());
    QTRY_VERIFY_WITH_TIMEOUT(server.hasPendingConnections(), 2000);

    QTcpSocket *peer = server.nextPendingConnection();
    QVERIFY(peer != nullptr);

    peer->write("RPM=nope,SPEED=10,TEMP=80,BATTERY=13.5\n");
    peer->flush();

    QTRY_COMPARE_WITH_TIMEOUT(rejectedSpy.count(), 1, 2000);
    QCOMPARE(frameSpy.count(), 0);
    QVERIFY(rejectedSpy.at(0).at(0).toString().contains(QStringLiteral("Invalid numeric value")));

    worker.disconnectTransport();
}

QTEST_APPLESS_MAIN(TcpIntegrationTests)
#include "tst_tcpintegration.moc"
