#ifndef GANALYTICS_H
#define GANALYTICS_H

#include <QObject>
#include <QNetworkReply>

class GAnalytics : public QObject
{
    Q_OBJECT
public:
    explicit GAnalytics(const QString &trackingID, QObject *parent = 0);
    ~GAnalytics();

public:
    // Getter and Setter
    void setViewportSize(const QString &viewportSize);
    QString viewportSize() const;
    void setLanguage(const QString &language);
    QString langugae() const;
    void setTrackingID(const QString &trackingID);
    QString trackingID() const;
    void setMessagesFilePath(const QString &path);
    QString messagesFilePath() const;
    void setMessagesFileName(const QString &name);
    QString messagesFileName() const;
    void setTimerIntervall(const int mseconds);
    int timerIntervall() const;
    bool isSendingMessages() const;

signals:
    void postNextMessage();

public slots:
    void sendAppview(const QString screenName = QString());
    void sendEvent(const QString category = QString(),
                   const QString action = QString(),
                   const QString label = QString(),
                   const QVariant value = QVariant());
    void sendException(const QString &exceptionDescription, const bool exceptionFatal = true);
    void endSession();
    void postMessage();
    void postMessageFinished(QNetworkReply *reply);

private:
    class Private;
    Private *d;

};

#endif // GANALYTICS_H
