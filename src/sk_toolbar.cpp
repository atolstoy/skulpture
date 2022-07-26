/*
 * skulpture_toolbar.cpp
 *
 */

#include "skulpture_p.h"
#include <QtGui/QPainter>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QTabBar>


/*-----------------------------------------------------------------------*/

#define PAINT_SEPARATOR 0


/*-----------------------------------------------------------------------*/

extern void paintCachedGrip(QPainter *painter, const QStyleOption *option, QPalette::ColorRole bgrole);

void paintToolBarSeparator(QPainter *painter, const QStyleOptionToolBar *option)
{
#if 0
	int d = 3;
	QRect rect(QRect(option->rect).center() - QPoint(d / 2, d / 2), QSize(d, d));
	QStyleOption iOption;
	iOption.QStyleOption::operator=(*option);
	if (option->state & QStyle::State_Horizontal) {
		iOption.rect = rect.adjusted(1, 0, 1, 0);
	} else {
		iOption.rect = rect.adjusted(0, 1, 0, 1);
	}
	iOption.palette.setCurrentColorGroup(QPalette::Disabled);
//	iOption.state &= ~QStyle::State_Enabled;
	iOption.palette.setColor(QPalette::Button, iOption.palette.color(QPalette::Window));
	paintCachedGrip(painter, &iOption, QPalette::Window);
#else
#if PAINT_SEPARATOR
	QRect rect = option->rect;

	if (option->state & QStyle::State_Horizontal) {
	//	rect.adjust(2, 3, -2, -3);
		rect.adjust(2, -1, -2, 1);
	} else {
	//	rect.adjust(3, 2, -3, -2);
		rect.adjust(-1, 2, 1, -2);
	}
	paintThinFrame(painter, rect, option->palette, 60, -20);
#else
	Q_UNUSED(painter);
	Q_UNUSED(option);
#endif
#endif
}

/*-----------------------------------------------------------------------*/

void paintToolBarHandle(QPainter *painter, const QStyleOptionToolBar *option)
{
#if 1
	int d = 5;
	QRect rect(QRect(option->rect).center() - QPoint(d / 2, d / 2), QSize(d, d));
	QStyleOption iOption;
	iOption.QStyleOption::operator=(*option);
	iOption.rect = rect;
	iOption.palette.setCurrentColorGroup(QPalette::Disabled);
//	iOption.state &= ~QStyle::State_Enabled;
	iOption.palette.setColor(QPalette::Button, iOption.palette.color(QPalette::Window));
	paintCachedGrip(painter, &iOption, QPalette::Window);
#else
	QRect rect = option->rect;

	if (option->state & QStyle::State_Horizontal) {
		rect.adjust(2, 2, -2, -2);
#if PAINT_SEPARATOR
		rect.adjust(0, 1, 0, 1);
#endif
	} else {
		rect.adjust(2, 2, -2, -2);
#if PAINT_SEPARATOR
		rect.adjust(1, 0, 1, 0);
#endif
	}
	paintThinFrame(painter, rect.adjusted(-1, -1, 1, 1), option->palette, 60, -20);
	paintThinFrame(painter, rect, option->palette, -30, 80);
#endif
}


/*-----------------------------------------------------------------------*/

