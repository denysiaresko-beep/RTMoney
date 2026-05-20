#ifndef CURRENCYMANAGER_H
#define CURRENCYMANAGER_H

#include <QNetworkAccessManager>

#include <QObject>

class CurrencyManager : public QObject
{
    Q_OBJECT
public:
    QNetworkAccessManager *manager;
    CurrencyManager();

    Q_INVOKABLE void updateRate(const QString &coin, const QString &curr);
    Q_INVOKABLE void updateChart(const QString &coin, const QString &curr, int days = 7);
    //Q_INVOKABLE void updateChartRange(const QString &coin, const QString &curr, qint64 from, qint64 to);
signals:
    void rateChanged(double);
    void chartDataReady(const QVariantList &points, double minPrice, double maxPrice);

};

#endif // CURRENCYMANAGER_H
