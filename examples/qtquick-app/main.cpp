#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QQmlContext>

#include "ganalytics.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    GAnalytics tracker("1234-45");

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("tracker", &tracker);
    engine.load(QUrl("qrc:/qml/MainWindow.qml"));

    return app.exec();
}
