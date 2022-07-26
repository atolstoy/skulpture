/*
 * skulpture_dock.cpp
 *
 */

#include "skulpture_p.h"
#include <QtGui/QPainter>
#include <QtWidgets/QDockWidget>


/*-----------------------------------------------------------------------*/

void paintFrameDockWidget(QPainter *painter, const QStyleOptionFrame *option)
{
	paintThinFrame(painter, option->rect, option->palette, -60, 160);
	paintThinFrame(painter, option->rect.adjusted(1, 1, -1, -1), option->palette, -20, 60);
}


void paintDockWidgetTitle(QPainter *painter, const QStyleOptionDockWidget *option, const QWidget *widget, const QStyle *style)
{
	const QDockWidget *dock = qobject_cast<const QDockWidget *>(widget);
    const bool vertical = dock && (dock->features() & QDockWidget::DockWidgetVerticalTitleBar);
    const bool floating = dock && dock->isFloating();
	QRect r = option->rect;
	if (floating) {
		if (vertical) {
			r.adjust(-3, 3, 0, -3);
		} else {
			r.adjust(3, -3, -3, 0);
		}
	//	painter->fillRect(r.adjusted(1, 1, -1, -1), QColor(30, 40, 80));
	}
	QColor color = option->palette.color(QPalette::Window);
	paintThinFrame(painter, r, option->palette, 40, -20);
	paintThinFrame(painter, r.adjusted(1, 1, -1, -1), option->palette, -20, 80);
	QLinearGradient gradient(r.topLeft(), vertical ? r.topRight() : r.bottomLeft());
	gradient.setColorAt(0.0, shaded_color(color, 50));
	gradient.setColorAt(0.2, shaded_color(color, 30));
	gradient.setColorAt(0.5, shaded_color(color, 0));
	gradient.setColorAt(0.51, shaded_color(color, -10));
	gradient.setColorAt(1.0, shaded_color(color, -20));
	painter->fillRect(r.adjusted(1, 1, -1, -1), gradient);
#if 0
	QRadialGradient dialogGradient2(r.left() + r.width() / 2, r.top(), r.height());
	dialogGradient2.setColorAt(0.0, QColor(255, 255, 255, 50));
	dialogGradient2.setColorAt(1.0, QColor(0, 0, 0, 0));
	painter->save();
	painter->translate(r.center());
	painter->scale(r.width() / 2.0 / r.height(), 1);
	painter->translate(-r.center());
	painter->fillRect(r.adjusted(1, 1, -1, -1), dialogGradient2);
	painter->restore();
#endif
	QFont font = painter->font();
	font.setBold(true);
	font.setPointSizeF(font.pointSizeF() / 1.19);
	painter->save();
	painter->setFont(font);
	r = style->subElementRect(QStyle::SE_DockWidgetTitleBarText, option, widget);
	// ### fix for Plastique centering
	if (vertical && option->rect.height() & 1) {
		if (!floating) {
			r.adjust(0, 1, 0, 1);
		} else {
			r.adjust(0, -1, 0, -1);
		}
	}
	if (floating) {
		if (vertical) {
			r.adjust(-1, 12, 3, -10);
		} else {
			r.adjust(2, 3, -3, -7);
		}
	} else {
		if (vertical) {
			r.adjust(0, 8, 4, -8);
		} else {
			r.adjust(0, 5, 0, -7);
		}
	}
	if (vertical) {
        QTransform mat;
		QPointF c = r.center();
		mat.translate(c.x(), c.y());
		mat.rotate(-90);
		mat.translate(-c.x(), -c.y());
		r = mat.mapRect(r);
        painter->setTransform(mat, true);
	}
//	painter->fillRect(r, Qt::red);
	painter->setClipRect(r);
	style->drawItemText(painter, r, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextShowMnemonic | Qt::TextHideMnemonic, option->palette, true, option->title, QPalette::WindowText);
	painter->restore();
}


QRect subElementRectDockWidget(QStyle::SubElement element, const QStyleOptionDockWidget *option, const QWidget *widget, const QStyle *style)
{
    switch (element) {
        case QStyle::SE_DockWidgetCloseButton:
        case QStyle::SE_DockWidgetFloatButton: {
            const QDockWidget *dock = qobject_cast<const QDockWidget *>(widget);
            bool floating = option->floatable && dock && dock->isFloating();
            bool vertical = dock && (dock->features() & QDockWidget::DockWidgetVerticalTitleBar);
            QRect r = ((QCommonStyle *) style)->QCommonStyle::subElementRect(element, option, widget);
            if (!vertical) {
                if (floating) {
                    if (option->direction == Qt::LeftToRight) {
                        return r.adjusted(-6, 0, -6, 0);
                    } else {
                        return r.adjusted(6, 0, 6, 0);
                    }
                } else {
                    if (option->direction == Qt::LeftToRight) {
                        return r.adjusted(-3, 1, -3, 1);
                    } else {
                        return r.adjusted(3, 1, 3, 1);
                    }
                }
            } else {
                if (floating) {
                    return r.adjusted(0, 6, 0, 6);
                } else {
                    return r.adjusted(1, 3, 1, 3);
                }
            }
        }
    case QStyle::SE_DockWidgetTitleBarText:
        return ((QCommonStyle *) style)->QCommonStyle::subElementRect(element, option, widget).adjusted(4, -3, -4, 5);
    case QStyle::SE_DockWidgetIcon:
        return ((QCommonStyle *) style)->QCommonStyle::subElementRect(element, option, widget).adjusted(4, -3, 4, 5);
    default: // avoid warning
        return option->rect;
    }
}


