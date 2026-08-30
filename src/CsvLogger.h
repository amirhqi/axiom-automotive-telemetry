#pragma once

#include "TelemetryFrame.h"

#include <QFile>
#include <QString>

class CsvLogger
{
public:
    bool start(const QString &path, QString *error = nullptr);
    void stop();
    bool isActive() const;
    QString path() const;
    bool writeFrame(const TelemetryFrame &frame, const QString &transport, QString *error = nullptr);

private:
    QFile m_file;
};
