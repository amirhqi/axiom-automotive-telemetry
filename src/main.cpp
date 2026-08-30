#include "TelemetryController.h"

#include <QGuiApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QtQml/qqml.h>

int main(int argc, char *argv[])
{
    // Fusion gives the desktop controls a consistent engineering-tool look on
    // Windows and Linux instead of inheriting a platform-specific light style.
    qputenv("QT_QUICK_CONTROLS_STYLE", QByteArrayLiteral("Fusion"));

    QGuiApplication app(argc, argv);
    QGuiApplication::setOrganizationName(QStringLiteral("AXIOM Engineering"));
    QGuiApplication::setApplicationName(QStringLiteral("AXIOM Automotive Telemetry Workbench"));
    QGuiApplication::setApplicationVersion(QStringLiteral("2.6"));
    QGuiApplication::setWindowIcon(QIcon(QStringLiteral(":/assets/axiom_icon.png")));

    TelemetryController controller;
    qmlRegisterSingletonInstance("AutomotiveTelemetry", 1, 0, "Telemetry", &controller);

    QQmlApplicationEngine engine;
    engine.loadFromModule("AutomotiveTelemetry", "Main");

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}