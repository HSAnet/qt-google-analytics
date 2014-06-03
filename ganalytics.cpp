#include "ganalytics.h"
#include <QUrlQuery>

/**
 * Constructs the GAnalytics Object.
 * @param parent        The application which uses this object.
 * @param trackingID
 * @param clientID
 * @param withGet       Determines wheather the messages are send with GET or POST.
 */
GAnalytics::GAnalytics(QCoreApplication *parent, QString trackingID, QString clientID, bool withGet) :
    QObject(parent),
    trackingID(trackingID),
    clientID(clientID),
    networkManager(this),
    baseUrl("http://www.google-analytics.com/collect")
{
}

/**
 * SentAppview is called when the user changed the applications view.
 * These action of the user should be noticed and reported. Therefore
 * an Url is build in this method. The Url will be stored in a message
 * Queue.
 * @param appName
 * @param appVersion
 * @param screenName
 */
void GAnalytics::sendAppview(QString appName, QString appVersion, QString screenName)
{
    QUrl messageUrl(baseUrl);
    QUrlQuery query;
    query.addQueryItem("v", "1");
    query.addQueryItem("tid", trackingID);
    query.addQueryItem("cid", clientID);
    query.addQueryItem("t", "appview");
    if(! userID.isEmpty())
        query.addQueryItem("uid", userID);
    if(! userIPAddress.isEmpty())
        query.addQueryItem("uip", userIPAddress);
    // Didn't implement: screenResolution, viewPortSize, userLanguage
    if(! this->appName.isEmpty())
        query.addQueryItem("an", this->appName);
    else if(! appName.isEmpty())
        query.addQueryItem("an", appName);
    if(! this->appVersion.isEmpty())
        query.addQueryItem("av", this->appVersion);
    else if(! appVersion.isEmpty())
        query.addQueryItem("av", appVersion);
    messageUrl.setQuery(query);
    messageQueue.enqueue(messageUrl);
}
