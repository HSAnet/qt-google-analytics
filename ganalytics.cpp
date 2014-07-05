#include "ganalytics.h"
#include <QQueue>
#include <QTimer>
#include <QFile>
#include <QScreen>
#include <QGuiApplication>
#include <QSettings>
#include <QUuid>
#include <QStandardPaths>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QDateTime>


struct QueryBuffer
{
    QUrlQuery postQuery;
    QDateTime time;
};

/**
 * Class Private
 * Private members and functions.
 */
class GAnalytics::Private : public QObject
{
    Q_OBJECT
public:
    explicit Private(GAnalytics *parent = 0);
    ~Private();

    QNetworkAccessManager networkManager;
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
    bool isSending;
    const static int fourHours = 4 * 60 * 60 * 1000;
    const static QString dateTimeFormat;

private:
    GAnalytics *q;

public:
    QUrlQuery buildStandardPostQuery(const QString &type);
    QString getScreenResolution();
    QString getUserAgent();
    QString getSystemInfo();
    QList<QString> persistMessageQueue();
    void readMessagesFromFile(QList<QString> dataList);
    QString getClientID();
    void enqueQueryWithCurrentTime(const QUrlQuery &query);
    QString removeNewLineSymbol(QByteArray &line);
    void setIsSending(bool doSend);

signals:
    void postNextMessage();

public slots:
    void postMessage();
    void postMessageFinished(QNetworkReply *reply);

};

const QString GAnalytics::Private::dateTimeFormat  = "yyyy,MM,dd-hh:mm::ss:zzz";

/**
 * Constructor
 * Constructs an object of class Private.
 * @param parent
 */
GAnalytics::Private::Private(GAnalytics *parent) :
    QObject(parent),
    request(QUrl("http://www.google-analytics.com/collect")),
    messagesFileName(".postMassages"),
    networkManager(parent),
    timer(parent),
    q(parent)
{
    messagesFilePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    clientID = getClientID();
    language = QLocale::system().name().toLower().replace("_", "-");
    //screenResolution = getScreenResolution();
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setHeader(QNetworkRequest::UserAgentHeader, getUserAgent());
    appName = qApp->applicationName();
    appVersion = qApp->applicationVersion();
    connect(&networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(postMessageFinished(QNetworkReply*)));
    connect(this, SIGNAL(postNextMessage()), this, SLOT(postMessage()));
    timer.start(30000);
    connect(&timer, SIGNAL(timeout()), this, SLOT(postMessage()));
}

/**
 * Destructor
 * Delete an object of class Private.
 */
GAnalytics::Private::~Private()
{

}

/**
 * Build the POST query. Adds all parameter to the query
 * which are used in every POST.
 * @param type      Type of POST message. The event which is to post.
 * @return query    Most used parameter in a query for a POST.
 */
QUrlQuery GAnalytics::Private::buildStandardPostQuery(const QString &type)
{
    QUrlQuery query;
    query.addQueryItem("v", "1");
    query.addQueryItem("tid", trackingID);
    query.addQueryItem("cid", clientID);
    query.addQueryItem("t", type);
    query.addQueryItem("vp", viewportSize);
    query.addQueryItem("sr", screenResolution);
    query.addQueryItem("ul", language);

    return query;
}
/**
 * Get devicese screen resolution.
 * @return      A QString like "800x600".
 */
QString GAnalytics::Private::getScreenResolution()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    QSize size = screen->size();

    return QString::number(size.width()) + "x" + QString::number(size.height());
}


/**
 * Try to gain information about the system where this application
 * is running. It needs to get the name and version of the operating
 * system, the language and screen resolution.
 * All this information will be send in POST messages.
 * @return agent        A QString with all the information formatted for a POST message.
 */
QString GAnalytics::Private::getUserAgent()
{
    QString locale = QLocale::system().name();
    QString system = getSystemInfo();

    return QString("%1/%2 (%3; %4) GAnalytics/1.0 (Qt/%5)").arg(appName).arg(appVersion).arg(system).arg(locale).arg(QT_VERSION_STR);
}


#ifdef Q_OS_MAC
/**
 * Only on Mac OS X
 * Get the Operating system name and version.
 * @return os   The operating system name and version in a string.
 */
