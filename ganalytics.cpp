#include "ganalytics.h"

/**
 * Constructs the GAnalytics Object.
 * @param parent        The application which uses this object.
 * @param trackingID
 * @param clientID
 * @param withGet       Determines wheather the messages are send with GET or POST.
 */
GAnalytics::GAnalytics(QCoreApplication *parent, const QString trackingID) :
    QObject(parent),
    trackingID(trackingID),
    request(QUrl("http://www.google-analytics.com/collect")),
    timer(this),
    networkManager(this),
    messagesFilePath("~/"),
    messagesFileName(".postMassages")
{
    clientID = getClientID();
    language = QLocale::system().nativeLanguageName();
    screenResolution = getScreenResolution();
    readMessagesFromFile();
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setHeader(QNetworkRequest::UserAgentHeader, getUserAgent());
    appName = qApp->applicationName();
    appVersion = parent->applicationVersion();
    connect(&networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(postMessageFinished(QNetworkReply*)));
    connect(this, SIGNAL(postNextMessage()), this, SLOT(postMessage()));
    timer.start(30000);
    connect(&timer, SIGNAL(timeout()), this, SLOT(postMessage()));
}

/**
 * Destructor of class GAnalytics.
 */
GAnalytics::~GAnalytics()
{
    if (! messageQueue.isEmpty())
    {
        storeMessageQueue();
    }
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
    QUrlQuery query = buildStandardPostQuery("appview");
    if (! screenName.isEmpty())
    {
        query.addQueryItem("cd", screenName);
    }
    query.addQueryItem("an", this->appName);
    query.addQueryItem("av", this->appVersion);

    messageQueue.enqueue(query);
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
void GAnalytics::sendEvent(const QString eventCategory, const QString eventAction, const QString eventLabel, const QVariant eventValue)
{
    QUrlQuery query = buildStandardPostQuery("event");
    query.addQueryItem("an", appName);
    query.addQueryItem("av", appVersion);
    if (! eventCategory.isEmpty())
        query.addQueryItem("ec", eventCategory);
    if (! eventAction.isEmpty())
        query.addQueryItem("ea", eventAction);
    if (! eventLabel.isEmpty())
        query.addQueryItem("el", eventLabel);
    if (eventValue.isValid())
        query.addQueryItem("ev", eventValue.toString());

    messageQueue.enqueue(query);
}

/**
 * Method is called after an exception was raised. It builds a
 * query for a POST message. These query will be stored in a
 * message queue.
 * @param exceptionDescription
 * @param exceptionFatal
 */
void GAnalytics::sendException(const QString exceptionDescription, const bool exceptionFatal)
{
    QUrlQuery query = buildStandardPostQuery("exception");
    query.addQueryItem("exd", exceptionDescription);
    if (exceptionFatal)
    {
        query.addQueryItem("exf", "1");
    }
    else
    {
        query.addQueryItem("exf", "0");
    }

    messageQueue.enqueue(query);
}

/**
 * Session ends. This event will be sent by a POST message.
 * Query is setup in this method and stored in the message
 * queue.
 */
void GAnalytics::endSession()
{
    QUrlQuery query = buildStandardPostQuery("event");
    query.addQueryItem("sc", "end");

    messageQueue.enqueue(query);
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
void GAnalytics::postMessage()
{
    if (messageQueue.isEmpty())
    {
        return;
    }
    QString connection = "close";
    if (messageQueue.count() > 1)
    {
        connection = "keep-alive";
    }
    QUrlQuery param = messageQueue.head();
    request.setRawHeader("Connection", connection.toUtf8());
    request.setHeader(QNetworkRequest::ContentLengthHeader, param.toString().length());
    networkManager.post(request, param.query(QUrl::EncodeUnicode).toUtf8());
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
void GAnalytics::postMessageFinished(QNetworkReply *replay)
{
    int httpStausCode = replay->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (httpStausCode < 200 || httpStausCode > 299)
    {
        // An error ocurred.
        return;
    }
    QUrlQuery remove = messageQueue.dequeue();
    emit postNextMessage();
    replay->deleteLater();
}

/**
 * Build the POST query. Adds all parameter to the query
 * which are used in every POST.
 * @param type      Type of POST message. The event which is to post.
 * @return query    Most used parameter in a query for a POST.
 */
QUrlQuery GAnalytics::buildStandardPostQuery(const QString type)
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
QString GAnalytics::getScreenResolution()
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
QString GAnalytics::getUserAgent()
{
    QString locale = QLocale::system().name();
    QString system = getSystemInfo();

    return appName + "/" + appVersion + " (" + system + "; " + locale + ") GAnalytics/1.0 (Qt/" QT_VERSION_STR ")";
}

#ifdef Q_OS_MAC
/**
 * Only on Mac OS X
 * Get the Operating system name and version.
 * @return os   The operating system name and version in a string.
 */
QString GAnalytics::getSystemInfo()
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

/**
 * Store the content of the message queue to a file.
 * Messages which could not be send are stored into
 * a file. They can be read when application starts
 * again.
 */
void GAnalytics::storeMessageQueue()
{
    QString filePath(messagesFilePath + messagesFileName);
    QFile file(filePath);
    file.open(QIODevice::WriteOnly);
    QDataStream outStream(&file);
    while (! messageQueue.isEmpty())
    {
        QUrlQuery msgQuery = messageQueue.dequeue();
        QString queryString = msgQuery.query();
        outStream << queryString;
    }
    file.close();
}

/**
 * Reads stored messages from a file.
 */
void GAnalytics::readMessagesFromFile()
{
    QString filePath(messagesFilePath + messagesFileName);
    if (! QFile::exists(filePath))
    {
        return;
    }
    QFile file(filePath);
    file.open(QIODevice::ReadWrite);
    QDataStream inStream(&file);
    while (! inStream.atEnd())
    {
        QString queryString;
        inStream >> queryString;
        QUrlQuery msgQuery;
        msgQuery.setQuery(queryString);
        messageQueue.enqueue(msgQuery);
    }
    file.close();
    file.remove();
}

/**
 * Get the client id.
 * Client id once created is stored in application settings.
 * @return clientID         A string with the client id.
 */
QString GAnalytics::getClientID()
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

#ifdef Q_OS_WIN
/**
 * Only on Windows
 * Get operating system and its version.
 * @return os   A QString containing the oprating systems name and version.
 */
QString GAnalytics::getSystemInfo()
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
QString GAnalytics::getSystemInfo()
{
    struct utsname buf;
    uname(&buf);
    QString system(buf.sysname);
    QString release(buf.release);

    return system + "; " + release;
}
#endif

