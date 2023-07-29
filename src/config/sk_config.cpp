/*
 * sk_config.cpp - Classical Three-Dimensional Artwork for Qt 4
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

#include "sk_config.h"
#include "configmanager.h"
#include <QtCore/QDir>
#include <QtCore/QUuid>
#include <QtGui/QCloseEvent>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QMdiSubWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QStyleFactory>
#include <KConfigCore/KConfig>
#include <KConfigWidgets/KStandardAction>
#include <KCoreAddons/KAboutData>
#include <KWidgetsAddons/KAcceleratorManager>
#include <KXmlGui/KActionCollection>
#include <KXmlGui/KHelpMenu>


/*-----------------------------------------------------------------------*/

extern "C" Q_DECL_EXPORT QWidget* allocate_kstyle_config(QWidget* parent)
{
	return new SkulptureStyleConfig(parent);
}


/*-----------------------------------------------------------------------*/

#include "ui_sk_preview1.h"

class Preview1Window : public QDialog, private Ui::SkulpturePreview1
{
	public:
		explicit Preview1Window(QWidget *parent = 0);

	public:
		virtual void closeEvent(QCloseEvent *e);
                QLayout *dialogLayout() const {
                    return verticalLayout_9;
                }

	private:
		void init();
};


Preview1Window::Preview1Window(QWidget *parent)
	: QDialog(parent)
{
	init();
}


void Preview1Window::closeEvent(QCloseEvent *e)
{
	e->ignore();
}


void Preview1Window::init()
{
	setWindowFlags(Qt::Widget);
	setupUi(this);
}


/*-----------------------------------------------------------------------*/

#include "ui_sk_preview2.h"

class Preview2Window : public KXmlGuiWindow, private Ui::SkulpturePreview2
{
	public:
		explicit Preview2Window(QWidget *parent, const KAboutData &aboutData);

	public:
		virtual void closeEvent(QCloseEvent *e);

	private:
		void init(const KAboutData &aboutData);
};


Preview2Window::Preview2Window(QWidget *parent, const KAboutData &aboutData)
	: KXmlGuiWindow(parent)
{
	init(aboutData);
}


void Preview2Window::closeEvent(QCloseEvent *e)
{
	e->ignore();
}


static const KStandardAction::StandardAction standardAction[] = {
    KStandardAction::New, KStandardAction::Open, KStandardAction::OpenRecent, KStandardAction::Save, KStandardAction::SaveAs, KStandardAction::Revert, KStandardAction::Close,
//    Print, PrintPreview, Mail,
    KStandardAction::Quit, // KStandardAction::Undo, KStandardAction::Redo,
    KStandardAction::Cut, KStandardAction::Copy, KStandardAction::Paste,
//    SelectAll, Deselect,
    KStandardAction::Find, KStandardAction::FindNext, KStandardAction::FindPrev, KStandardAction::Replace,
//    ActualSize, FitToPage, FitToWidth, FitToHeight, ZoomIn, ZoomOut, Zoom, Redisplay,
    KStandardAction::Up, KStandardAction::Back, KStandardAction::Forward, KStandardAction::Home,
//    Prior, Next, Goto, GotoPage, GotoLine,
//    FirstPage, LastPage, DocumentBack, DocumentForward,
//    AddBookmark, EditBookmarks, Spelling,
     KStandardAction::ShowMenubar, KStandardAction::ShowToolbar, KStandardAction::ShowStatusbar,
     /*SaveOptions, */ KStandardAction::KeyBindings,
     /*Preferences, */ KStandardAction::ConfigureToolbars,
//    Help, HelpContents, WhatsThis, ReportBug, AboutApp, AboutKDE,
//    TipofDay, ConfigureNotifications, FullScreen, Clear,
//    PasteText, SwitchApplicationLanguage,
     KStandardAction::ActionNone
};


void Preview2Window::init(const KAboutData &aboutData)
{
	setWindowFlags(Qt::Widget);
	setupUi(this);
        setHelpMenuEnabled(false);
        setComponentName(aboutData.componentName(), aboutData.componentName());
        for (uint i = 0; i < sizeof(standardAction) / sizeof(standardAction[0]); ++i) {
            if (standardAction[i] != KStandardAction::ActionNone) {
                actionCollection()->addAction(standardAction[i]);
            }
        }
        createGUI();
        //menuBar()->removeAction(menuBar()->actions().last());
        KHelpMenu *helpMenu = new KHelpMenu(this, aboutData, false);
        menuBar()->addMenu((QMenu *) helpMenu->menu());
        statusBar()->setSizeGripEnabled(true);
        //setToolButtonStyle(Qt::ToolButtonIconOnly);
}


/*-----------------------------------------------------------------------*/

