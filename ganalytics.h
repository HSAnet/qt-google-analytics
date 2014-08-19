#ifndef GANALYTICS_H
#define GANALYTICS_H

#include <QObject>
#include <QVariant>

class GAnalytics : public QObject
{
    Q_OBJECT
    Q_ENUMS(LogLevel)
    Q_PROPERTY(LogLevel logLevel READ logLevel WRITE setLogLevel NOTIFY logLevelChanged)
    Q_PROPERTY(QString viewportSize READ viewportSize WRITE setViewportSize NOTIFY viewportSizeChanged)
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)
    Q_PROPERTY(QString trackingID READ trackingID WRITE setTrackingID NOTIFY trackingIDChanged)
    Q_PROPERTY(int sendInterval READ sendInterval WRITE setSendInterval NOTIFY sendIntervalChanged)
    Q_PROPERTY(bool isSending READ isSending NOTIFY statusSendingChanged)

public:
    explicit GAnalytics(QObject *parent = 0);
    explicit GAnalytics(const QString &trackingID, QObject *parent = 0);
    ~GAnalytics();

public:
    enum LogLevel
    {
        Debug,
        Info,
        Error
    };

    void setLogLevel(LogLevel logLevel);
    LogLevel logLevel() const;

    // Getter and Setters
    void setViewportSize(const QString &viewportSize);
    QString viewportSize() const;

    void setLanguage(const QString &language);
    QString language() const;

    void setTrackingID(const QString &trackingID);
    QString trackingID() const;

    void setSendInterval(int milliseconds);
    int sendInterval() const;

    bool isSending() const;

public slots:
    void sendAppView(const QString &screenName = QString());
    void sendEvent(const QString &category = QString(),
                   const QString &action = QString(),
                   const QString &label = QString(),
                   const QVariant &value = QVariant());
    void sendException(const QString &exceptionDescription, bool exceptionFatal = true);
    void endSession();


signals:
    void logLevelChanged();
    void viewportSizeChanged();
    void languageChanged();
    void trackingIDChanged();
    void sendIntervalChanged();
    void statusSendingChanged();

private:
    class Private;
    Private *d;

    friend QDataStream& operator<<(QDataStream &outStream, const GAnalytics &analytics);
    friend QDataStream& operator>>(QDataStream &inStream, GAnalytics &analytics);

};

QDataStream& operator<<(QDataStream &outStream, const GAnalytics &analytics);
QDataStream& operator>>(QDataStream &inStream, GAnalytics &analytics);

#endif // GANALYTICS_H
