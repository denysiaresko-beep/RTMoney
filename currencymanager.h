#ifndef CURRENCYMANAGER_H
#define CURRENCYMANAGER_H

#include <QNetworkAccessManager>

#include <QObject>


struct ParserChartData;

class CurrencyManager : public QObject
{
    Q_OBJECT
public:
    QNetworkAccessManager *manager;
    CurrencyManager();
    QStringList const currencies = { "usd", "eur", "uah" };

    Q_INVOKABLE void updateRate(const QString &coin, const QString &curr);
    Q_INVOKABLE void updateChart(const QString &coin, const QString &curr, int days = 7);
    ParserChartData parseChartJson(const QByteArray &jsonData, bool needsInvert, double btcCrossRate = 1.0);

signals:
    void rateChanged(double);
    void chartDataReady(const QVariantList &points, double minPrice, double maxPrice);

};

struct ParserChartData {


    QVariantList points;
    double minPrice = 0.0;
    double maxPrice = 0.0;
    double latestRate = 0.0;
    bool isValid = false;


};

struct Config {
    QString apiKey;

    Config(QString filepath);
};

#endif // CURRENCYMANAGER_H
