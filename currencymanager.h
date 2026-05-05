#ifndef CURRENCYMANAGER_H
#define CURRENCYMANAGER_H

#include <QNetworkAccessManager>

#include <QObject>

class CurrencyManager : public QObject
{
public:
    QNetworkAccessManager *manager;
    CurrencyManager();

    void updateRate(QString coin);
signals:
    double rateChanged(double);

};

#endif // CURRENCYMANAGER_H
