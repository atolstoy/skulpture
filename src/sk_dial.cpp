/*
 * skulpture_dial.cpp
 *
 */

#include "skulpture_p.h"
#include <QtGui/QPainter>
#include <QtWidgets/QAbstractSlider>
#include <cmath>


/*-----------------------------------------------------------------------*/

extern void paintIndicatorDial(QPainter *painter, const QStyleOptionSlider *option);
extern void paintCachedGrip(QPainter *painter, const QStyleOption *option, QPalette::ColorRole bgrole);

void paintDial(QPainter *painter, const QStyleOptionSlider *option, const QWidget *widget, const QStyle *style)
{
	int d = qMin(option->rect.width() & ~1, option->rect.height() & ~1);
	QStyleOptionSlider opt = *option;
	const QAbstractSlider *slider;
	// always highlight knob if pressed (even if mouse is not over knob)
	if ((option->state & QStyle::State_HasFocus) && (slider = qobject_cast<const QAbstractSlider *>(widget))) {
		if (slider->isSliderDown()) {
			opt.state |= QStyle::State_MouseOver;
		}
	}

	// tickmarks
	opt.palette.setColor(QPalette::Inactive, QPalette::WindowText, QColor(120, 120, 120, 255));
	opt.palette.setColor(QPalette::Active, QPalette::WindowText, QColor(120, 120, 120, 255));
	opt.state &= ~QStyle::State_HasFocus;
        opt.rect.setWidth(opt.rect.width() & ~1);
        opt.rect.setHeight(opt.rect.height() & ~1);
        ((QCommonStyle *) style)->QCommonStyle::drawComplexControl(QStyle::CC_Dial, &opt, painter, widget);
        opt.rect.setWidth((opt.rect.width() & ~1) - 1);
        opt.rect.setHeight((opt.rect.height() & ~1) - 1);
        opt.rect.translate(1, 1);

	// focus rectangle
	if (option->state & QStyle::State_HasFocus) {
		QStyleOptionFocusRect focus;
		opt.state |= QStyle::State_HasFocus;
		focus.QStyleOption::operator=(opt);
		focus.rect.adjust(-1, -1, 1, 1);
		style->drawPrimitive(QStyle::PE_FrameFocusRect, &focus, painter, widget);
	}
	opt.palette = option->palette;

	// dial base
	if (d <= 256) {
		paintIndicatorDial(painter, &opt);
	} else {
		// large dials are slow to render, do not render them
	}

	// dial knob
	d -= 6;
	int gripSize = (option->fontMetrics.height() / 4) * 2 - 1;
	opt.rect.setSize(QSize(gripSize, gripSize));
	opt.rect.moveCenter(option->rect.center());
	// angle calculation from qcommonstyle.cpp (c) Trolltech 1992-2007, ASA.
	qreal angle;
	int sliderPosition = option->upsideDown ? option->sliderPosition : (option->maximum - option->sliderPosition);
	int range = option->maximum - option->minimum;
	if (!range) {
		angle = M_PI / 2;
	} else if (option->dialWrapping) {
		angle = M_PI * 1.5 - (sliderPosition - option->minimum) * 2 * M_PI / range;
	} else {
		angle = (M_PI * 8 - (sliderPosition - option->minimum) * 10 * M_PI / range) / 6;
	}

	qreal rr = d / 2.0 - gripSize - 2;
	opt.rect.translate(int(0.5 + rr * cos(angle)), int(0.5 - rr * sin(angle)));
	paintCachedGrip(painter, &opt, option->state & QStyle::State_Enabled ? QPalette::Button : QPalette::Window);
}


