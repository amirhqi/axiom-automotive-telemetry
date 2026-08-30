#include "PacketParser.h"

#include <QDateTime>
#include <QHash>
#include <QStringList>

bool PacketParser::parse(const QByteArray &line, TelemetryFrame &out, QString *error)
{
    const QString text = QString::fromUtf8(line).trimmed();
    if (text.isEmpty()) {
        if (error)
            *error = QStringLiteral("Empty packet");
        return false;
    }

    QHash<QString, QString> fields;
    const QStringList parts = text.split(',', Qt::KeepEmptyParts);

    for (const QString &part : parts) {
        const int equalPos = part.indexOf('=');
        if (equalPos <= 0 || equalPos == part.size() - 1) {
            if (error)
                *error = QStringLiteral("Malformed field: %1").arg(part);
            return false;
        }

        const QString key = part.left(equalPos).trimmed().toUpper();
        const QString value = part.mid(equalPos + 1).trimmed();

        if (fields.contains(key)) {
            if (error)
                *error = QStringLiteral("Duplicate field: %1").arg(key);
            return false;
        }

        fields.insert(key, value);
    }

    const QStringList required = {QStringLiteral("RPM"), QStringLiteral("SPEED"),
                                  QStringLiteral("TEMP"), QStringLiteral("BATTERY")};
    for (const QString &key : required) {
        if (!fields.contains(key)) {
            if (error)
                *error = QStringLiteral("Missing field: %1").arg(key);
            return false;
        }
    }

    bool rpmOk = false;
    bool speedOk = false;
    bool tempOk = false;
    bool batteryOk = false;

    const int rpm = fields.value(QStringLiteral("RPM")).toInt(&rpmOk);
    const int speed = fields.value(QStringLiteral("SPEED")).toInt(&speedOk);
    const double temperature = fields.value(QStringLiteral("TEMP")).toDouble(&tempOk);
    const double battery = fields.value(QStringLiteral("BATTERY")).toDouble(&batteryOk);

    if (!rpmOk || !speedOk || !tempOk || !batteryOk) {
        if (error)
            *error = QStringLiteral("Invalid numeric value");
        return false;
    }

    if (rpm < 0 || rpm > 12000 || speed < 0 || speed > 350 || temperature < -50.0
        || temperature > 180.0 || battery < 0.0 || battery > 30.0) {
        if (error)
            *error = QStringLiteral("Value outside accepted range");
        return false;
    }

    out.rpm = rpm;
    out.speed = speed;
    out.temperature = temperature;
    out.battery = battery;
    out.receivedAt = QDateTime::currentDateTimeUtc();
    return true;
}
