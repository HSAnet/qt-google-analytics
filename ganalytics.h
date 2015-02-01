#ifndef GANALYTICS_H
#define GANALYTICS_H

#include <QObject>
#include <QVariant>
#include <QNetworkAccessManager>

#ifdef QT_QML_LIB
#include <QQmlParserStatus>
#endif // QT_QML_LIB

class GAnalytics : public QObject
#ifdef QT_QML_LIB
                 , public QQmlParserStatus
#endif // QT_QML_LIB
{
    Q_OBJECT
#ifdef QT_QML_LIB
    Q_INTERFACES(QQmlParserStatus)
#endif // QT_QML_LIB
    Q_ENUMS(LogLevel)
    Q_PROPERTY(LogLevel logLevel READ logLevel WRITE setLogLevel NOTIFY logLevelChanged)
    Q_PROPERTY(QString viewportSize READ viewportSize WRITE setViewportSize NOTIFY viewportSizeChanged)
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)
    Q_PROPERTY(QString trackingID READ trackingID WRITE setTrackingID NOTIFY trackingIDChanged)
    Q_PROPERTY(int sendInterval READ sendInterval WRITE setSendInterval NOTIFY sendIntervalChanged)
    Q_PROPERTY(bool isSending READ isSending NOTIFY isSendingChanged)

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

    /// Get or set the network access manager. If none is set, the class creates its own on the first request
    void setNetworkAccessManager(QNetworkAccessManager *networkAccessManager);
    QNetworkAccessManager *networkAccessManager() const;

#ifdef QT_QML_LIB
    // QQmlParserStatus interface
    void classBegin();
    void componentComplete();
#endif // QT_QML_LIB

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
    void isSendingChanged(bool isSending);

private:
    class Private;
    Private *d;

    friend QDataStream& operator<<(QDataStream &outStream, const GAnalytics &analytics);
    friend QDataStream& operator>>(QDataStream &inStream, GAnalytics &analytics);
};

QDataStream& operator<<(QDataStream &outStream, const GAnalytics &analytics);
QDataStream& operator>>(QDataStream &inStream, GAnalytics &analytics);

#endif // GANALYTICS_H
