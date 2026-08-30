#include <QtTest>

#include "CsvLogger.h"

#include <QFile>
#include <QTemporaryDir>

class CsvLoggerTests : public QObject
{
    Q_OBJECT

private slots:
    void writesHeaderAndValidatedFrame();
};

void CsvLoggerTests::writesHeaderAndValidatedFrame()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString path = dir.filePath(QStringLiteral("session.csv"));
    CsvLogger logger;
    QString error;
    QVERIFY2(logger.start(path, &error), qPrintable(error));

    TelemetryFrame frame;
    frame.rpm = 2450;
    frame.speed = 82;
    frame.temperature = 91.5;
    frame.battery = 13.9;
    frame.receivedAt = QDateTime::fromString(QStringLiteral("2026-08-29T03:00:00.000Z"), Qt::ISODateWithMs);

    QVERIFY2(logger.writeFrame(frame, QStringLiteral("TCP 127.0.0.1:5000"), &error), qPrintable(error));
    logger.stop();

    QFile file(path);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    const QByteArray data = file.readAll();
    QVERIFY(data.startsWith("timestamp_utc,transport,rpm,speed_kmh,coolant_c,battery_v\n"));
    QVERIFY(data.contains("TCP 127.0.0.1:5000,2450,82,91.5,13.90"));
}

QTEST_APPLESS_MAIN(CsvLoggerTests)
#include "tst_csvlogger.moc"
