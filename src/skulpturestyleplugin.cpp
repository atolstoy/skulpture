#include "skulpturestyleplugin.h"
#include "skulpture.h"

QStringList SkulptureStylePlugin::keys() const
{
    return QStringList() << QLatin1String("Skulpture");
}

QStyle *SkulptureStylePlugin::create(const QString &key)
{
    if (key.toLower() == QLatin1String("skulpture")) {
        return new SkulptureStyle;
    }
    return 0;
}

