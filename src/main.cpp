#include <QByteArray>
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
    // AXIOM_READABILITY_SCALE_BEGIN
    // Accessibility/readability: make the whole Qt UI larger on 100% scale displays.
    qputenv("QT_SCALE_FACTOR", QByteArrayLiteral("1.20"));
    // AXIOM_READABILITY_SCALE_END

    QGuiApplication app(argc, argv);
    // AXIOM_V263_ICON_BEGIN
    app.setWindowIcon(QIcon(QStringLiteral(":/branding/axiom_icon.png")));
    // AXIOM_V263_ICON_END
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