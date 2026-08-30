#pragma once

#include "TelemetryFrame.h"

#include <QByteArray>
#include <QString>

class PacketParser
{
public:
    static bool parse(const QByteArray &line, TelemetryFrame &out, QString *error = nullptr);
};
