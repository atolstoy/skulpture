/*
 * configmanager.h - Classical Three-Dimensional Artwork for Qt 4
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

#include <QtCore/QSettings>
#include <QtGui/QCheckBox>
#include <QtGui/QSpinBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QAbstractSlider>
#include <QtGui/QLineEdit>
#include <QtGui/QComboBox>

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
                if (widget->isEnabled()) {
                    items.append(ConfigItem(widget, QVariant()));
                    items.last().configDefault = items.last().value();
                }
            }
        }

        void connectConfigChanged(QObject *receiver, const char slot[]) const {
            for (ItemList::const_iterator item = items.constBegin(); item != items.constEnd(); ++item) {
                item->connectValueChanged(receiver, slot);
            }
        }

        void save(QSettings &settings) const {
            for (ItemList::const_iterator item = items.constBegin(); item != items.constEnd(); ++item) {
                QVariant value = item->value();
                if (!value.isNull()) {
                    if (value == item->configDefault) {
                        settings.remove(item->configLabel());
                    } else {
                        settings.setValue(item->configLabel(), item->value());
                    }
                }
            }
        }

        void load(const QSettings &settings) const {
            for (ItemList::const_iterator item = items.constBegin(); item != items.constEnd(); ++item) {
                item->setValue(((ConfigItem) *item).configSaved = settings.value(item->configLabel(), item->configDefault));
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
                QString comboLabel(int index) const;

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
    } else if (QAbstractSlider *widget = qobject_cast<QAbstractSlider *>(configWidget)) {
        widget->setValue(value.toInt());
    } else if (QLineEdit *widget = qobject_cast<QLineEdit *>(configWidget)) {
        widget->setText(value.toString());
    } else if (QComboBox *widget = qobject_cast<QComboBox *>(configWidget)) {
        int index;
        if (widget->isEditable()) {
            index = widget->findText(value.toString());
            if (index < 0) {
                if (widget->isEditable()) {
                    widget->addItem(value.toString());
                    index = widget->findText(value.toString());
                } else {
                    return;
                }
            }
        } else {
            index = 0;
            for (int i = widget->count(); --i >= 0; ) {
                if (value.toString() == comboLabel(i)) {
                    index = i;
                    break;
                }
            }
        }
        widget->setCurrentIndex(index);
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
    } else if (QAbstractSlider *widget = qobject_cast<QAbstractSlider *>(configWidget)) {
        return widget->value();
    } else if (QLineEdit *widget = qobject_cast<QLineEdit *>(configWidget)) {
        return widget->text();
    } else if (QComboBox *widget = qobject_cast<QComboBox *>(configWidget)) {
        if (widget->isEditable()) {
            return widget->currentText();
        } else {
            return comboLabel(widget->currentIndex());
        }
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
    } else if (QAbstractSlider *widget = qobject_cast<QAbstractSlider *>(configWidget)) {
        QObject::connect(widget, SIGNAL(valueChanged(int)), receiver, slot);
    } else if (QLineEdit *widget = qobject_cast<QLineEdit *>(configWidget)) {
        QObject::connect(widget, SIGNAL(textChanged(QString)), receiver, slot);
    } else if (QComboBox *widget = qobject_cast<QComboBox *>(configWidget)) {
        if (widget->isEditable()) {
            QObject::connect(widget, SIGNAL(editTextChanged(QString)), receiver, slot);
        } else {
            QObject::connect(widget, SIGNAL(currentIndexChanged(int)), receiver, slot);
        }
    }
}


QString ConfigManager::ConfigItem::configLabel() const
{
    QString cmName = configWidget->objectName().mid(3);
    if (QComboBox *widget = qobject_cast<QComboBox *>(configWidget)) {
        if (!widget->isEditable()) {
            int index = -1;
            for (int i = widget->count(); --i >= 0;) {
                index = cmName.lastIndexOf(QChar('_', 0), index - 1);
            }
            cmName = cmName.left(index);
        }
    }
    return cmName.replace(QChar('_', 0), QChar('/', 0));
}


QString ConfigManager::ConfigItem::comboLabel(int index) const
{
    QString cmName = configWidget->objectName().mid(3);
    int scanIndex = -1;
    int lastIndex = -1; // avoid warning
    for (int i = qobject_cast<QComboBox *>(configWidget)->count() - index; --i >= 0; ) {
        lastIndex = scanIndex;
        scanIndex = cmName.lastIndexOf(QChar('_', 0), scanIndex - 1);
    }
    ++scanIndex;
    if (lastIndex == -1) {
        cmName = cmName.mid(scanIndex);
    } else {
        cmName = cmName.mid(scanIndex, lastIndex - scanIndex);
    }
    return cmName;
}


/*-----------------------------------------------------------------------*/

#endif


