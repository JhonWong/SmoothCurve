#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include "smooth_curve_graph.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<SmoothCurve>("SceneGraphRendering", 1, 0, "SmoothCurve");

    QQuickView view;
    //QSurfaceFormat format = view.format();
    //format.setSamples(16);
    //view.setFormat(format);
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl("qrc:/main.qml"));
    view.show();

    return app.exec();
}
