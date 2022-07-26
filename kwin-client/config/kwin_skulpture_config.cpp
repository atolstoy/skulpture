/*
 * kwin_skulpture_config.cpp - Skulpture window decoration for KDE
 *
 * Copyright (c) 2008 Christoph Feck <christoph@maxiom.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "kwin_skulpture_config.h"
#include <KDE/KConfig>
#include <KDE/KConfigGroup>
#include <KDE/KGlobal>
#include <KDE/KLocale>


/*-----------------------------------------------------------------------*/

extern "C" KDE_EXPORT QObject *allocate_config(KConfig *conf, QWidget *parent)
{
    return new SkulptureConfig(conf, parent);
}


/*-----------------------------------------------------------------------*/

SkulptureConfig::SkulptureConfig(KConfig *config, QWidget *parent)
    : QObject(parent)
{
    Q_UNUSED(config);
    init(parent);
}


void SkulptureConfig::init(QWidget *parent)
{
    skulptureConfig = new KConfig(QLatin1String("kwinskulpturerc"));
    KGlobal::locale()->insertCatalog(QLatin1String("kwin_clients"));
    ui = new SkulptureConfigUi(parent);
    configManager.addWidgets(ui);
    load(KConfigGroup(skulptureConfig, "General"));
    configManager.connectConfigChanged(this, SLOT(slotSelectionChanged()));
}


SkulptureConfig::~SkulptureConfig()
{
    delete skulptureConfig;
    delete ui;
}


void SkulptureConfig::load(const KConfigGroup &configGroup)
{
    Q_UNUSED(configGroup);
    configManager.load(KConfigGroup(skulptureConfig, "General"));
}


void SkulptureConfig::save(KConfigGroup &configGroup)
{
    Q_UNUSED(configGroup);
    KConfigGroup skulptureConfigGroup(skulptureConfig, "General");
    configManager.save(skulptureConfigGroup);
    skulptureConfig->sync();
}


void SkulptureConfig::defaults()
{
    configManager.defaults();
}


void SkulptureConfig::slotSelectionChanged()
{
    if (configManager.hasChanged()) {
        Q_EMIT changed();
    }
}


/*-----------------------------------------------------------------------*/

#include "kwin_skulpture_config.moc"