QString GAnalytics::Private::getSystemInfo()
{
    QSysInfo::MacVersion version = QSysInfo::macVersion();
    QString os;
    switch (version) {
    case QSysInfo::MV_9:
        os = "Macintosh; Mac OS 9";
        break;
    case QSysInfo::MV_10_0:
        os = "Macintosh; Mac OS 10.0";
        break;
    case QSysInfo::MV_10_1:
        os = "Macintosh; Mac OS 10.1";
        break;
    case QSysInfo::MV_10_2:
        os = "Macintosh; Mac OS 10.2";
        break;
    case QSysInfo::MV_10_3:
        os = "Macintosh; Mac OS 10.3";
        break;
    case QSysInfo::MV_10_4:
        os = "Macintosh; Mac OS 10.4";
        break;
    case QSysInfo::MV_10_5:
        os = "Macintosh; Mac OS 10.5";
        break;
    case QSysInfo::MV_10_6:
        os = "Macintosh; Mac OS 10.6";
        break;
    case QSysInfo::MV_10_7:
        os = "Macintosh; Mac OS 10.7";
        break;
    case QSysInfo::MV_10_8:
        os = "Macintosh; Mac OS 10.8";
        break;
    case QSysInfo::MV_10_9:
        os = "Macintosh; Mac OS 10.9";
        break;
    case QSysInfo::MV_Unknown:
        os = "Macintosh; Mac OS unknown";
        break;
    case QSysInfo::MV_IOS_5_0:
        os = "iPhone; iOS 5.0";
        break;
    case QSysInfo::MV_IOS_5_1:
        os = "iPhone; iOS 5.1";
        break;
    case QSysInfo::MV_IOS_6_0:
        os = "iPhone; iOS 6.0";
        break;
    case QSysInfo::MV_IOS_6_1:
        os = "iPhone; iOS 6.1";
        break;
    case QSysInfo::MV_IOS_7_0:
        os = "iPhone; iOS 7.0";
        break;
    case QSysInfo::MV_IOS_7_1:
        os = "iPhone; iOS 7.1";
        break;
    case QSysInfo::MV_IOS:
        os = "iPhone; iOS unknown";
        break;
    default:
        os = "Macintosh";
        break;
    }
    return os;
}
#endif

#ifdef Q_OS_WIN
/**
 * Only on Windows
 * Get operating system and its version.
 * @return os   A QString containing the oprating systems name and version.
 */
QString GAnalytics::Private::getSystemInfo()
{
    QSysInfo::WindowsVersion version = QSysInfo::windowsVersion();
    QString os("Windows; ");
    switch (version) {
    case QSysInfo::WV_95:
        os += "Win 95";
        break;
    case QSysInfo::WV_98:
        os += "Win 98";
        break;
    case QSysInfo::WV_Me:
        os += "Win ME";
        break;
    case QSysInfo::WV_NT:
        os += "Win NT";
        break;
    case QSysInfo::WV_2000:
        os += "Win 2000";
        break;
    case QSysInfo::WV_2003:
        os += "Win Server 2003";
        break;
    case QSysInfo::WV_VISTA:
        os += "Win Vista";
        break;
    case QSysInfo::WV_WINDOWS7:
        os += "Win 7";
        break;
    case QSysInfo::WV_WINDOWS8:
        os += "Win 8";
        break;
    case QSysInfo::WV_WINDOWS8_1:
        os += "Win 8.1";
        break;
    default:
        os = "Windows; unknown";
        break;
    }
    return os;
}
#endif

#ifdef Q_OS_LINUX
#include <sys/utsname.h>

/**
 * Only on Unix systems.
 * Get operation system name and version.
 * @return os       A QString with the name and version of the operating system.
 */
QString GAnalytics::Private::getSystemInfo()
{
    struct utsname buf;
    uname(&buf);
    QString system(buf.sysname);
    QString release(buf.release);

    return system + "; " + release;
}
#endif


/**
 * The message queue contains a list of QueryBuffer object.
 * QueryBuffer holds a QUrlQuery object and a QDateTime object.
 * These both object are freed from the buffer object and
 * inserted as QString objects in a QList.
 * @return dataList     The list with concartinated queue data.
 */
QList<QString> GAnalytics::Private::persistMessageQueue()
{
    QList<QString> dataList;
    foreach (QueryBuffer buffer, messageQueue) {
        dataList << buffer.postQuery.toString();
        dataList << buffer.time.toString(dateTimeFormat);
    }

    return dataList;
}

/**
 * Reads persistent messages from a file.
 * Gets all message data as a QList<QString>.
 * Two lines in the list build a QueryBuffer object.
 */
