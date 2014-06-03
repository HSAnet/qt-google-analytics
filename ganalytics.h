#ifndef GANALYTICS_H
#define GANALYTICS_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QCoreApplication>
#include <QUrl>
#include <QQueue>

class GAnalytics : public QObject
{
    Q_OBJECT
public:
    explicit GAnalytics(QCoreApplication *parent, QString trackingID, QString clientID = "", bool withGet = false);

private:
    QNetworkAccessManager networkManager;
    QQueue<QUrl> messageQueue;
    QString baseUrl;
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

};

#endif // GANALYTICS_H
