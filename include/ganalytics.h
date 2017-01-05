#pragma once

#include <QObject>
#include <QVariantMap>
#include <QUrlQuery>
#include <QDateTime>
#include <QTimer>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QQueue>

class GAnalyticsWorker;

class GAnalytics : public QObject
{
	Q_OBJECT
	Q_ENUMS(LogLevel)
    Q_PROPERTY(QString trackingID READ trackingID WRITE setTrackingID)
    Q_PROPERTY(QString clientID READ clientID WRITE setClientID)
    Q_PROPERTY(QString viewportSize READ viewportSize WRITE setViewportSize)
    Q_PROPERTY(int sendInterval READ sendInterval WRITE setSendInterval)
    Q_PROPERTY(LogLevel logLevel READ logLevel WRITE setLogLevel)

public:
	explicit GAnalytics(QObject *parent = 0);
	explicit GAnalytics(const QString &trackingID, const QString &clientID, QObject *parent = 0);
	~GAnalytics();

public:
	enum LogLevel
	{
		Debug,
		Info,
		Error
	};

	void setTrackingID(const QString& trackingID);
	QString trackingID() const;

	void setClientID(const QString& clientID);
	QString clientID() const;

	void setLogLevel(LogLevel logLevel);
	LogLevel logLevel() const;

	// Getter and Setters
	void setViewportSize(const QString &viewportSize);
	QString viewportSize() const;

	void setLanguage(const QString &language);
	QString language() const;

	void setAnonymizeIPs(bool anonymize);
	bool anonymizeIPs();

	void setSendInterval(int milliseconds);
	int sendInterval() const;

	void enable(bool state = true);
	bool isEnabled();

	/// Get or set the network access manager. If none is set, the class creates its own on the first request
	void setNetworkAccessManager(QNetworkAccessManager *networkAccessManager);
	QNetworkAccessManager *networkAccessManager() const;

public slots:
	void sendScreenView(const QString &screenName, const QVariantMap &customValues = QVariantMap());
	void sendEvent(const QString &category, const QString &action, const QString &label = QString(), const QVariant &value = QVariant(),
				   const QVariantMap &customValues = QVariantMap());
	void sendException(const QString &exceptionDescription, bool exceptionFatal = true, const QVariantMap &customValues = QVariantMap());
	void startSession();
	void endSession();

private:
	GAnalyticsWorker *d;

	friend QDataStream &operator<<(QDataStream &outStream, const GAnalytics &analytics);
	friend QDataStream &operator>>(QDataStream &inStream, GAnalytics &analytics);
};

QDataStream &operator<<(QDataStream &outStream, const GAnalytics &analytics);
QDataStream &operator>>(QDataStream &inStream, GAnalytics &analytics);

struct QueryBuffer
{
	QUrlQuery postQuery;
	QDateTime time;
};

class GAnalyticsWorker : public QObject
{
Q_OBJECT

public:
	explicit GAnalyticsWorker(GAnalytics *parent = 0);

	GAnalytics *q;

	QNetworkAccessManager *networkManager = nullptr;

	QQueue<QueryBuffer> m_messageQueue;
	QTimer m_timer;
	QNetworkRequest m_request;
	GAnalytics::LogLevel m_logLevel;

	QString m_trackingID;
	QString m_clientID;
	QString m_userID;
	QString m_appName;
	QString m_appVersion;
	QString m_language;
	QString m_screenResolution;
	QString m_viewportSize;

	bool m_anonymizeIPs = false;
	bool m_isEnabled = false;
	int m_timerInterval = 30000;

	const static int fourHours = 4 * 60 * 60 * 1000;
	const static QLatin1String dateTimeFormat;

public:
	void logMessage(GAnalytics::LogLevel level, const QString &message);

	QUrlQuery buildStandardPostQuery(const QString &type);
	QString getScreenResolution();
	QString getUserAgent();
	QList<QString> persistMessageQueue();
	void readMessagesFromFile(const QList<QString> &dataList);

	void enqueQueryWithCurrentTime(const QUrlQuery &query);
	void enable(bool state);

public slots:
	void postMessage();
	void postMessageFinished();
};