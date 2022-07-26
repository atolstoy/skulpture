/*
 * skulpture_slider.cpp
 *
 */

#include "skulpture_p.h"
#include <QtWidgets/QAbstractSlider>
#include <QtGui/QPainter>


/*-----------------------------------------------------------------------*/

void paintSliderGroove(QPainter *painter, QRect &rect, const QStyleOptionSlider *option)
{
	if (option->orientation == Qt::Horizontal) {
		int d = rect.height() / 2;
		rect.adjust(0, d, 0, -d);
	} else {
		int d = rect.width() / 2;
		rect.adjust(d, 0, -d, 0);
	}
	QColor color = option->palette.color(QPalette::Window);
	if (option->state & QStyle::State_Enabled) {
            color = color.darker(120);
		painter->fillRect(rect, color);
		paintThinFrame(painter, rect.adjusted(-1, -1, 1, 1), option->palette, -30, -90);
	} else {
		painter->fillRect(rect, color);
		paintThinFrame(painter, rect.adjusted(-1, -1, 1, 1), option->palette, -20, -60);
	}
}


void paintSliderHandle(QPainter *painter, const QRect &rect, const QStyleOptionSlider *option)
{
	// shadow
	painter->fillRect(rect.adjusted(2, 2, 2, 2), QColor(0, 0, 0, 5));
	painter->fillRect(rect.adjusted(1, 1, 1, 1), QColor(0, 0, 0, 8));
	// slider color
	QColor color = option->palette.color(QPalette::Button);
	if (option->state & QStyle::State_Enabled) {
		if (option->state & QStyle::State_Sunken) {
                    color = color.lighter(102);
                } else if (option->state & QStyle::State_MouseOver) {
                    color = color.lighter(104);
		}
	} else {
		color = option->palette.color(QPalette::Window);
	}
	painter->fillRect(rect, color);

#if 1 // slider gradient
	if ((option->state & QStyle::State_Enabled) && !(option->state & QStyle::State_Sunken)) {
            QLinearGradient gradient(rect.topLeft(), option->orientation == Qt::Horizontal ? rect.bottomLeft() : rect.topRight());
#if 1
		// SkandaleStyle 0.0.2
		gradient.setColorAt(0.0, shaded_color(color, 40));
		gradient.setColorAt(0.5, shaded_color(color, 0));
		gradient.setColorAt(1.0, shaded_color(color, 70));
#else
		// glassy
		gradient.setColorAt(0.0, shaded_color(color, 40));
		gradient.setColorAt(0.4, shaded_color(color, -5));
		gradient.setColorAt(0.405, shaded_color(color, -15));
		gradient.setColorAt(1.0, shaded_color(color, 70));
#endif
		painter->fillRect(rect, gradient);
	}
#endif
	// slider frame
	paintThinFrame(painter, rect, option->palette, -70, -20, QPalette::Button);
	paintThinFrame(painter, rect.adjusted(1, 1, -1, -1), option->palette, -30, 130, QPalette::Button);
}


/*-----------------------------------------------------------------------*/

