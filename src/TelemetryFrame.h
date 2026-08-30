#pragma once

#include <QDateTime>
#include <QMetaType>

struct TelemetryFrame
{
    int rpm = 0;
    int speed = 0;
    double temperature = 0.0;
    double battery = 0.0;
    QDateTime receivedAt;
};

Q_DECLARE_METATYPE(TelemetryFrame)
