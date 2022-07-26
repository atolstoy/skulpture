/*
 * skulpture_shortcuts.cpp
 *
 */

#include "skulpture_p.h"
#include <QtGui/QKeyEvent>
#include <QtGui/QFocusEvent>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QToolBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QTabBar>
#include <QtWidgets/QApplication>


/*-----------------------------------------------------------------------*/
/**
 * Class to manage the applications keyboard shortcuts
 *
 * It acts as an eventfilter for the application, and does the following:
 *
 *	* the shortcuts are only underlined when the Alt key is pressed, and
 *		the underline is removed once it is released.
 *	* on many widgets the focus frame is not displayed if the focus was
 *		received using the mouse. It is, however, still displayed if
 *		you use the Tab key.
 *
 * Additionally, this class is responsible for blanking the mouse pointer:
 *
 *      * when the tablet pen leaves proximity
 *
 * Any other keyboard shortcut and focus issues should also be handled here.
 *
 */


/*-----------------------------------------------------------------------*/

bool ShortcutHandler::underlineShortcut(const QWidget *widget) const
{
	if (widget && widget->isEnabled()) {
		if (alt_pressed.contains(widget->window())) {
			return true;
		}
		if (qobject_cast<const QMenuBar *>(widget)) {
			if (widget->hasFocus()) {
				return true;
			}
			QList<QWidget *> children = widget->findChildren<QWidget *>();
            Q_FOREACH (QWidget *child, children) {
				if (child->hasFocus()) {
					return true;
				}
			}
		}
		if (qobject_cast<const QMenu *>(widget)) {
			return true;
		}
	}
	return false;
}


static inline bool hasShortcut(QWidget *widget)
{
	if (qobject_cast<const QAbstractButton *>(widget) // && (qobject_cast<const QAbstractButton *>(widget))->text.contains(QChar('&', 0))
	 || qobject_cast<const QLabel *>(widget)
	 || qobject_cast<const QDockWidget *>(widget)
	 || qobject_cast<const QToolBox *>(widget)
//	 || qobject_cast<const QMenu *>(widget)
	 || qobject_cast<const QMenuBar *>(widget)
	 || qobject_cast<const QGroupBox *>(widget) // && (qobject_cast<const QGroupBox *>(widget))->isCheckable()
	 || qobject_cast<const QTabBar *>(widget)) {
		return true;
	}
	return false;
}


static inline void updateShortcuts(QWidget *widget)
{
	QList<QWidget *> children = widget->findChildren<QWidget *>();
    Q_FOREACH (QWidget *child, children) {
		if (child->isVisible() && hasShortcut(child)) {
			child->update();
		}
	}
}


bool ShortcutHandler::eventFilter(QObject *watched, QEvent *event)
{
    if (!watched->isWidgetType()) {
        switch (event->type()) {
            case QEvent::TabletEnterProximity:
                if (tabletCursorState != TabletCursor) {
                    if (tabletCursorState != DefaultCursor) {
                        QApplication::restoreOverrideCursor();
                    }
                    tabletCursorState = DefaultCursor;
                }
                break;
            case QEvent::TabletLeaveProximity:
                if (tabletCursorState != BlankCursor) {
                    if (tabletCursorState != DefaultCursor) {
                        QApplication::restoreOverrideCursor();
                    }
                    QApplication::setOverrideCursor(Qt::BlankCursor);
                    tabletCursorState = BlankCursor;
                }
                break;
            default:
                break;
        }
        return QObject::eventFilter(watched, event);
    }
	QWidget *widget = reinterpret_cast<QWidget *>(watched);

	switch (event->type()) {
                case QEvent::MouseMove:
                    if (tabletCursorState != DefaultCursor) {
                        QApplication::restoreOverrideCursor();
                        tabletCursorState = DefaultCursor;
                    }
                    break;
                case QEvent::FocusIn: {
				Qt::FocusReason reason = ((QFocusEvent *) event)->reason();
				if (reason != Qt::TabFocusReason && reason != Qt::BacktabFocusReason) {
					QWidget *window = widget->window();
					window->setAttribute(Qt::WA_KeyboardFocusChange, false);
				}
			}
			break;
		case QEvent::KeyPress:
			if (((QKeyEvent *) event)->key() == Qt::Key_Alt) {
				QWidget *window = widget->window();
				if (!alt_pressed.contains(window)) {
					alt_pressed.append(window);
					window->installEventFilter(this);
					updateShortcuts(window);
				}
			}
			break;
		case QEvent::Close:
			if (widget->isWindow()) {
				alt_pressed.removeAll(widget);
				widget->removeEventFilter(this);
			}
			break;
		case QEvent::WindowDeactivate:
			if (widget->isWindow()) {
				alt_pressed.removeAll(widget);
				widget->removeEventFilter(this);
				updateShortcuts(widget);
			}
			break;
		case QEvent::KeyRelease:
			if (((QKeyEvent *) event)->key() == Qt::Key_Alt) {
				QWidget *window = widget->window();
				if (alt_pressed.contains(window)) {
					alt_pressed.removeAll(window);
					window->removeEventFilter(this);
					updateShortcuts(window);
				}
			}
			break;
		default:
			break;
	}
	return QObject::eventFilter(watched, event);
}


/*-----------------------------------------------------------------------*/

ShortcutHandler::ShortcutHandler(QObject *parent)
	: QObject(parent), tabletCursorState(DefaultCursor)
{
	init();
}


ShortcutHandler::~ShortcutHandler()
{
    if (tabletCursorState != DefaultCursor) {
        tabletCursorState = DefaultCursor;
        QApplication::restoreOverrideCursor();
    }
}


