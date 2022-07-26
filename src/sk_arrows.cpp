/*
 * skulpture_arrows.cpp
 *
 */

#include "skulpture_p.h"
#include "sk_factory.h"
#include <QtGui/QPainter>


/*-----------------------------------------------------------------------*/

#define awf 0.8 /* width of inner arrow 0 ... 1 */
#define ahf 0.2 /* position of inner arrow, -1 ... 1 */
#define spf 0.2 /* position of spin plus/minus edges */
#define swf 0.8 /* width of spin plus/minus sign */
#define shf 1.0 /* height of spin plus/minus sign */

static const ShapeFactory::Code arrowShapeDescription[] = {
    Pmove(-1, 1), Pline(-awf, 1), Pline(0, ahf), Pline(awf, 1), Pline(1, 1), Pline(0, -1), Pend
};

static const ShapeFactory::Code spinPlusDescription[] = {
    Pmove(-swf, spf), Pline(-spf, spf), Pline(-spf, shf), Pline(spf, shf), Pline(spf, spf), Pline(swf, spf),
           Pline(swf, -spf), Pline(spf, -spf), Pline(spf, -shf), Pline(-spf, -shf), Pline(-spf, -spf), Pline(-swf, -spf), Pend
};

static const ShapeFactory::Code spinMinusDescription[] = {
    Pmove(-swf, spf), Pline(swf, spf), Pline(swf, -spf), Pline(-swf, -spf), Pend
};

static const ShapeFactory::Code sortIndicatorShapeDescription[] = {
    Pmove(-1, 1), Pline(1, 1), Pline(0, -1), Pend
};


/*-----------------------------------------------------------------------*/

static inline QPainterPath arrowPath(const QStyleOption *option, Qt::ArrowType arrow, bool spin)
{
	qreal var[ShapeFactory::MaxVar + 1];
	var[1] = 0.01 * arrow;
	var[2] = spin ? 1.0 : 0.0;
	var[3] = option->fontMetrics.height();
	var[4] = 0.0;
	QPainterPath shape = ShapeFactory::createShape(
         spin && arrow == Qt::LeftArrow ? spinMinusDescription :
         spin && arrow == Qt::RightArrow ? spinPlusDescription :
        arrowShapeDescription, var);
	if (var[4] != 0.0) {
		shape.setFillRule(Qt::WindingFill);
	}

	qreal h = 2.0 + var[3] * (spin ? 2.0 : 3.0) / 9.0;
	qreal w = 2.0 + var[3] / 3.0;
	h /= 2; w /= 2;
	if (arrow == Qt::DownArrow || arrow == Qt::RightArrow) {
		h = -h;
	}
	bool horiz = !spin && (arrow == Qt::LeftArrow || arrow == Qt::RightArrow);
	QTransform arrowTransform(horiz ? 0 : w, horiz ? w : 0, horiz ? h : 0 , horiz ? 0 : h, 0, 0);
	return arrowTransform.map(shape);
}


void paintScrollArrow(QPainter *painter, const QStyleOption *option, Qt::ArrowType arrow, bool spin)
{
	painter->save();
	// FIXME: combine translations with path matrix
	painter->translate(option->rect.center());
#if 1
	painter->setRenderHint(QPainter::Antialiasing, true);
#else
	painter->setRenderHint(QPainter::Antialiasing, false);
#endif
	if (painter->renderHints() & QPainter::Antialiasing) {
		painter->translate(0.5, 0.5);
	}
#if 1
	switch (arrow) {
		case Qt::UpArrow:
			painter->translate(0, -0.5);
			break;
		case Qt::DownArrow:
			painter->translate(0, 0.5);
			break;
		case Qt::LeftArrow:
                    if (!spin) {
                        painter->translate(-0.5, 0);
                    }
			break;
		case Qt::RightArrow:
                    if (!spin) {
                        painter->translate(0.5, 0);
                    }
			break;
		case Qt::NoArrow:
			break;
	}
#endif
	painter->setPen(Qt::NoPen);
        QColor color = option->palette.color(spin ? (option->state & QStyle::State_Enabled ? QPalette::WindowText : QPalette::Text) : QPalette::ButtonText);
	if ((option->state & QStyle::State_MouseOver) && option->state & QStyle::State_Enabled /* && !(option->state & QStyle::State_Sunken)*/) {
		color = option->palette.color(QPalette::Highlight).darker(200);
	//	painter->setPen(QPen(Qt::white, 1.0));
	} else {
	//	painter->setPen(QPen(Qt::white, 0.5));
	}
	color.setAlpha((179 * color.alpha()) >> 8);
	painter->setBrush(color);
	painter->drawPath(arrowPath(option, arrow, spin));
	painter->restore();
}


void paintIndicatorArrowDown(QPainter *painter, const QStyleOption *option)
{
	paintScrollArrow(painter, option, Qt::DownArrow, false);
}


void paintIndicatorArrowLeft(QPainter *painter, const QStyleOption *option)
{
	paintScrollArrow(painter, option, Qt::LeftArrow, false);
}


void paintIndicatorArrowRight(QPainter *painter, const QStyleOption *option)
{
	paintScrollArrow(painter, option, Qt::RightArrow, false);
}


void paintIndicatorArrowUp(QPainter *painter, const QStyleOption *option)
{
	paintScrollArrow(painter, option, Qt::UpArrow, false);
}


