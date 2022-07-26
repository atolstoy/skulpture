/*
 * configmanager.h - Skulpture window decoration for KDE
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

#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H 1


/*-----------------------------------------------------------------------*/

#include <KDE/KConfigGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QSpinBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QRadioButton>

class ConfigManager
{
    public:
        ConfigManager() {
            /* */
        }

        ~ConfigManager() {
            /* */
        }

    public:
        void addWidgets(QWidget *parent) {
            typedef QList<QWidget *> WidgetList;
            WidgetList widgets = parent->findChildren<QWidget *>(QRegExp(QLatin1String("^cm_")));
            for (WidgetList::const_iterator i = widgets.constBegin(); i != widgets.constEnd(); ++i) {
                QWidget *widget = *i;
                items.append(ConfigItem(widget, QVariant()));
                items.last().configDefault = items.last().value();
            }
        }

        void connectConfigChanged(QObject *receiver, const char slot[]) const {
            for (ItemList::const_iterator item = items.constBegin(); item != items.constEnd(); ++item) {
                item->connectValueChanged(receiver, slot);
            }
        }

        void save(KConfigGroup &configGroup) const {
            for (ItemList::const_iterator item = items.constBegin(); item != items.constEnd(); ++item) {
                QVariant value = item->value();
                if (!value.isNull()) {
                    if (value == item->configDefault) {
                        configGroup.deleteEntry(item->configLabel());
                    } else {
                        configGroup.writeEntry(item->configLabel(), value);
                    }
                }
            }
        }

        void load(const KConfigGroup &configGroup) const {
            for (ItemList::const_iterator item = items.constBegin(); item != items.constEnd(); ++item) {
                item->setValue(((ConfigItem) *item).configSaved = configGroup.readEntry(item->configLabel(), item->configDefault.isNull() ? QString() : item->configDefault));
            }
        }

        bool hasChanged() const {
            for (ItemList::const_iterator item = items.constBegin(); item != items.constEnd(); ++item) {
                if (item->configSaved != item->value()) {
                    return true;
                }
            }
            return false;
        }

        void defaults() const {
            for (ItemList::const_iterator item = items.constBegin(); item != items.constEnd(); ++item) {
                item->setValue(item->configDefault);
            }
        }

    private:
        class ConfigItem
        {
            public:
                ConfigItem(QWidget *widget, const QVariant &def = QVariant())
                : configWidget(widget), configDefault(def) {
                    /* */
                }

            public:
                void setValue(const QVariant &value) const;
                QVariant value() const;
                void connectValueChanged(QObject *receiver, const char slot[]) const;
                QString configLabel() const;
                QString radioLabel() const;

            public:
                QWidget *configWidget;
                QVariant configDefault;
                QVariant configSaved;
        };

    private:
        typedef QList<ConfigItem> ItemList;

    private:
        ItemList items;
};


void ConfigManager::ConfigItem::setValue(const QVariant &value) const
{
    if (QCheckBox *widget = qobject_cast<QCheckBox *>(configWidget)) {
        widget->setChecked(value.toBool());
    } else if (QSpinBox *widget = qobject_cast<QSpinBox *>(configWidget)) {
        widget->setValue(value.toInt());
    } else if (QDoubleSpinBox *widget = qobject_cast<QDoubleSpinBox *>(configWidget)) {
        widget->setValue(value.toDouble());
    } else if (QRadioButton *widget = qobject_cast<QRadioButton *>(configWidget)) {
        widget->setChecked(value.toString() == radioLabel());
    }
}


QVariant ConfigManager::ConfigItem::value() const
{
    if (QCheckBox *widget = qobject_cast<QCheckBox *>(configWidget)) {
        return widget->isChecked();
    } else if (QSpinBox *widget = qobject_cast<QSpinBox *>(configWidget)) {
        return widget->value();
    } else if (QDoubleSpinBox *widget = qobject_cast<QDoubleSpinBox *>(configWidget)) {
        return widget->value();
    } else if (QRadioButton *widget = qobject_cast<QRadioButton *>(configWidget)) {
        return widget->isChecked() ? radioLabel() : QVariant();
    }
    return QVariant();
}


void ConfigManager::ConfigItem::connectValueChanged(QObject *receiver, const char slot[]) const
{
    if (QCheckBox *widget = qobject_cast<QCheckBox *>(configWidget)) {
        QObject::connect(widget, SIGNAL(toggled(bool)), receiver, slot);
    } else if (QSpinBox *widget = qobject_cast<QSpinBox *>(configWidget)) {
        QObject::connect(widget, SIGNAL(valueChanged(int)), receiver, slot);
    } else if (QDoubleSpinBox *widget = qobject_cast<QDoubleSpinBox *>(configWidget)) {
        QObject::connect(widget, SIGNAL(valueChanged(double)), receiver, slot);
    } else if (QRadioButton *widget = qobject_cast<QRadioButton *>(configWidget)) {
        QObject::connect(widget, SIGNAL(toggled(bool)), receiver, slot);
    }
}


QString ConfigManager::ConfigItem::configLabel() const
{
    QString cmName = configWidget->objectName().mid(3);
    if (qobject_cast<QRadioButton *>(configWidget)) {
        int index = cmName.lastIndexOf(QChar('_', 0));
        cmName = cmName.left(index);
    }
    return cmName;
}


QString ConfigManager::ConfigItem::radioLabel() const
{
    QString cmName = configWidget->objectName().mid(3);
    int index = cmName.lastIndexOf(QChar('_', 0));
    return cmName.mid(index + 1);
}


/*-----------------------------------------------------------------------*/

#endif