void SkulptureStyleConfig::subWindowActivated(QMdiSubWindow *window)
{
	if (window) {
		QWidget *parent = window->parentWidget();
		if (parent) {
			const QList<QObject *> children = parent->children();
            Q_FOREACH (QObject *child, children) {
				QWidget *widget = qobject_cast<QWidget *>(child);
				if (widget) {
					widget->setWindowTitle(QLatin1String(child == window ? "Active Window" : "Inactive Window"));
				}
			}
		}
	}
}



void SkulptureStyleConfig::updatePreview()
{
	QStyle *style = QStyleFactory::create(QLatin1String("skulpture"));
	if (style) {
		int index = style->metaObject()->indexOfMethod("skulpturePrivateMethod(SkulpturePrivateMethod,void*)");
		if (index >= 0) {
			QUuid uuid = QUuid::createUuid();
			QDir tempDir = QDir::temp();
			// ### we may add a way to avoid the file completely.
			QString fileName = QLatin1String("skulpture-temp-settings-") + uuid.toString() + QLatin1String(".ini");
			QString absFileName = tempDir.absoluteFilePath(fileName);
			{
				QSettings s(absFileName, QSettings::IniFormat);
				configManager->save(s);
				// make visible in other process
				s.sync();
			}
			SkMethodDataSetSettingsFileName data;
			data.version = 1;
			data.fileName = absFileName;

			int result;
			int id = SkulptureStyle::SPM_SetSettingsFileName;
			void *dat = &data;
			void *param[] = {&result, &id, &dat};
			style->qt_metacall(QMetaObject::InvokeMetaMethod, index, param);

			tempDir.remove(fileName);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
			int margin = style->pixelMetric(QStyle::PM_LayoutLeftMargin, nullptr, this->window());
#else
			int margin = style->pixelMetric(QStyle::PM_DefaultTopLevelMargin);
#endif
			dialogLayout->setContentsMargins(margin, margin, margin, margin);
			QList<QMdiSubWindow *> windows = mdiArea->findChildren<QMdiSubWindow *>();
            Q_FOREACH (QMdiSubWindow *window, windows) {
                                window->setFocusPolicy(Qt::FocusPolicy(window->focusPolicy() & ~Qt::TabFocus));
				QList<QWidget *> widgets = window->widget()->findChildren<QWidget *>();
                Q_FOREACH (QWidget *widget, widgets) {
					// TODO: only update widgets that have changes in settings
					widget->setStyle(style);
                                        if (widget->inherits("QTextEdit") || (widget->parentWidget() && widget->parentWidget()->inherits("QTextEdit"))) {
                                            widget->setFocusPolicy(Qt::ClickFocus);
                                        } else {
                                            widget->setFocusPolicy(Qt::FocusPolicy(widget->focusPolicy() & ~Qt::TabFocus));
                                        }
				}
			}
		}
	}
        if (oldStyle) {
            delete oldStyle;
        }
        oldStyle = style;
}


/*-----------------------------------------------------------------------*/

SkulptureStyleConfig::SkulptureStyleConfig(QWidget *parent)
	: QWidget(parent), oldStyle(0)
{
	init();
}


enum PreviewPosition
{
    PP_Auto,
    PP_Hidden,
    PP_InTab,
    PP_Left,
    PP_Top,
    PP_Right,
    PP_Bottom
};


