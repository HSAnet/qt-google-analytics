#include "ganalytics.h"

GAnalytics::GAnalytics(QCoreApplication *parent, QString trackingID, QString clientID, bool withGet) :
    QObject(parent),
    trackingID(trackingID),
    clientID(clientID),
    networkManager(parent)
{
}

void GAnalytics::sendAppview(QString appName, QString appVersion, QString screenName) const
{
}
