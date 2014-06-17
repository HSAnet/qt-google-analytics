#ifndef GANALYTICS_H
#define GANALYTICS_H

#include <QObject>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QDateTime>

struct QueryBuffer
{
    QUrlQuery postQuery;
    QDateTime time;
};

class GAnalytics : public QObject
{
    Q_OBJECT
public:
    explicit GAnalytics(const QString &trackingID, QObject *parent = 0);
    ~GAnalytics();

public:
    // Getter and Setter
    void setViewportSize(const QString &viewportSize);
    QString getViewportSize() const;
    void setLanguage(const QString &language);
    QString getLangugae() const;
    void setTrackingID(const QString &trackingID);
    QString getTrackingID() const;
    void setMessagesFilePath(const QString &path);
    QString getMessagesFilePath() const;
    void setMessagesFileName(const QString &name);
    QString getMessagesFileName() const;
    void setTimerIntervall(const int seconds);
    int getTimerIntervall() const;

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
    class Private;
    Private *analyticsPrivate;

};

#endif // GANALYTICS_H