void SkulptureStyleConfig::init()
{
	configManager = new ConfigManager();
        aboutData = new KAboutData("skulpture", "Sculpture", "0.2.4",
            "Three-dimensional classical artwork.",
            KAboutLicense::GPL_V3,
            "(c) 2007-2010, Christoph Feck", "",
            "http://skulpture.maxiom.de/", "christoph@maxiom.de");

	setupUi(this);
        QList<QWidget *> children = tabWidget->findChildren<QWidget *>();
        Q_FOREACH (QWidget *child, children) {
            if (!(child->isEnabled())) {
                child->hide();
            }
        }
        QSize availableSize = QApplication::desktop()->availableGeometry(this).size();
        QSettings s(QSettings::IniFormat, QSettings::UserScope, QLatin1String("SkulptureStyle"), QLatin1String(""));
        QString previewPositionText = s.value(QLatin1String("ConfigDialog/PreviewPosition"), QLatin1String("auto")).toString().toLower();
        PreviewPosition previewPosition = PP_Auto;
        switch (previewPositionText.at(0).unicode()) {
            case 'a':
            default:
                previewPosition = PP_Auto;
                break;
            case 'l':
                previewPosition = PP_Left;
                break;
            case 't':
                previewPosition = PP_Top;
                break;
            case 'b':
                previewPosition = PP_Bottom;
                break;
            case 'r':
                previewPosition = PP_Right;
                break;
            case 'c': // center
            case 'i':
                previewPosition = PP_InTab;
                break;
            case 'n': // none
            case 'h':
                previewPosition = PP_Hidden;
                break;
        }
        if (previewPosition == PP_Auto) {
            if (availableSize.height() < 700) {
                if (availableSize.width() < 800) {
                    previewPosition = PP_InTab;
                } else {
                    previewPosition = PP_Left;
                }
            } else {
                previewPosition = PP_Top;
            }
        }
        switch (previewPosition) {
            case PP_Left:
                mdiArea->setMinimumSize(QSize(qMin(570, availableSize.width() - 400), 290));
                vboxLayout->setDirection(QBoxLayout::LeftToRight);
                break;
            case PP_Top:
                mdiArea->setMinimumSize(QSize(570, 290));
                break;
            case PP_Right:
                mdiArea->setMinimumSize(QSize(qMin(570, availableSize.width() - 400), 290));
                vboxLayout->setDirection(QBoxLayout::RightToLeft);
                break;
            case PP_Bottom:
                mdiArea->setMinimumSize(QSize(570, 290));
                vboxLayout->setDirection(QBoxLayout::BottomToTop);
                break;
            case PP_InTab:
                tabWidget->addTab(mdiArea, QLatin1String("Preview"));
                break;
            case PP_Hidden:
                mdiArea->hide();
                break;
            case PP_Auto: // avoid warning
                break;
        }
        mdiArea->setBackground(QColor(120, 120, 150));
        tabWidget->setCurrentIndex(0);

//	KConfig wm(QLatin1String("kwinskulpturerc"), KConfig::FullConfig);
//	KConfigGroup group = wm.group("WM");
//	QColor activeBackground;
//	QString c = group.readEntry("activeBackground");
//	qDebug() << "activeBackground = " << c;
	connect(this, SIGNAL(changed(bool)), this, SLOT(updatePreview()));
	connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow *)), this, SLOT(subWindowActivated(QMdiSubWindow *)));
        configManager->addWidgets(this);
        configManager->load(s);

        if (cm_General_TextShift->value() > 0) {
            cm_General_TextShift->setPrefix(QLatin1String("+"));
        } else {
            cm_General_TextShift->setPrefix(QString());
        }
#if 1
#if 1
	QMdiSubWindow *previewwindow2 = new QMdiSubWindow;
//	previewwindow2->setObjectName(QLatin1String("SkulpturePreviewWindow"));
//	previewwindow2->setProperty("sk_kwin_activeBackground",
	previewwindow2->setStyle(QStyleFactory::create(QLatin1String("skulpture")));
        previewWindow = new Preview2Window(previewwindow2, *aboutData);
	previewwindow2->setWidget(previewWindow);
	previewwindow2->setGeometry(107, 7, 450, 265);
#endif
#if 1
	QMdiSubWindow *previewwindow1 = new QMdiSubWindow;
//	previewwindow1->setObjectName(QLatin1String("SkulpturePreviewWindow"));
	previewwindow1->setStyle(QStyleFactory::create(QLatin1String("skulpture")));
        Preview1Window *preview = new Preview1Window(previewwindow1);
	previewwindow1->setWidget(preview);
	previewwindow1->setGeometry(7, 35, 450, 245);
        dialogLayout = preview->dialogLayout();
#endif
	updatePreview();
#else
	mdiArea->hide();
#endif
	mdiArea->addSubWindow(previewwindow2);
	mdiArea->addSubWindow(previewwindow1);
        KAcceleratorManager::manage(this);
        configManager->connectConfigChanged(this, SLOT(updateChanged()));
}


void SkulptureStyleConfig::showEvent(QShowEvent *)
{
        cm_General_TextShift->setFocus(Qt::ActiveWindowFocusReason);
}


QSize SkulptureStyleConfig::sizeHint() const
{
	return QSize(570, 0);
}


SkulptureStyleConfig::~SkulptureStyleConfig()
{
        if (previewWindow) {
            previewWindow->hide();
            previewWindow->setParent(0);
            previewWindow->deleteLater();
        }
        delete configManager;
        delete aboutData;
}


/*-----------------------------------------------------------------------*/

void SkulptureStyleConfig::save()
{
    QSettings s(QSettings::IniFormat, QSettings::UserScope, QLatin1String("SkulptureStyle"), QLatin1String(""));
    configManager->save(s);
}


void SkulptureStyleConfig::defaults()
{
    configManager->defaults();
}


void SkulptureStyleConfig::updateChanged()
{
    if (cm_General_TextShift->value() > 0) {
        cm_General_TextShift->setPrefix(QLatin1String("+"));
    } else {
        cm_General_TextShift->setPrefix(QString());
    }
    Q_EMIT changed(configManager->hasChanged());
}


/*-----------------------------------------------------------------------*/