void GAnalytics::Private::readMessagesFromFile(QList<QString> dataList)
{
    while (! dataList.isEmpty())
    {
        QString queryString = dataList.takeFirst();
        QString dateString = dataList.takeFirst();
        QUrlQuery query;
        query.setQuery(queryString);
        QDateTime dateTime = QDateTime::fromString(dateString, dateTimeFormat);
        QueryBuffer buffer;
        buffer.postQuery = query;
        buffer.time = dateTime;
        messageQueue.enqueue(buffer);
    }
}


/**
 * Get the client id.
 * Client id once created is stored in application settings.
 * @return clientID         A string with the client id.
 */
QString GAnalytics::Private::getClientID()
{
    QSettings settings;
    QString clientID;
    if (! settings.contains("GAnalytics-cid"))
    {
        clientID = QUuid::createUuid().toString();
        settings.setValue("GAnalytics-cid", clientID);
    }
    else
    {
        clientID = settings.value("GAnalytics-cid").toString();
    }

    return clientID;
}

/**
 * Takes a QUrlQuery object and wrapp it together with
 * a QTime object into a QueryBuffer struct. These struct
 * will be stored in the message queue.
 * @param query
 */
void GAnalytics::Private::enqueQueryWithCurrentTime(const QUrlQuery &query)
{
    QueryBuffer buffer;
    buffer.postQuery = query;
    buffer.time = QDateTime::currentDateTime();

    messageQueue.enqueue(buffer);
}

/**
* Takes a QByteArray which contains a new line symbol at the end.
* The "\n" symbol at the end will be removed.
* @param line
* @return       Returns a QString without end line symbol.
*/
QString GAnalytics::Private::removeNewLineSymbol(QByteArray &line)
{
    int pos = line.size() - 1;
    line = line.remove(pos, 1);

    return QString(line);
}

/**
 * Change status of class. Emit signal that status was changed.
 * @param doSend
 */
void GAnalytics::Private::setIsSending(bool doSend)
{
    if (doSend)
        timer.stop();
    else
        timer.start();
    if (isSending != doSend)
        emit q->statusSendingChanged();
    isSending = doSend;
}


/**
 * CONSTRUCTOR  GAnalytics
 * ------------------------------------------------------------------------------------------------------------
 * Constructs the GAnalytics Object.
 * @param parent        The application which uses this object.
 * @param trackingID
 * @param clientID
 * @param withGet       Determines wheather the messages are send with GET or POST.
 */
GAnalytics::GAnalytics(const QString &trackingID, QObject *parent) :
    QObject(parent),
    d(new Private(this))
{
    setTrackingID(trackingID);
}

/**
 * Destructor of class GAnalytics.
 */
GAnalytics::~GAnalytics()
{
    delete d;
}

// SETTER and GETTER
void GAnalytics::setViewportSize(const QString &viewportSize)
{
    d->viewportSize = viewportSize;
    emit viewportSizeChanged();
}

QString GAnalytics::viewportSize() const
{
    return d->viewportSize;
}

void GAnalytics::setLanguage(const QString &language)
{
    d->language = language;
    emit languageChanged();
}

QString GAnalytics::language() const
{
    return d->language;
}

void GAnalytics::setTrackingID(const QString &trackingID)
{
    d->trackingID = trackingID;
    emit trackingIDChanged();
}

QString GAnalytics::trackingID() const
{
    return d->trackingID;
}

void GAnalytics::setMessagesFilePath(const QString &path)
{
    d->messagesFileName = path;
    emit messagesFilePathChanged();
}

QString GAnalytics::messagesFilePath() const
{
    return d->messagesFilePath;
}

void GAnalytics::setMessagesFileName(const QString &name)
{
    d->messagesFileName = name;
    emit messagesFileNameChanged();
}

QString GAnalytics::messagesFileName() const
{
    return d->messagesFileName;
}

void GAnalytics::setSendInterval(const int mseconds)
{
    d->timer.setInterval(mseconds);
    emit sendIntervalChanged();
}

int GAnalytics::sendInterval() const
{
    return (d->timer.interval());
}

bool GAnalytics::statusSending() const
{
    return d->isSending;
}

/**
 * SentAppview is called when the user changed the applications view.
 * These action of the user should be noticed and reported. Therefore
 * a QUrlQuery is build in this method. It holts all the parameter for
 * a http POST. The UrlQuery will be stored in a message Queue.
 * @param appName
 * @param appVersion
 * @param screenName
 */
void GAnalytics::sendAppview(const QString screenName)
{
    QUrlQuery query = d->buildStandardPostQuery("appview");
    if (! screenName.isEmpty())
    {
        query.addQueryItem("cd", screenName);
    }
    query.addQueryItem("an", d->appName);
    query.addQueryItem("av", d->appVersion);

    d->enqueQueryWithCurrentTime(query);
}