void paintPanelToolBar(QPainter *painter, const QStyleOptionToolBar *option)
{
#if 0
    QLinearGradient gradient(option->rect.topLeft(), option->rect.bottomLeft());
    gradient.setColorAt(0.0, option->palette.color(QPalette::Window).lighter(105));
    gradient.setColorAt(1.0, option->palette.color(QPalette::Window));
    painter->fillRect(option->rect, gradient);
    QRect r;
//    r = option->rect;
//    r.setTop(r.top() + r.height() - 1);
//    painter->fillRect(r, option->palette.color(QPalette::Window).darker(105));
    r = option->rect;
    r.setHeight(1);
    painter->fillRect(r, option->palette.color(QPalette::Window).darker(105));
//	painter->fillRect(option->rect, option->palette.color(QPalette::Window));
//	paintThinFrame(painter, option->rect, option->palette, -20, 60);
#endif
#if PAINT_SEPARATOR
	QRect r = option->rect;
	if (option->state & QStyle::State_Horizontal) {
		r.setHeight(2);
	} else {
		r.setWidth(2);
	}
	paintThinFrame(painter, r, option->palette, 60, -20);
#else
	Q_UNUSED(painter);
	Q_UNUSED(option);
#if 0
        if (option->state & QStyle::State_Horizontal) {
            paintThinFrame(painter, option->rect.adjusted(-1, 0, 1, 0), option->palette, 80, -30);
            painter->fillRect(option->rect.adjusted(0, 1, 0, -1), QColor(200, 210, 230));
            QLinearGradient toolBarGradient(option->rect.topLeft(), option->rect.bottomLeft());
            toolBarGradient.setColorAt(0.0, QColor(0, 0, 0, 30));
            toolBarGradient.setColorAt(0.05, QColor(0, 0, 0, 0));
#if 0
            toolBarGradient.setColorAt(0.5, QColor(0, 0, 0, 10));
            toolBarGradient.setColorAt(0.51, QColor(0, 0, 0, 30));
#endif
            toolBarGradient.setColorAt(0.8, QColor(0, 0, 0, 20));
            toolBarGradient.setColorAt(1.0, QColor(0, 0, 0, 0));
            painter->fillRect(option->rect.adjusted(0, 1, 0, -1), toolBarGradient);
        } else {
            paintThinFrame(painter, option->rect.adjusted(0, -1, 0, 1), option->palette, 80, -30);
        }
#else
//        painter->fillRect(option->rect, QColor(0, 0, 0, 10));
#endif
#endif
}


/*-----------------------------------------------------------------------*/

static inline bool inVerticalToolBar(const QStyleOption *option, const QWidget *widget)
{
	// ### option->state does not reflect orientation
	Q_UNUSED(option);
	bool verticalBar = false;

	if (widget && widget->parentWidget()) {
		const QToolBar *toolBar = qobject_cast<const QToolBar *>(widget->parentWidget());
		//	printf("toolbar: %p\n", toolBar);
		if (toolBar && toolBar->orientation() == Qt::Vertical) {
			verticalBar = true;
		}
	}
	return verticalBar;
}


/*-----------------------------------------------------------------------*/

extern void paintMenuTitle(QPainter *painter, const QStyleOptionToolButton *option, const QWidget *widget, const QStyle *style);