void paintSlider(QPainter *painter, const QStyleOptionSlider *option, const QWidget *widget, const QStyle *style)
{
	// groove
	if (option->subControls & QStyle::SC_SliderGroove) {
	//	painter->fillRect(option->rect, option->palette.color(QPalette::Window).darker(105));
	//	paintThinFrame(painter, option->rect, option->palette, 130, -30);
#if 0
		int handlesize = style->pixelMetric(QStyle::PM_SliderLength, option, widget);
		int e = handlesize / 2 - 1;
#else
		int e = 1;
#endif
#if 0
		QRect rect = option->rect;
	//	QRect rect = style->subControlRect(QStyle::CC_Slider, option, QStyle::SC_SliderGroove, widget);
		if (option->orientation == Qt::Horizontal) {
			rect.adjust(e, 0, -e, 0);
		} else {
			rect.adjust(0, e, 0, -e);
		}
		paintSliderGroove(painter, rect, option);
#else
		QRect rect = style->subControlRect(QStyle::CC_Slider, option, QStyle::SC_SliderGroove, widget);
		//QRect rect = option->rect;
		QRect handle_rect = style->subControlRect(QStyle::CC_Slider, option, QStyle::SC_SliderHandle, widget);
		QStyleOptionSlider aOption = *option;
		aOption.palette.setColor(QPalette::Window, aOption.palette.color(QPalette::Highlight));
		if (option->orientation == Qt::Horizontal) {
			handle_rect.adjust(0, 2, 0, -2);
			rect.adjust(e, 0, -e, 0);
			rect.setWidth(handle_rect.left() - rect.left() - 1);
			if (rect.width() > -3) {
				paintSliderGroove(painter, rect, option->upsideDown ? option : &aOption);
			}
			rect.setLeft(handle_rect.right() + 2);
			rect.setWidth(option->rect.right() - handle_rect.right() - 1 - e);
			if (rect.width() > -3) {
				paintSliderGroove(painter, rect, option->upsideDown ? &aOption : option);
			}
		} else {
			handle_rect.adjust(2, 0, -2, 0);
			rect.adjust(0, e, 0, -e);
			rect.setHeight(handle_rect.top() - rect.top() - 1);
			if (rect.height() > -3) {
				paintSliderGroove(painter, rect, option->upsideDown ? option : &aOption);
			}
			rect.setTop(handle_rect.bottom() + 2);
			rect.setHeight(option->rect.bottom() - handle_rect.bottom() - e);
			if (rect.height() > -3) {
				paintSliderGroove(painter, rect, option->upsideDown ? &aOption : option);
			}
		}
#endif
	}

#if 1	// tickmarks
	if (option->subControls & QStyle::SC_SliderTickmarks) {
		QStyleOptionSlider slider = *option;
		slider.subControls = QStyle::SC_SliderTickmarks;
		// ### for now, just use common tickmarks
                QPalette palette = slider.palette;
                QColor col = palette.color(QPalette::WindowText);
                col.setAlpha(51);
                palette.setColor(QPalette::WindowText, col);
                slider.palette = palette;
                if (option->orientation == Qt::Horizontal) {
			slider.rect.adjust(-1, 0, -1, 0);
		} else {
			slider.rect.adjust(0, -1, 0, -1);
		}
		((QCommonStyle *) style)->QCommonStyle::drawComplexControl(QStyle::CC_Slider, &slider, painter, widget);
		slider.rect = option->rect;
                palette.setColor(QPalette::WindowText, QColor(255, 255, 255, 77));
		slider.palette = palette;
		((QCommonStyle *) style)->QCommonStyle::drawComplexControl(QStyle::CC_Slider, &slider, painter, widget);
	}
#endif
	// focus rect
	if (option->state & QStyle::State_HasFocus) {
		QStyleOptionFocusRect focus;
		focus.QStyleOption::operator=(*option);
		focus.rect = style->subElementRect(QStyle::SE_SliderFocusRect, option, widget);
		focus.state |= QStyle::State_FocusAtBorder;
		style->drawPrimitive(QStyle::PE_FrameFocusRect, &focus, painter, widget);
	}

	// handle
	if (option->subControls & QStyle::SC_SliderHandle) {
		QStyleOptionSlider aOption = *option;
		if (!(option->activeSubControls & QStyle::SC_SliderHandle)) {
			aOption.state &= ~QStyle::State_MouseOver;
			aOption.state &= ~QStyle::State_Sunken;
		}
		QRect rect = style->subControlRect(QStyle::CC_Slider, option, QStyle::SC_SliderHandle, widget);
		if (option->orientation == Qt::Horizontal) {
			rect.adjust(0, 2, 0, -2);
		} else {
			rect.adjust(2, 0, -2, 0);
		}
#if 0
		if (option->orientation == Qt::Horizontal) {
			rect.setTop(option->rect.top());
			rect.setHeight(option->rect.height());
		//	rect.adjust(0, 1, 0, -1);
		} else {
			rect.setLeft(option->rect.left());
			rect.setWidth(option->rect.width());
		//	rect.adjust(1, 0, -1, 0);
		}
#endif
	//	rect.adjust(0, 0, -1, -1);
		paintSliderHandle(painter, rect, &aOption);
	//	rect.adjust(0, 0, 1, 1);
	//	paintThinFrame(painter, rect.adjusted(1, 1, 1, 1), option->palette, -20, 0);
#if 0
		// grip
		const int o = 5;
		const int s = 6;
		if (option->orientation == Qt::Horizontal) {
			int d = (rect.width() - 2) / 2;
			rect.adjust(d, 0, -d, 0);
			rect.translate(-s, 0);
		} else {
			int d = (rect.height() - 2) / 2;
			rect.adjust(0, d, 0, -d);
			rect.translate(0, -s);
		}
		for (int k = -1; k < 2; ++k) {
			if (option->orientation == Qt::Horizontal) {
				painter->fillRect(rect.adjusted(0, o, 0, -o), QColor(0, 0, 0, 30));
				painter->fillRect(rect.adjusted(1, o, 1, -o), QColor(255, 255, 255, 80));
				rect.translate(s, 0);
			} else {
				painter->fillRect(rect.adjusted(o, 0, -o, 0), QColor(0, 0, 0, 30));
				painter->fillRect(rect.adjusted(o, 1, -o, 1), QColor(255, 255, 255, 80));
				rect.translate(0, s);
			}
		}
#endif
	}
#if 0
	painter->fillRect(style->subControlRect(QStyle::CC_Slider, option, QStyle::SC_SliderGroove, widget), QColor(0, 0, 0, 70));
	if (option->subControls & QStyle::SC_SliderGroove) {
		painter->fillRect(style->subControlRect(QStyle::CC_Slider, option, QStyle::SC_SliderGroove, widget), QColor(255, 0, 0, 70));
	}
	if (option->subControls & QStyle::SC_SliderHandle) {
		painter->fillRect(style->subControlRect(QStyle::CC_Slider, option, QStyle::SC_SliderHandle, widget), QColor(0, 100, 255, 70));
	}
	if (option->subControls & QStyle::SC_SliderTickmarks) {
		painter->fillRect(style->subControlRect(QStyle::CC_Slider, option, QStyle::SC_SliderTickmarks, widget), QColor(0, 255, 0, 170));
	}
#endif
}


/*-----------------------------------------------------------------------*/

QRect subControlRectSlider(const QStyleOptionSlider *option, QStyle::SubControl subControl, const QWidget *widget, const QStyle *style)
{
	QRect rect = ((QCommonStyle *) style)->QCommonStyle::subControlRect(QStyle::CC_Slider, option, subControl, widget);
	switch (subControl) {
		case QStyle::SC_SliderGroove:
		case QStyle::SC_SliderTickmarks:
		case QStyle::SC_SliderHandle:
		default:
			break;
	}
	return rect;
#if 0
	/*
	option->orientation
	option->tickPosition: QSlider::TicksAbove | QSlider::TicksBelow
	option->state
	option->tickInterval
	*/
	switch (subControl) {
		case SC_SliderGrove:
		case SC_SliderTickmarks:
		case SC_SliderHandle:
	}
#endif
}


