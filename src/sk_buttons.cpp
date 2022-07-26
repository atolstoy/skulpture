/*
 * skulpture_buttons.cpp
 *
 */

#include "skulpture_p.h"
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>


static QPainterPath button_path(const QRectF &rect, qreal k)
{
	k *= 0.1;
	const qreal tlh_edge = 6.0 * k;
	const qreal tlv_edge = 6.0 * k;
	const qreal tlh_control = 4.0 * k;
	const qreal tlv_control = 4.0 * k;
	const qreal blh_edge = 6.0 * k;
	const qreal blv_edge = 6.0 * k;
	const qreal blh_control = 4.0 * k;
	const qreal blv_control = 4.0 * k;
	const qreal trh_edge = 6.0 * k;
	const qreal trv_edge = 6.0 * k;
	const qreal trh_control = 4.0 * k;
	const qreal trv_control = 4.0 * k;
	const qreal brh_edge = 6.0 * k;
	const qreal brv_edge = 6.0 * k;
	const qreal brh_control = 4.0 * k;
	const qreal brv_control = 4.0 * k;
	QPainterPath path;
	path.moveTo(rect.left() + tlh_edge, rect.top());
	path.lineTo(rect.right() - trh_edge, rect.top());
	path.cubicTo(rect.right() - trh_edge + trh_control, rect.top(), rect.right(), rect.top() + trv_edge - trv_control, rect.right(), rect.top() + trv_edge);
	path.lineTo(rect.right(), rect.bottom() - brv_edge);
	path.cubicTo(rect.right(), rect.bottom() - brv_edge + brv_control, rect.right() - brh_edge + brh_control, rect.bottom(), rect.right() - brh_edge, rect.bottom());
	path.lineTo(rect.left() + blh_edge, rect.bottom());
	path.cubicTo(rect.left() + blh_edge - blh_control, rect.bottom(), rect.left(), rect.bottom() - blv_edge + blv_control, rect.left(), rect.bottom() - blv_edge);
	path.lineTo(rect.left(), rect.top() + tlv_edge);
	path.cubicTo(rect.left(), rect.top() + tlv_edge - tlv_control, rect.left() + tlh_edge - tlh_control, rect.top(), rect.left() + tlh_edge, rect.top());
	return path;
}


static QBrush button_gradient(const QRectF &rect, const QColor &color, const QStyleOptionButton *option)
{
    Q_UNUSED(option);

    qreal ch = color.hueF();
    qreal cs = color.saturationF() * 1.0;
    qreal cv = color.valueF() * 1.0;
    int ca = color.alpha();
    QColor col;

    if (rect.height() > 64) {
        return QColor(color);
    }
    QLinearGradient gradient(rect.topLeft(), rect.bottomLeft());
    col.setHsvF(ch, cs, qMax(0.0, cv - 0.02));
    col.setAlpha(ca);
    gradient.setColorAt(0.0, col);
    col.setHsvF(ch, cs, qMin(1.0, cv + 0.03));
    col.setAlpha(ca);
    gradient.setColorAt(1.0, col);
    return gradient;
}


