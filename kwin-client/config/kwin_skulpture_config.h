/*
 * kwin_skulpture_config.h - Skulpture window decoration for KDE
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

#ifndef KWIN_SKULPTURE_CONFIG_H
#define KWIN_SKULPTURE_CONFIG_H 1


/*-----------------------------------------------------------------------*/

#include "ui_kwin_skulpture_dialog.h"

class SkulptureConfigUi : public QWidget, public Ui::KWinSkulptureDialog
{
    Q_OBJECT

    public:
        explicit SkulptureConfigUi(QWidget *parent = 0) : QWidget(parent) {
            setupUi(this);
            verticalLayout->setMargin(0);
            cm_BorderSize->updateGeometry();
            cm_BottomBorderSize->updateGeometry();
        }
};


/*-----------------------------------------------------------------------*/

#include "configmanager.h"

class SkulptureConfig : public QObject
{
    Q_OBJECT

    public:
        explicit SkulptureConfig(KConfig *config, QWidget *parent = 0);
        virtual ~SkulptureConfig();

    public Q_SLOTS:
        void load(const KConfigGroup &configGroup);
        void save(KConfigGroup &configGroup);
        void defaults();

    Q_SIGNALS:
        void changed();

    protected Q_SLOTS:
        void slotSelectionChanged();

    private:
        void init(QWidget *parent);

    private:
        ConfigManager configManager;
        KConfig *skulptureConfig;
        SkulptureConfigUi *ui;
};


/*-----------------------------------------------------------------------*/

#endif


