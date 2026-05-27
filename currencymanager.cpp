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

    bool isCoinFiat = currencies.contains(coin);
    bool isCurrFiat = currencies.contains(curr);
    QString finalId = coin;
    QString finalVs = curr;
    bool needsInvert = false;

    if (isCoinFiat && !isCurrFiat) {
        finalId = curr;
        finalVs = coin;
        needsInvert = true;
    } else if (isCoinFiat && isCurrFiat) {
        QString urlString = QString("https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=%1,%2&x_cg_demo_api_key=CG-StPNxD7SgVnr81ZfNS5fvcaF")
        .arg(coin).arg(curr);

        QNetworkRequest request((QUrl(urlString)));
        QNetworkReply *priceReply = manager->get(request);

        connect(priceReply, &QNetworkReply::finished, this, [this, priceReply, coin, curr, days]() {
            if(priceReply->error() == QNetworkReply::NoError) {
                QJsonDocument doc = QJsonDocument::fromJson(priceReply->readAll());
                QJsonObject btcObj = doc.object()["bitcoin"].toObject();

                double btcToCoin = btcObj[coin].toDouble();
                double btcToCurr = btcObj[curr].toDouble();

                if (btcToCoin > 0) {
                    double currentCrossRate = btcToCurr / btcToCoin;
                    emit rateChanged(currentCrossRate);
                    qDebug() << "Fiat Cross-Rate [" << coin << "->" << curr << "]:" << currentCrossRate;

                    QString chartUrlStr = QString("https://api.coingecko.com/api/v3/coins/bitcoin/market_chart?vs_currency=%1&days=%2&x_cg_demo_api_key=CG-StPNxD7SgVnr81ZfNS5fvcaF")
                                              .arg(curr).arg(days);
                    QNetworkRequest chartRequest((QUrl(chartUrlStr)));
                    QNetworkReply *chartReply = manager->get(chartRequest);

                    connect(chartReply, &QNetworkReply::finished, this, [this, chartReply, btcToCoin]() {
                        if (chartReply->error() == QNetworkReply::NoError) {
                            chartReply->deleteLater();

                            ParserChartData parsedData = parseChartJson(chartReply->readAll(), false, btcToCoin);

                            if(parsedData.isValid) {
                                emit chartDataReady(parsedData.points, parsedData.minPrice, parsedData.maxPrice);
                                qDebug() << "Fiat Cross-Chart ready for rendering.";
                            }
                        } else {
                            qDebug() << "Fiat Cross-Chart Error:" << chartReply->errorString();
                        }
                    });
                }
            } else {
                qDebug() << "Fiat Cross-Price Bridge Error:" << priceReply->errorString();
            }
            priceReply->deleteLater();
        });
        return;
    }

    QString urlString = QString("https://api.coingecko.com/api/v3/coins/%1/market_chart?vs_currency=%2&days=%3&x_cg_demo_api_key=CG-StPNxD7SgVnr81ZfNS5fvcaF").arg(finalId).arg(finalVs).arg(days);
    QUrl url(urlString);
    QNetworkRequest request(url);

    QNetworkReply *reply = manager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply, coin, curr, needsInvert](){
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "Network Error:" << reply->errorString();
            return;
        }


        ParserChartData parsedData = parseChartJson(reply->readAll(), needsInvert);

        if (!parsedData.isValid) {
            qDebug() << "Failed ti parse chart JSON";
            return;
        }

        emit rateChanged(parsedData.latestRate);
        emit chartDataReady(parsedData.points, parsedData.minPrice, parsedData.maxPrice);

        qDebug() << "Chart rate for " << coin << " to " << parsedData.latestRate;
    });
}


ParserChartData CurrencyManager::parseChartJson(const QByteArray &jsonData, bool needsInvert, double btcCrossRate) {
    ParserChartData result;

    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    QJsonObject rootObject = doc.object();

    if(!rootObject.contains("prices")) {
        return result;
    }

    QJsonArray pricesArray = rootObject["prices"].toArray();
    result.minPrice = std::numeric_limits<double>::max();
    result.maxPrice = std::numeric_limits<double>::lowest();

    for(const QJsonValue &value : pricesArray) {
        QJsonArray pt = value.toArray();
        double timestamp = pt[0].toDouble();
        double price = pt[1].toDouble();


        if (btcCrossRate != 1.0 && btcCrossRate > 0) {
            price = price / btcCrossRate;
        }

        if (needsInvert && price != 0.0) {
            price = 1.0 / price;
        }

        result.minPrice = std::min(result.minPrice, price);
        result.maxPrice = std::max(result.maxPrice, price);
        result.latestRate = price;

        QVariantMap pointMap;
        pointMap["x"] = timestamp;
        pointMap["y"] = price;
        result.points.append(pointMap);
    }

    result.isValid = true;
    return result;


}
