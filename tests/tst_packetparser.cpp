#include <QtTest>

#include "PacketParser.h"

class PacketParserTests : public QObject
{
    Q_OBJECT

private slots:
    void parsesValidPacket();
    void acceptsWhitespaceAndLowercaseKeys();
    void acceptsBoundaryValues();
    void rejectsEmptyPacket();
    void rejectsMissingField();
    void rejectsMalformedField();
    void rejectsDuplicateField();
    void rejectsInvalidNumber();
    void rejectsOutOfRangeRpm();
    void rejectsOutOfRangeBattery();
};

void PacketParserTests::parsesValidPacket()
{
    TelemetryFrame frame;
    QString error;
    QVERIFY(PacketParser::parse("RPM=2450,SPEED=82,TEMP=91.5,BATTERY=13.9", frame, &error));
    QCOMPARE(frame.rpm, 2450);
    QCOMPARE(frame.speed, 82);
    QCOMPARE(frame.temperature, 91.5);
    QCOMPARE(frame.battery, 13.9);
    QVERIFY(frame.receivedAt.isValid());
}

void PacketParserTests::acceptsWhitespaceAndLowercaseKeys()
{
    TelemetryFrame frame;
    QVERIFY(PacketParser::parse(" rpm = 1200 , speed = 25 , temp = 80.0 , battery = 12.5 ", frame));
    QCOMPARE(frame.rpm, 1200);
    QCOMPARE(frame.speed, 25);
}

void PacketParserTests::acceptsBoundaryValues()
{
    TelemetryFrame frame;
    QVERIFY(PacketParser::parse("RPM=0,SPEED=350,TEMP=-50,BATTERY=30", frame));
    QCOMPARE(frame.speed, 350);
    QCOMPARE(frame.temperature, -50.0);
}

void PacketParserTests::rejectsEmptyPacket()
{
    TelemetryFrame frame;
    QString error;
    QVERIFY(!PacketParser::parse("   ", frame, &error));
    QCOMPARE(error, QStringLiteral("Empty packet"));
}

void PacketParserTests::rejectsMissingField()
{
    TelemetryFrame frame;
    QString error;
    QVERIFY(!PacketParser::parse("RPM=2450,SPEED=82,TEMP=91.5", frame, &error));
    QVERIFY(error.contains(QStringLiteral("Missing field")));
}

void PacketParserTests::rejectsMalformedField()
{
    TelemetryFrame frame;
    QString error;
    QVERIFY(!PacketParser::parse("RPM=2450,SPEED,TEMP=91.5,BATTERY=13.9", frame, &error));
    QVERIFY(error.contains(QStringLiteral("Malformed field")));
}

void PacketParserTests::rejectsDuplicateField()
{
    TelemetryFrame frame;
    QString error;
    QVERIFY(!PacketParser::parse("RPM=2450,RPM=2500,SPEED=82,TEMP=91.5,BATTERY=13.9", frame, &error));
    QVERIFY(error.contains(QStringLiteral("Duplicate field")));
}

void PacketParserTests::rejectsInvalidNumber()
{
    TelemetryFrame frame;
    QString error;
    QVERIFY(!PacketParser::parse("RPM=abc,SPEED=82,TEMP=91.5,BATTERY=13.9", frame, &error));
    QCOMPARE(error, QStringLiteral("Invalid numeric value"));
}

void PacketParserTests::rejectsOutOfRangeRpm()
{
    TelemetryFrame frame;
    QString error;
    QVERIFY(!PacketParser::parse("RPM=20000,SPEED=82,TEMP=91.5,BATTERY=13.9", frame, &error));
    QCOMPARE(error, QStringLiteral("Value outside accepted range"));
}

void PacketParserTests::rejectsOutOfRangeBattery()
{
    TelemetryFrame frame;
    QString error;
    QVERIFY(!PacketParser::parse("RPM=2000,SPEED=82,TEMP=91.5,BATTERY=31", frame, &error));
    QCOMPARE(error, QStringLiteral("Value outside accepted range"));
}

QTEST_APPLESS_MAIN(PacketParserTests)
#include "tst_packetparser.moc"
