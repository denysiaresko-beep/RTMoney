#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "currencymanager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;

    CurrencyManager* my = new CurrencyManager();

    engine.rootContext()->setContextProperty("myManager", my);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    engine.loadFromModule("RTMoney", "Main");


    return QCoreApplication::exec();
}
