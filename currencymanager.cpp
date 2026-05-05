#include "currencymanager.h"
#include <QByteArray>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>

CurrencyManager::CurrencyManager() {
    manager = new QNetworkAccessManager(this);
}


void CurrencyManager::updateRate(QString coin) {
    QString urlString = QString("https://api.coingecko.com/api/v3/simple/price?vs_currencies=usd&ids=bitcoin&x_cg_demo_api_key=CG-StPNxD7SgVnr81ZfNS5fvcaF").arg(coin);
    QUrl url(urlString);

    QNetworkRequest request(url);

    QNetworkReply *replay = manager->get(request);

    connect(replay, &QNetworkReply::finished, this, [this, replay, coin]() {
        if (replay->error() == QNetworkReply::NoError) {
            QByteArray responseData = replay->readAll();

            QJsonDocument doc = QJsonDocument::fromJson(responseData);
            QJsonObject rootObj = doc.object();

            if (rootObj.contains(coin)) {
                QJsonObject coinObj = rootObj[coin].toObject();
                double rate = coinObj["usd"].toDouble();

                qDebug() << "rate for " << coin << ":" << rate;

                emit rateChanged(rate);
            }


        } else {
            qDebug() << "Network Error! " << replay->errorString();
        }

        replay->deleteLater();
    });

}
