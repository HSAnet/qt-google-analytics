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
#include <QFile>
#include <QScreen>
#include <QGuiApplication>
#include <QSettings>
#include <QUuid>
#include <QStandardPaths>
#include <QDateTime>

class GAnalytics : public QObject
{
    Q_OBJECT
public:
    explicit GAnalytics(const QString &trackingID, QObject *parent = 0);
    ~GAnalytics();

private:
    QNetworkAccessManager networkManager;
    struct QueryBuffer { QUrlQuery postQuery; QDateTime time; };
    QQueue<QueryBuffer> messageQueue;
    QTimer timer;
    QNetworkRequest request;
    QString trackingID;
    QString clientID;
    QString appName;
    QString appVersion;
    QString language;
    QString screenResolution;
    QString messagesFilePath;
    QString messagesFileName;
    QString viewportSize;

public:
    // Getter and Setter
    void setViewportSize(const QString &viewportSize)        { this->viewportSize = viewportSize; }
    QString getViewportSize() const                         { return viewportSize; }
    void setLanguage(const QString &language)                { this->language = language; }
    QString getLangugae() const                             { return language; }
    void setTrackingID(const QString &trackingID)            { this->trackingID = trackingID; }
    QString getTrackingID() const                           { return trackingID; }
    void setMessagesFilePath(const QString &path)            { messagesFilePath = path; }
    QString getMessagesFilePath() const                     { return messagesFilePath; }
    void setMessagesFileName(const QString &name)            { messagesFileName = name; }
    QString getMessagesFileName() const                     { return messagesFileName; }
    void setTimerIntervall(const int seconds)               { timer.setInterval(seconds * 1000); }
    int getTimerIntervall() const                           { return timer.interval() / 1000; }

signals:
    void postNextMessage();

public slots:
    void sendAppview(const QString screenName = QString());
    void sendEvent(const QString eventCategory = QString(),
                   const QString eventAction = QString(),
                   const QString eventLabel = QString(),
                   const QVariant eventValue = QVariant());
    void sendException(const QString &exceptionDescription, const bool exceptionFatal = true);
    void endSession();
    void postMessage();
    void postMessageFinished(QNetworkReply *replay);

private:
    QUrlQuery buildStandardPostQuery(const QString &type);
    QString getScreenResolution();
    QString getUserAgent();
    QString getSystemInfo();
    void persistMessageQueue();
    void readMessagesFromFile();
    QString getClientID();
    void enqueQueryWithCurrentTime(QUrlQuery &query);
    QUrlQuery getQueryWithQueueTime(QueryBuffer &buffer);
    QString removeNewLineSymbol(QByteArray &line);

};

#endif // GANALYTICS_H
