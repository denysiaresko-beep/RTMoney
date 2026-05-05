#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "currencymanager.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("RTMoney", "Main");

    CurrencyManager* my = new CurrencyManager();

    my->updateRate("BTC");

    return QCoreApplication::exec();
}
