#include "CsvLogger.h"

#include <QDir>
#include <QFileInfo>
#include <QTextStream>
#include <QUrl>

namespace {
QString normalizePath(const QString &path)
{
    const QUrl url(path);
    if (url.isValid() && url.isLocalFile())
        return url.toLocalFile();
    return path;
}
}

bool CsvLogger::start(const QString &path, QString *error)
{
    stop();

    const QString localPath = normalizePath(path.trimmed());
    if (localPath.isEmpty()) {
        if (error)
            *error = QStringLiteral("Log path is empty");
        return false;
    }

    const QFileInfo info(localPath);
    if (!info.absolutePath().isEmpty())
        QDir().mkpath(info.absolutePath());

    m_file.setFileName(localPath);
    if (!m_file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        if (error)
            *error = m_file.errorString();
        return false;
    }

    QTextStream stream(&m_file);
    stream << "timestamp_utc,transport,rpm,speed_kmh,coolant_c,battery_v\n";
    stream.flush();
    return true;
}

void CsvLogger::stop()
{
    if (m_file.isOpen()) {
        m_file.flush();
        m_file.close();
    }
}

bool CsvLogger::isActive() const
{
    return m_file.isOpen();
}

QString CsvLogger::path() const
{
    return m_file.fileName();
}

bool CsvLogger::writeFrame(const TelemetryFrame &frame, const QString &transport, QString *error)
{
    if (!m_file.isOpen()) {
        if (error)
            *error = QStringLiteral("Logger is not active");
        return false;
    }

    QTextStream stream(&m_file);
    const QDateTime timestamp = frame.receivedAt.isValid() ? frame.receivedAt : QDateTime::currentDateTimeUtc();
    stream << timestamp.toString(Qt::ISODateWithMs) << ',' << transport << ',' << frame.rpm << ','
           << frame.speed << ',' << QString::number(frame.temperature, 'f', 1) << ','
           << QString::number(frame.battery, 'f', 2) << '\n';

    if (stream.status() != QTextStream::Ok) {
        if (error)
            *error = QStringLiteral("Failed to write CSV row");
        return false;
    }

    stream.flush();
    return true;
}
