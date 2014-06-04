#include "ganalytics.h"

/**
 * Constructs the GAnalytics Object.
 * @param parent        The application which uses this object.
 * @param trackingID
 * @param clientID
 * @param withGet       Determines wheather the messages are send with GET or POST.
 */
GAnalytics::GAnalytics(QCoreApplication *parent, QString trackingID, QString clientID) :
    QObject(parent),
    trackingID(trackingID),
    clientID(clientID),
    requestUrl(QUrl("http://www.google-analytics.com/collect")),
    timer(this),
    networkManager(this)
{
    connect(&timer, SIGNAL(timeout()), this, SLOT(postMessage()));
    timer.start(30000);
    connect(&networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(postMessageFinished(QNetworkReply*)));
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
void GAnalytics::sendAppview(QString appName, QString appVersion, QString screenName)
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
    if (! this->appName.isEmpty())
        query.addQueryItem("an", this->appName);
    else if (! appName.isEmpty())
        query.addQueryItem("an", appName);
    if (! this->appVersion.isEmpty())
        query.addQueryItem("av", this->appVersion);
    else if (! appVersion.isEmpty())
        query.addQueryItem("av", appVersion);
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
    if (messageQueue.isEmpty())
    {
        return;
    }
    QUrlQuery param = messageQueue.head();
    networkManager.post(requestUrl, param.query().toUtf8());
}

/**
 * NetworkAccsessManager has finished to POST a message.
 * If POST message was successfully send then the query
 * should be removed from queue.
 * SIGNAL "postMessage" will be emitted to send next message
 * if there is any.
 * @param replay    Replay to the http POST.
 */
void GAnalytics::postMessageFinished(QNetworkReply *replay)
{
    int httpStausCode = replay->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (httpStausCode < 200 || httpStausCode > 299)
    {
        return;
    }
    QUrlQuery remove = messageQueue.dequeue();
    emit postMessage();
    replay->deleteLater();
}
