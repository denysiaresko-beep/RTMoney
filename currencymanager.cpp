#include "currencymanager.h"
#include <QByteArray>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

CurrencyManager::CurrencyManager() {
    manager = new QNetworkAccessManager(this);
}


void CurrencyManager::updateRate(const QString &coin,const QString &curr) {
    QString urlString = QString("https://api.coingecko.com/api/v3/simple/price?vs_currencies=%2&ids=%1&x_cg_demo_api_key=CG-StPNxD7SgVnr81ZfNS5fvcaF").arg(coin).arg(curr);
    QUrl url(urlString);

    QNetworkRequest request(url);

    QNetworkReply *replay = manager->get(request);

    connect(replay, &QNetworkReply::finished, this, [this, replay, coin, curr]() {
        if (replay->error() == QNetworkReply::NoError) {
            QByteArray responseData = replay->readAll();

            QJsonDocument doc = QJsonDocument::fromJson(responseData);
            QJsonObject rootObj = doc.object();

            if (rootObj.contains(coin)) {
                QJsonObject coinObj = rootObj[coin].toObject();
                double rate = coinObj[curr].toDouble();

                qDebug() << "rate for " << coin << ":" << rate;

                emit rateChanged(rate);
            }


        } else {
            qDebug() << "Network Error! " << replay->errorString();
        }

        replay->deleteLater();
    });

}


void CurrencyManager::updateChart(const QString &coin, const QString &curr, int days) {
    QString urlString = QString("https://api.coingecko.com/api/v3/coins/%1/market_chart?vs_currency=%2&days=%3&x_cg_demo_api_key=CG-StPNxD7SgVnr81ZfNS5fvcaF").arg(coin).arg(curr).arg(days);
    QUrl url(urlString);
    QNetworkRequest request(url);

    QNetworkReply *reply = manager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply, coin, curr](){
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray responseData = reply->readAll();

            QJsonDocument doc = QJsonDocument::fromJson(responseData);
            QJsonObject rootObj = doc.object();

            if (rootObj.contains("prices")) {
                QJsonArray pricesArray = rootObj["prices"].toArray();
                QVariantList points;

                double minPrice = std::numeric_limits<double>::max();
                double maxPrice = std::numeric_limits<double>::lowest();
                double latestRate = 0.0;

                for (int i = 0; i < pricesArray.size(); ++i) {
                    QJsonArray pt = pricesArray[i].toArray();
                    double timestamp = pt[0].toDouble();
                    double price = pt[1].toDouble();

                    if (price < minPrice) {
                        minPrice = price;
                    }
                    if (price > maxPrice) {
                        maxPrice = price;
                    }

                    QVariantMap pointMap;
                    pointMap["x"] = timestamp;
                    pointMap["y"] = price;
                    points.append(pointMap);

                    if (i == pricesArray.size() - 1) {
                        latestRate = price;
                    }
                }

                emit rateChanged(latestRate);
                emit chartDataReady(points, minPrice, maxPrice);

                qDebug() << "Chart rate for " << coin << " to " << latestRate;
            }
        } else {
            qDebug() << "Network Error! " << reply->errorString();
        }
        reply->deleteLater();

    });
}

void CurrencyManager::updateChartRange(const QString &coin, const QString &curr, qint64 from, qint64 to) {
    // API CoinGecko для діапазону дат приймає час у секундах (UNIX timestamp)
    QString urlString = QString("https://api.coingecko.com/api/v3/coins/%1/market_chart/range?vs_currency=%2&from=%3&to=%4&x_cg_demo_api_key=CG-StPNxD7SgVnr81ZfNS5fvcaF")
                            .arg(coin).arg(curr).arg(from).arg(to);
    QUrl url(urlString);
    QNetworkRequest request(url);

    QNetworkReply *reply = manager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply, coin, curr](){
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray responseData = reply->readAll();

            QJsonDocument doc = QJsonDocument::fromJson(responseData);
            QJsonObject rootObj = doc.object();

            if (rootObj.contains("prices")) {
                QJsonArray pricesArray = rootObj["prices"].toArray();
                QVariantList points;

                double minPrice = std::numeric_limits<double>::max();
                double maxPrice = std::numeric_limits<double>::lowest();
                double latestRate = 0.0;

                for (int i = 0; i < pricesArray.size(); ++i) {
                    QJsonArray pt = pricesArray[i].toArray();
                    double timestamp = pt[0].toDouble();
                    double price = pt[1].toDouble();

                    if (price < minPrice) {
                        minPrice = price;
                    }
                    if (price > maxPrice) {
                        maxPrice = price;
                    }

                    QVariantMap pointMap;
                    pointMap["x"] = timestamp;
                    pointMap["y"] = price;
                    points.append(pointMap);

                    if (i == pricesArray.size() - 1) {
                        latestRate = price;
                    }
                }

                if (!points.isEmpty()) {
                    emit rateChanged(latestRate);
                    emit chartDataReady(points, minPrice, maxPrice);
                }
            }
        } else {
            qDebug() << "Network Error! " << reply->errorString();
        }
        reply->deleteLater();
    });
}