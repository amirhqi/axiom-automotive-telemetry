#include <QtTest>

#include "TelemetryController.h"

#include <QHostAddress>
#include <QSignalSpy>
#include <QTcpServer>
#include <QTcpSocket>

class FunctionalSmokeTests : public QObject
{
    Q_OBJECT
private slots:
    void endToEndTcpUpdatesController();
};

void FunctionalSmokeTests::endToEndTcpUpdatesController()
{
    QTcpServer server;
    QVERIFY(server.listen(QHostAddress::LocalHost, 0));

    TelemetryController controller;
    controller.setAutoReconnect(false);
    QSignalSpy telemetrySpy(&controller, &TelemetryController::telemetryChanged);

    controller.connectTcp(QStringLiteral("127.0.0.1"), server.serverPort());
    QTRY_VERIFY_WITH_TIMEOUT(server.hasPendingConnections(), 2000);
    QTcpSocket *peer = server.nextPendingConnection();
    QVERIFY(peer != nullptr);

    peer->write("RPM=3100,SPEED=101,TEMP=94.5,BATTERY=13.6\n");
    peer->flush();

    QTRY_COMPARE_WITH_TIMEOUT(telemetrySpy.count(), 1, 2000);
    QCOMPARE(controller.rpm(), 3100);
    QCOMPARE(controller.speed(), 101);
    QCOMPARE(controller.temperature(), 94.5);
    QCOMPARE(controller.battery(), 13.6);
    QCOMPARE(controller.validPackets(), qulonglong(1));

    peer->disconnectFromHost();
    QTRY_COMPARE_WITH_TIMEOUT(controller.status(), QStringLiteral("Disconnected"), 2000);
}

QTEST_APPLESS_MAIN(FunctionalSmokeTests)
#include "tst_functionalsmoke.moc"