/**
 * This method is called whenever a button was pressed in the application.
 * A query for a POST message will be created to report this event. The
 * created query will be stored in a message queue.
 * @param eventCategory
 * @param eventAction
 * @param eventLabel
 * @param eventValue
 */
void GAnalytics::sendEvent(const QString category, const QString action, const QString label, const QVariant value)
{
    QUrlQuery query = d->buildStandardPostQuery("event");
    query.addQueryItem("an", d->appName);
    query.addQueryItem("av", d->appVersion);
    if (! category.isEmpty())
        query.addQueryItem("ec", category);
    if (! action.isEmpty())
        query.addQueryItem("ea", action);
    if (! label.isEmpty())
        query.addQueryItem("el", label);
    if (value.isValid())
        query.addQueryItem("ev", value.toString());

    d->enqueQueryWithCurrentTime(query);
}

/**
 * Method is called after an exception was raised. It builds a
 * query for a POST message. These query will be stored in a
 * message queue.
 * @param exceptionDescription
 * @param exceptionFatal
 */
void GAnalytics::sendException(const QString &exceptionDescription, const bool exceptionFatal)
{
    QUrlQuery query = d->buildStandardPostQuery("exception");
    query.addQueryItem("exd", exceptionDescription);
    if (exceptionFatal)
    {
        query.addQueryItem("exf", "1");
    }
    else
    {
        query.addQueryItem("exf", "0");
    }

    d->enqueQueryWithCurrentTime(query);
}

/**
 * Session ends. This event will be sent by a POST message.
 * Query is setup in this method and stored in the message
 * queue.
 */
void GAnalytics::endSession()
{
    QUrlQuery query = d->buildStandardPostQuery("event");
    query.addQueryItem("sc", "end");

    d->enqueQueryWithCurrentTime(query);
}

/**
 * This function is called by a timer interval.
 * The function tries to send a messages from the queue.
 * If message was successfully send then this function
 * will be called back to send next message.
 * If message queue contains more than one message then
 * the connection will kept open.
 * The message POST is asyncroniously when the server
 * answered a signal will be emitted.
 */
void GAnalytics::Private::postMessage()
{
    if (messageQueue.isEmpty())
    {
        setIsSending(false);
        return;
    }
    else
    {
        setIsSending(true);
    }
    QString connection = "close";
    if (messageQueue.count() > 1)
    {
        connection = "keep-alive";
    }
    QueryBuffer buffer = messageQueue.head();
    QDateTime sendTime = QDateTime::currentDateTime();
    qint64 timeDiff = buffer.time.msecsTo(sendTime);
    if(timeDiff > fourHours)
    {
        // to old.
        messageQueue.dequeue();
        emit postMessage();
    }
    buffer.postQuery.addQueryItem("qt", QString::number(timeDiff));
    request.setRawHeader("Connection", connection.toUtf8());
    request.setHeader(QNetworkRequest::ContentLengthHeader, buffer.postQuery.toString().length());
    networkManager.post(request, buffer.postQuery.query(QUrl::EncodeUnicode).toUtf8());
}

/**
 * NetworkAccsessManager has finished to POST a message.
 * If POST message was successfully send then the message
 * query should be removed from queue.
 * SIGNAL "postMessage" will be emitted to send next message
 * if there is any.
 * If message couldn't be send then next try is when the
 * timer emits its signal.
 * @param replay    Replay to the http POST.
 */
void GAnalytics::Private::postMessageFinished(QNetworkReply *reply)
{
    int httpStausCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (httpStausCode < 200 || httpStausCode > 299)
    {
        // An error ocurred.
        setIsSending(false);
        return;
    }
    QueryBuffer remove = messageQueue.dequeue();
    emit postNextMessage();
    reply->deleteLater();
}


/**
 * Qut stream to persist class GAnalytics.
 * @param outStream
 * @param analytics
 * @return
 */
QDataStream &operator<<(QDataStream &outStream, const GAnalytics &analytics)
{
    outStream << analytics.d->persistMessageQueue();
}


/**
 * In stream to read GAnalytics from file.
 * @param inStream
 * @param analytics
 * @return
 */
QDataStream &operator >>(QDataStream &inStream, GAnalytics &analytics)
{
    QList<QString> dataList;
    inStream >> dataList;
    analytics.d->readMessagesFromFile(dataList);
}

#include "ganalytics.moc"
