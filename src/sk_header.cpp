/*
 * skulpture_header.cpp
 *
 */

#include "skulpture_p.h"
#include "sk_factory.h"
#include <QtGui/QPainter>
#include <QtWidgets/QHeaderView>


/*-----------------------------------------------------------------------*/

void paintHeaderEmptyArea(QPainter *painter, const QStyleOption *option)
{
	if (option->state & QStyle::State_Enabled) {
		painter->fillRect(option->rect, option->palette.color(QPalette::Window).lighter(107));
	} else {
		painter->fillRect(option->rect, option->palette.color(QPalette::Window).darker(104));
	}
	if (option->state & QStyle::State_Horizontal) {
		paintThinFrame(painter, option->rect.adjusted(0, -2, 32000, -1), option->palette, -20, 60);
//		painter->fillRect(option->rect.adjusted(0, option->rect.height() - 1, 0, 0), QColor(255, 255, 255, 160));
	} else {
		paintThinFrame(painter, option->rect.adjusted(-2, 0, -1, 32000), option->palette, -20, 60);
//		painter->fillRect(option->rect.adjusted(option->rect.width() - 1, 0, 0, 0), QColor(255, 255, 255, 160));
	}
}


static bool isHeaderEnabled(const QStyleOptionHeader *option, const QWidget *widget)
{
    bool enabled = true;
    if (!(option->state & QStyle::State_Enabled)) {
        enabled = false;
        if (widget && widget->inherits("Q3Header")) {
            enabled = widget->isEnabled();
        }
    }
    return enabled;
}


void paintHeaderSection(QPainter *painter, const QStyleOptionHeader *option, const QWidget *widget, const QStyle *style)
{
	Q_UNUSED(style);

	if (!(option->state & (QStyle::State_Raised | QStyle::State_Sunken))) {
		painter->fillRect(option->rect, option->palette.color(QPalette::Window).darker(104));
		paintRecessedFrame(painter, option->rect.adjusted(-9, -9, 3, 3), option->palette, RF_Small);
		painter->fillRect(QRect(option->rect.right(), option->rect.bottom(), 1, 1), option->palette.color(QPalette::Window));
	} else {
                if (isHeaderEnabled(option, widget)) {
			bool hover = false;
			const QHeaderView *view = qobject_cast<const QHeaderView *>(widget);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
                        if (view && (view->sectionsClickable() || view->sectionsMovable())) {
#else
			if (view && (view->isClickable() || view->isMovable())) {
#endif
				hover = option->state & QStyle::State_MouseOver;
			}
			painter->fillRect(option->rect, option->palette.color(QPalette::Base).darker(hover ? 104 : (option->state & QStyle::State_On ? 120 : 106)));
		} else {
			painter->fillRect(option->rect, option->palette.color(QPalette::Window).darker(104));
		}
		if (true || !(option->state & QStyle::State_On)) {
			if (option->orientation == Qt::Horizontal) {
				const QHeaderView *view = qobject_cast<const QHeaderView *>(widget);
				if (view && view->rect().right() == option->rect.right()) {
					paintThinFrame(painter, option->rect.adjusted(0, -2, 1, -1), option->palette, -20, 60);
				} else {
					paintThinFrame(painter, option->rect.adjusted(0, -2, 0, -1), option->palette, -20, 60);
				}
			} else {
				if (option->direction == Qt::LeftToRight) {
					paintThinFrame(painter, option->rect.adjusted(-2, 0, -1, 0), option->palette, -20, 60);
				} else {
					paintThinFrame(painter, option->rect.adjusted(1, 0, 2, 0), option->palette, -20, 60);
				}
			}
		}
#if 0
		if (option->orientation == Qt::Horizontal) {
			painter->fillRect(option->rect.adjusted(0, option->rect.height() - 1, 0, 0), QColor(255, 255, 255, 160));
		} else {
			painter->fillRect(option->rect.adjusted(option->rect.width() - 1, 0, 0, 0), QColor(255, 255, 255, 160));
		}
#endif
	}
}


void paintHeaderLabel(QPainter *painter, const QStyleOptionHeader *option, const QWidget *widget, const QStyle *style)
{
    QStyleOptionHeader opt = *option;
    if (isHeaderEnabled(option, widget)) {
        opt.palette.setColor(QPalette::ButtonText, opt.palette.color(QPalette::Text));
    } else {
        opt.palette.setColor(QPalette::ButtonText, opt.palette.color(QPalette::WindowText));
    }
	painter->save();
	if (widget) {
		painter->setFont(widget->font());
	}
	((QCommonStyle *) style)->QCommonStyle::drawControl(QStyle::CE_HeaderLabel, &opt, painter, widget);
	painter->restore();
}


static const ShapeFactory::Code headerSortIndicatorShapeDescription[] = {
	Pmove(-1, 1), Pline(1, 1), Pline(0, -1), Pend
};


void paintHeaderSortIndicator(QPainter *painter, const QStyleOptionHeader *option)
{
	int h = option->fontMetrics.height() / 2 + 2;
	int w = option->fontMetrics.height() / 4 + 2;
	QPainterPath path;

	h /= 2; w /= 2;
	if (option->sortIndicator == QStyleOptionHeader::SortDown) {
		h = -h;
	}
	painter->save();
	painter->setRenderHint(QPainter::Antialiasing, true);
	painter->translate(option->rect.center());
	painter->translate(0.5, 1.5);
	painter->setPen(Qt::NoPen);
	QColor color = option->palette.color(option->state & QStyle::State_Enabled ? QPalette::Text : QPalette::WindowText);
	color.setAlphaF(0.6 * color.alphaF());
	painter->setBrush(color);
    QTransform transform(w, 0, 0, h, 0, 0);
    painter->drawPath(transform.map(ShapeFactory::createShape(headerSortIndicatorShapeDescription)));
	painter->restore();
}