void paintToolButton(QPainter *painter, const QStyleOptionToolButton *option, const QWidget *widget, const QStyle *style)
{
	if (widget) {
		QTabBar *bar = qobject_cast<QTabBar *>(widget->parentWidget());
                if (bar) {
			// tabbar scroll button
			QStyleOptionToolButton opt = *option;
			if (int(bar->shape()) & 2) {
				opt.rect.adjust(4, 0, -4, -1);
			} else {
				opt.rect.adjust(0, 4, 0, -3);
			}
			painter->save();
			painter->setClipRect(opt.rect);
			painter->fillRect(opt.rect, option->palette.color(QPalette::Window));
			((QCommonStyle *) style)->QCommonStyle::drawComplexControl(QStyle::CC_ToolButton, &opt, painter, widget);
			painter->restore();
			return;
		} else if (widget->objectName() == QLatin1String("qt_menubar_ext_button") || widget->objectName() == QLatin1String("qt_toolbar_ext_button")) {
			QStyleOptionToolButton opt = *option;
			/* do not render menu arrow, because extension buttons already have an arrow */
			opt.features &= ~(QStyleOptionToolButton::Menu | QStyleOptionToolButton::HasMenu);
			((QCommonStyle *) style)->QCommonStyle::drawComplexControl(QStyle::CC_ToolButton, &opt, painter, widget);
			return;
		} else {
                    const QToolButton *button = qobject_cast<const QToolButton *>(widget);

                    if (button && button->isDown() && button->toolButtonStyle() == Qt::ToolButtonTextBesideIcon) {
                        if (widget->parentWidget() && widget->parentWidget()->inherits("QMenu")) {
                            paintMenuTitle(painter, option, widget, style);
                            return;
                        }
                    }
                }
	}
	if (option->features & QStyleOptionToolButton::HasMenu) {
		if (option->features & QStyleOptionToolButton::Menu) {
			if (option->subControls & QStyle::SC_ToolButton) {
				painter->save();
				QStyleOptionToolButton opt = *option;
                opt.rect = style->subControlRect(QStyle::CC_ToolButton, option, QStyle::SC_ToolButton, widget);
				// opt.features &= ~(QStyleOptionToolButton::Menu | QStyleOptionToolButton::HasMenu | QStyleOptionToolButton::Arrow);
				opt.arrowType = Qt::NoArrow;
				opt.subControls &= ~(QStyle::SC_ToolButtonMenu);
				opt.activeSubControls &= ~(QStyle::SC_ToolButtonMenu);
                                if (opt.state & QStyle::State_Sunken) {
                                    opt.state |= QStyle::State_MouseOver;
                                }
				if (inVerticalToolBar(option, widget)) {
					painter->setClipRect(opt.rect.adjusted(0, 0, 0, -1));
				} else {
					painter->setClipRect(opt.rect.adjusted(option->direction == Qt::LeftToRight ? 0 : 1, 0, option->direction == Qt::LeftToRight ? -1 : 0, 0));
				}
				((QCommonStyle *) style)->QCommonStyle::drawComplexControl(QStyle::CC_ToolButton, &opt, painter, widget);
				painter->restore();
			}
			if (option->subControls & QStyle::SC_ToolButtonMenu) {
				painter->save();
				QStyleOptionToolButton opt = *option;
				opt.rect = style->subControlRect(QStyle::CC_ToolButton, option, QStyle::SC_ToolButtonMenu, widget);
				QStyle::State state = option->state;
				state &= ~(QStyle::State_Sunken | QStyle::State_Raised);
				if (!(state & QStyle::State_AutoRaise) || (state & QStyle::State_MouseOver)) {
					state |= QStyle::State_Raised;
				}
                                {
                                    if (option->state & QStyle::State_Sunken) {
                                        state |= QStyle::State_Sunken;
                                    } else {
                                        if (!(option->activeSubControls & QStyle::SC_ToolButtonMenu)) {
                                            state &= ~QStyle::State_MouseOver;
                                        }
                                    }
                                }
				opt.state = state;
				if (inVerticalToolBar(option, widget)) {
					painter->setClipRect(opt.rect.adjusted(0, 1, 0, 0));
				} else {
					painter->setClipRect(opt.rect.adjusted(option->direction == Qt::LeftToRight ? 1 : 0, 0, option->direction == Qt::LeftToRight ? 0 : -1, 0));
				}
				if (state & (QStyle::State_Sunken | QStyle::State_On | QStyle::State_Raised)) {
					style->drawPrimitive(QStyle::PE_PanelButtonTool, &opt, painter, widget);
				}
				painter->restore();
				QRect r;
				if (inVerticalToolBar(option, widget)) {
					if (option->direction == Qt::LeftToRight) {
						r = QRect(opt.rect.right() - 9, opt.rect.top(), 7, opt.rect.height());
					} else {
						r = QRect(3, opt.rect.top(), 7, opt.rect.height());
					}
				} else {
					r = QRect(opt.rect.left(), opt.rect.bottom() - 9, opt.rect.width(), 7);
				}
				if (option->state & QStyle::State_Sunken) {
					if (option->direction == Qt::LeftToRight) {
						r.adjust(1, 1, 1, 1);
					} else {
						r.adjust(-1, 1, -1, 1);
					}
				}
				QFont font;
				font.setPixelSize(9);
				opt.fontMetrics = QFontMetrics(font);
				opt.rect = r;
				style->drawPrimitive(inVerticalToolBar(option, widget) ? QStyle::PE_IndicatorArrowRight : QStyle::PE_IndicatorArrowDown, &opt, painter, widget);
			}
		//	painter->fillRect(opt.rect.adjusted(3, 3, -3, -3), Qt::red);
		} else {
			QStyleOptionToolButton opt = *option;
			opt.features &= ~QStyleOptionToolButton::HasMenu;
			((QCommonStyle *) style)->QCommonStyle::drawComplexControl(QStyle::CC_ToolButton, &opt, painter, widget);
			QRect r;
			if (option->direction == Qt::LeftToRight) {
				r = QRect(option->rect.right() - 6, option->rect.bottom() - 6, 5, 5);
				if (option->state & QStyle::State_Sunken) {
					r.adjust(1, 1, 1, 1);
				}
			} else {
				r = QRect(2, option->rect.bottom() - 6, 5, 5);
				if (option->state & QStyle::State_Sunken) {
					r.adjust(-1, 1, -1, 1);
				}
			}
			QFont font;
			font.setPixelSize(7);
			opt.fontMetrics = QFontMetrics(font);
			opt.rect = r;
			style->drawPrimitive(inVerticalToolBar(option, widget) ? QStyle::PE_IndicatorArrowRight : QStyle::PE_IndicatorArrowDown, &opt, painter, widget);
		}
	} else {
		((QCommonStyle *) style)->QCommonStyle::drawComplexControl(QStyle::CC_ToolButton, option, painter, widget);
	}
}


