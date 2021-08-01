#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QQuickView>
#include "smoothcurve.h"


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<SmoothCurve>("CustomGeometry", 1, 0, "SmoothCurve");

    QQuickView view;
    QSurfaceFormat format = view.format();
    format.setSamples(16);
    view.setFormat(format);
    view.setSource(QUrl("qrc:///main.qml"));
    view.show();

    return app.exec();
}
