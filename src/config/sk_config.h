/*
 * sk_config.h - Classical Three-Dimensional Artwork for Qt 4
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

#ifndef SKULPTURE_CONFIG_H
#define SKULPTURE_CONFIG_H 1


/*-----------------------------------------------------------------------*/

#include <QtGui/QWidget>
#include "ui_sk_dialog.h"
#include "configmanager.h"

class QSettings;
class QMdiSubWindow;
class KXmlGuiWindow;
class KAboutData;
class KComponentData;

class SkulptureStyleConfig : public QWidget, private Ui::SkulptureStyleDialog
{
	Q_OBJECT

	public:
		explicit SkulptureStyleConfig(QWidget* parent = 0);
		virtual ~SkulptureStyleConfig();

		virtual QSize sizeHint() const;

	Q_SIGNALS:
		void changed(bool);

	public Q_SLOTS:
		void save();
		void defaults();

	protected Q_SLOTS:
		void updateChanged();
		void updatePreview();

	private Q_SLOTS:
		void subWindowActivated(QMdiSubWindow *window);

    protected:
        void showEvent(QShowEvent *);

	private:
		void init();

	private:
            ConfigManager configManager;
            QLayout *dialogLayout;
            KXmlGuiWindow *previewWindow;
            KAboutData *aboutData;
            KComponentData *componentData;
            QStyle *oldStyle;
};


/*-----------------------------------------------------------------------*/

#include <QtGui/QStyle>

struct SkMethodData
{
	int version;
};


struct SkMethodDataSetSettingsFileName : public SkMethodData
{
	// in version 1
	QString fileName;
};


class SkulptureStyle : public QStyle
{
	Q_OBJECT

	public:
		// internal, reserved for future use
		enum SkulpturePrivateMethod {
			SPM_SupportedMethods = 0,
			SPM_SetSettingsFileName = 1
		};

	public Q_SLOTS:
		int skulpturePrivateMethod(SkulpturePrivateMethod /*id*/, void */*data*/ = 0) {
			return 0;
		}
};


/*-----------------------------------------------------------------------*/

#endif