void paintToolButtonLabel(QPainter *painter, const QStyleOptionToolButton *option, const QWidget *widget, const QStyle *style)
{
    QStyleOptionToolButton opt = *option;
    if (option->state & QStyle::State_AutoRaise) {
        if (!(option->state & QStyle::State_Enabled) || !(option->state & QStyle::State_MouseOver)) {
            opt.palette.setColor(QPalette::ButtonText, opt.palette.color(QPalette::WindowText));
        }
    }
    ((QCommonStyle *) style)->QCommonStyle::drawControl(QStyle::CE_ToolButtonLabel, &opt, painter, widget);
}


/*-----------------------------------------------------------------------*/

QRect subControlRectToolButton(const QStyleOptionToolButton *option, QStyle::SubControl subControl, const QWidget *widget, const QStyle *style)
{
	QRect r = option->rect;

	if (option->features & QStyleOptionToolButton::Menu) {
		int pm = style->pixelMetric(QStyle::PM_MenuButtonIndicator, option, widget) - 2;
		bool verticalBar = inVerticalToolBar(option, widget);
		switch (subControl) {
			case QStyle::SC_ToolButton:
				if (verticalBar) {
					r.adjust(0, 0, 0, -pm);
				} else {
					r.adjust(0, 0, -pm, 0);
				}
				break;
			case QStyle::SC_ToolButtonMenu:
				if (verticalBar) {
					r.adjust(0, r.height() - pm - 2, 0, 0);
				} else {
					r.adjust(r.width() - pm - 2, 0, 0, 0);
				}
				break;
			default:
				break;
		}
		return style->visualRect(option->direction, option->rect, r);
	}
	return ((QCommonStyle *) style)->QCommonStyle::subControlRect(QStyle::CC_ToolButton, option, subControl, widget);
}


QSize sizeFromContentsToolButton(const QStyleOptionToolButton *option, const QSize &contentsSize, const QWidget *widget, const QStyle *style, int toolButtonSize)
{
	QSize size = contentsSize + QSize(4, 4);

        if (toolButtonSize >= 0) {
            if (option->toolButtonStyle == Qt::ToolButtonTextUnderIcon) {
                size += QSize(toolButtonSize, qMax(2, toolButtonSize));
            } else {
                size += QSize(toolButtonSize, toolButtonSize);
            }
        } else {
            size += QSize(4, 4);
        }
	if (widget && !qstrcmp(widget->metaObject()->className(), "KAnimatedButton")) {
            return contentsSize + QSize(4, 4);
	}
        if (widget && !qstrcmp(widget->metaObject()->className(), "QtColorButton")) {
            return contentsSize + QSize(12, 12);
        }
	if (option->features & QStyleOptionToolButton::Menu) {
		int pm = style->pixelMetric(QStyle::PM_MenuButtonIndicator, option, widget);
		size -= QSize(pm, 0);
		pm -= 2;
		bool verticalBar = inVerticalToolBar(option, widget);

		if (verticalBar) {
			size += QSize(0, pm);
		} else {
			size += QSize(pm, 0);
		}
	}
	return size;
}


/*-----------------------------------------------------------------------*/

void SkulptureStyle::Private::updateToolBarOrientation(Qt::Orientation /*orientation */)
{
	QToolBar *toolbar = static_cast<QToolBar *>(sender());
	QList<QToolButton *> toolbuttons = toolbar->findChildren<QToolButton *>();
	bool changed = false;

    Q_FOREACH (QToolButton *toolbutton, toolbuttons) {
		if (toolbutton->popupMode() == QToolButton::MenuButtonPopup) {
			// ### this hack forces Qt to invalidate the size hint
			Qt::ToolButtonStyle oldstyle = toolbutton->toolButtonStyle();
			Qt::ToolButtonStyle newstyle;
			if (oldstyle == Qt::ToolButtonIconOnly) {
				newstyle = Qt::ToolButtonTextOnly;
			} else {
				newstyle = Qt::ToolButtonIconOnly;
			}
			toolbutton->setToolButtonStyle(newstyle);
			toolbutton->setToolButtonStyle(oldstyle);
			changed = true;
		}
	}
	if (changed) {
		// ### Qt does not update dragged toolbars...
		toolbar->updateGeometry();
	}
}


