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
    explicit GAnalytics(QCoreApplication *parent, QString trackingID, QString clientID = QString());

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
    void postNextMessage();

public slots:
    void sendAppview(const QString screenName = QString());
    void sendEvent(const QString eventCategory = QString(),
                   const QString eventAction = QString(),
                   const QString eventLabel = QString(),
                   const QVariant eventValue = QVariant());
    void sendException(const QString exceptionDescription, const bool exceptionFatal = true);
    void endSession();
    void postMessage();
    void postMessageFinished(QNetworkReply *replay);

private:
    QString getUserAgent();

};

#endif // GANALYTICS_H