void paintButtonPanel(QPainter *painter, const QStyleOptionButton *option, QPalette::ColorRole bgrole)
{
	const QRectF &c_rect = option->rect;
	const qreal t = 1.0;
	QRectF rect = c_rect;
	bool frame = true;
	if (option->features & QStyleOptionButton::Flat && !(option->state & QStyle::State_Sunken)) {
		frame = false;
	}
	painter->setPen(Qt::NoPen);
	if ((option->features & QStyleOptionButton::DefaultButton) && (option->state & QStyle::State_Enabled)) {
		painter->setBrush(path_edge_gradient(rect, option, button_path(rect, 1.3), blend_color(QColor(0, 0, 0, 10), option->palette.color(QPalette::Highlight).lighter(110), 0.2), blend_color(QColor(0, 0, 0, 15), option->palette.color(QPalette::Highlight).lighter(110), 0.2)));
	} else {
		painter->setBrush(path_edge_gradient(rect, option, button_path(rect, 1.3), shaded_color(option->palette.color(QPalette::Window), -10), shaded_color(option->palette.color(QPalette::Window), -15)));
	}
	painter->drawPath(button_path(rect, 1.5));
	rect.adjust(t, t, -t, -t);
	QBrush bgbrush = option->palette.brush(option->state & QStyle::State_Enabled ? (bgrole == QPalette::NoRole ? QPalette::Button : bgrole) : QPalette::Button);
	if (bgbrush.style() == Qt::SolidPattern && bgbrush.color().alpha() == 0) {
		QColor color = option->palette.color(QPalette::Window);
		color.setAlpha(0);
		bgbrush = color;
	}
	if (frame) {
		if (option->state & QStyle::State_Enabled) {
                        if (option->state & QStyle::State_Sunken || option->state & QStyle::State_On) {
				painter->setBrush(path_edge_gradient(rect, option, button_path(rect, 1.1), shaded_color(option->palette.color(QPalette::Window), -35), shaded_color(option->palette.color(QPalette::Window), -75)));
			} else {
				painter->setBrush(path_edge_gradient(rect, option, button_path(rect, 1.1), shaded_color(option->palette.color(QPalette::Window), -75), shaded_color(option->palette.color(QPalette::Window), -45)));
			}
		} else {
			painter->setBrush(path_edge_gradient(rect, option, button_path(rect, 1.1), shaded_color(option->palette.color(QPalette::Window), -35), shaded_color(option->palette.color(QPalette::Window), -35)));
		}
		painter->drawPath(button_path(rect, 1.3));
		rect.adjust(t, t, -t, -t);

		if (bgbrush.style() == Qt::SolidPattern) {
			QColor bgcolor = bgbrush.color();
			if (option->state & QStyle::State_On) {
				bgcolor = blend_color(bgcolor, option->palette.color(QPalette::Highlight), 0.2);
				bgbrush = button_gradient(rect, bgcolor, option);
			}
			if (option->state & QStyle::State_Enabled) {
				if (option->state & QStyle::State_Sunken) {
					bgcolor = bgcolor.lighter(102);
				} else if (option->state & QStyle::State_MouseOver) {
					bgcolor = bgcolor.lighter(104);
				}
				bgbrush = button_gradient(rect, bgcolor, option);
			}
			painter->setBrush(bgbrush);
		//	painter->setBrush(option->palette.color(QPalette::Button));
			painter->drawPath(button_path(rect, 1.1));
			if (option->state  & QStyle::State_Enabled) {
				if (option->state & QStyle::State_Sunken || option->state & QStyle::State_On) {
					painter->setBrush(path_edge_gradient(rect, option, button_path(rect, 0.9), shaded_color(bgcolor, -10), shaded_color(bgcolor, -20)));
				} else {
					painter->setBrush(path_edge_gradient(rect, option, button_path(rect, 0.9), shaded_color(bgcolor, -20), shaded_color(bgcolor, 160)));
				}
				painter->drawPath(button_path(rect, 1.1));
			}
		}
		painter->setBrush(bgbrush);
	} else {
		QColor bgcolor = option->palette.color(QPalette::Window);
		if (option->state & QStyle::State_MouseOver) {
			bgcolor = bgcolor.lighter(104);
		}
		if (option->state & QStyle::State_On) {
			bgcolor = blend_color(bgcolor, option->palette.color(QPalette::Highlight), 0.2);
		}
		painter->setBrush(bgcolor);
	}
	rect.adjust(t, t, -t, -t);
	painter->save();
	// make transparent buttons appear transparent
	painter->setCompositionMode(QPainter::CompositionMode_DestinationOut);
        painter->setBrush(Qt::black);
	painter->drawPath(button_path(rect, 0.9));
	painter->restore();
	painter->drawPath(button_path(rect, 0.9));
}


void paintPushButtonBevel(QPainter *painter, const QStyleOptionButton *option, const QWidget *widget, const QStyle *style)
{
	QStyleOptionButton opt = *option;

	opt.features &= ~(QStyleOptionButton::HasMenu);
	((QCommonStyle *) style)->QCommonStyle::drawControl(QStyle::CE_PushButtonBevel, &opt, painter, widget);
	if (option->features & QStyleOptionButton::Flat) {
		if (!(option->state & (QStyle::State_Sunken | QStyle::State_On))) {
			if (option->state & QStyle::State_MouseOver) {
				painter->fillRect(option->rect.adjusted(2, 2, -2, -2), QColor(255, 255, 255, 60));
			}
		}
	}
	if (option->features & QStyleOptionButton::HasMenu) {
		int size = style->pixelMetric(QStyle::PM_MenuButtonIndicator, &opt, widget);
		opt.palette.setColor(QPalette::WindowText, opt.palette.color(widget ? widget->foregroundRole() : QPalette::ButtonText));
		opt.state &= ~(QStyle::State_MouseOver);
		if (option->direction == Qt::LeftToRight) {
			opt.rect = QRect(option->rect.right() - size - 2, option->rect.top(), size, option->rect.height());
		} else {
			opt.rect = QRect(option->rect.left() + 4, option->rect.top(), size, option->rect.height());
		}
		if (option->state & (QStyle::State_Sunken | QStyle::State_On)) {
			opt.rect.translate(style->pixelMetric(QStyle::PM_ButtonShiftHorizontal, &opt, widget), style->pixelMetric(QStyle::PM_ButtonShiftVertical, &opt, widget));
		}
		style->drawPrimitive(QStyle::PE_IndicatorArrowDown, &opt, painter, widget);
	}
}