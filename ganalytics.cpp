#include "ganalytics.h"

/**
 * Constructs the GAnalytics Object.
 * @param parent        The application which uses this object.
 * @param trackingID
 * @param clientID
 * @param withGet       Determines wheather the messages are send with GET or POST.
 */
GAnalytics::GAnalytics(const QCoreApplication *parent, const QString trackingID, const QString clientID) :
    QObject(parent),
    trackingID(trackingID),
    clientID(clientID),
    requestUrl(QUrl("http://www.google-analytics.com/collect")),
    timer(this),
    networkManager(this)
{
    requestUrl.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    appName = qApp->applicationName();
    appVersion = parent->applicationVersion();
    userAgent = "User-Agent: Mozilla/5.0 (iPad; U; CPU OS 3_2_1 like Mac OS X; en-us) AppleWebKit/531.21.10 (KHTML, like Gecko) Mobile/7B405";
    connect(&networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(postMessageFinished(QNetworkReply*)));
    connect(this, SIGNAL(postNextMessage()), this, SLOT(postMessage()));
    timer.start(30000);
    connect(&timer, SIGNAL(timeout()), this, SLOT(postMessage()));
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
    QUrlQuery query;
    query.addQueryItem("v", "1");
    query.addQueryItem("tid", trackingID);
    query.addQueryItem("cid", clientID);
    query.addQueryItem("t", "appview");
    if (! userID.isEmpty())
        query.addQueryItem("uid", userID);
    if (! userIPAddress.isEmpty())
        query.addQueryItem("uip", userIPAddress);
    // Didn't implement: screenResolution, viewPortSize, userLanguage, screenName
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
    QUrlQuery query;
    query.addQueryItem("v", "1");
    query.addQueryItem("tid", trackingID);
    query.addQueryItem("cid", clientID);
    query.addQueryItem("t", "event");
    if (! appName.isEmpty())
        query.addQueryItem("an", appName);
    if (! appVersion.isEmpty())
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
    QUrlQuery query;
    query.addQueryItem("v", "1");
    query.addQueryItem("tid", trackingID);
    query.addQueryItem("cid", clientID);
    query.addQueryItem("t", "exception");
    if (! exceptionDescription.isEmpty())
        query.addQueryItem("exd", exceptionDescription);
    if (exceptionFatal)
        query.addQueryItem("exf", "1");
    else
        query.addQueryItem("exf", "0");

    messageQueue.enqueue(query);
}

/**
 * Session ends. This event will be sent by a POST message.
 * Query is setup in this method and stored in the message
 * queue.
 */
void GAnalytics::endSession()
{
    QUrlQuery query;
    query.addQueryItem("v", "1");
    query.addQueryItem("tid", trackingID);
    query.addQueryItem("t", "event");
    query.addQueryItem("sc", "end");

    messageQueue.enqueue(query);
}

/**
 * This function is called by a timer every 30 seconds.
 * Its the time intervall to send messages. Function
 * tries to send a messages from the queue. It will be
 * called back if successfully send the message and there
 * are still any message in the queue.
 * The message POST is asyncroniously when ready it sends
 * a signal.
 */
void GAnalytics::postMessage()
{
    printf("Try to send ...");      // just to test
    if (messageQueue.isEmpty())
    {
        printf(" nothing to send.\n");  // just to test
        return;
    }
    printf("\n");                       // just to test
    QUrlQuery param = messageQueue.head();
    requestUrl.setHeader(QNetworkRequest::UserAgentHeader, userAgent);
    //requestUrl.setHeader(QNetworkRequest::ContentLengthHeader, param.toString().length());
    networkManager.post(requestUrl, param.query(QUrl::EncodeUnicode).toUtf8());
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
        printf("Error : %i\n", httpStausCode);              // just for testing
        // An error is occured. Is logging requiered ????
        return;
    }
    QUrlQuery remove = messageQueue.dequeue();
    printf("Send status : OK.\n");                          // just for testing
    emit postNextMessage();
    replay->deleteLater();
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
    QSysInfo::MacVersion macVersion = QSysInfo::macVersion();
}
