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
#include <QApplication>
#include <QDesktopWidget>
#include <QFile>

class GAnalytics : public QObject
{
    Q_OBJECT
public:
    explicit GAnalytics(QCoreApplication *parent, const QString trackingID, const QString clientID);
    ~GAnalytics();

private:
    QNetworkAccessManager networkManager;
    QQueue<QUrlQuery> messageQueue;
    QTimer timer;
    QNetworkRequest request;
    QString trackingID;
    QString clientID;
    QString appName;
    QString appVersion;
    QString language;
    QString screenResolution;
    QString messagesFileName;

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
    QUrlQuery buildStandardPostQuery(const QString type);
    QString getScreenResolution();
    QString getUserAgent();
    QString getSystemInfo();
    void storeMessageQueue();
    void readMessagesFromFile();

};

#endif // GANALYTICS_H
