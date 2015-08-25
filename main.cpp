#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include "match3model.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    qmlRegisterType<Match3Model>("mymodel", 1, 0, "MyModel");
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
