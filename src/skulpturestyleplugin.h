#ifndef SKULPTURE_STYLE_PLUGIN_H
#define SKULPTURE_STYLE_PLUGIN_H

#include <QtWidgets/QStylePlugin>

class SkulptureStylePlugin : public QStylePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QStyleFactoryInterface" FILE "skulpturestyle.json")

public:
    SkulptureStylePlugin() {}

    QStringList keys() const;
    QStyle *create(const QString &key);
};

#endif
