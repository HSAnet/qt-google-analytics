#ifndef GANALYTICS_H
#define GANALYTICS_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QCoreApplication>
#include <QUrlQuery>
#include <QQueue>
#include <QTimer>

class GAnalytics : public QObject
{
    Q_OBJECT
public:
    explicit GAnalytics(QCoreApplication *parent, QString trackingID, QString clientID = "");

private:
    QNetworkAccessManager networkManager;
    QQueue<QUrlQuery> messageQueue;
    QTimer timer;
    QNetworkRequest requestUrl;
    QString trackingID;
    QString clientID;
    QString userID;
    QString userIPAddress;
    QString userAgent;
    QString appName;
    QString appVersion;

signals:

public slots:
    void sendAppview(QString appName, QString appVersion = "", QString screenName = "");
    void postMessage();
    void postMessageFinished(QNetworkReply *replay);

};

#endif // GANALYTICS_H
