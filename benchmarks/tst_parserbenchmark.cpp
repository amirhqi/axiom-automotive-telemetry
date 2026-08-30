#include <QtTest>

#include "PacketParser.h"

class ParserBenchmark : public QObject
{
    Q_OBJECT
private slots:
    void parseValidFrame();
};

void ParserBenchmark::parseValidFrame()
{
    const QByteArray data("RPM=2450,SPEED=82,TEMP=91.5,BATTERY=13.9");
    QBENCHMARK {
        TelemetryFrame frame;
        QString error;
        const bool ok = PacketParser::parse(data, frame, &error);
        Q_UNUSED(ok);
    }
}

QTEST_APPLESS_MAIN(ParserBenchmark)
#include "tst_parserbenchmark.moc"
