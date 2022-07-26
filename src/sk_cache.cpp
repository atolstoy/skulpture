/*
 * skulpture_cache.cpp
 *
 */

#include "skulpture_p.h"
#include <QtGui/QPixmapCache>
#include <QtGui/QGradient>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QDebug>
#include <QFile>
#include "sk_factory.h"
#include <cmath>


/*-----------------------------------------------------------------------*/

static const bool UsePixmapCache = true;


/*-----------------------------------------------------------------------*/
/*
 * paint a pushbutton to painter
 *
 */

extern void paintButtonPanel(QPainter *painter, const QStyleOptionButton *option, QPalette::ColorRole bgrole);

static const int button_edge_size = 16;
static const int button_inner_width = 32;

void paintCommandButtonPanel(QPainter *painter, const QStyleOptionButton *option, const QWidget *widget)
{
	Q_UNUSED(widget);
	QPalette::ColorRole bgrole = /*widget ? widget->backgroundRole() : */QPalette::Button;

	bool useCache = UsePixmapCache;
	QString pixmapName;
	QPixmap pixmap;
	QRect r = option->rect;
	r.setWidth(button_inner_width + 2 * button_edge_size);

	if (/*option->state & (QStyle::State_HasFocus | QStyle::State_MouseOver) ||*/ r.height() > 64) {
		useCache = false;
	}
	if (useCache) {
		uint state = uint(option->state) & (QStyle::State_Enabled | QStyle::State_On | QStyle::State_MouseOver | QStyle::State_Sunken | QStyle::State_HasFocus);
		uint features = uint(option->features) & (QStyleOptionButton::Flat | QStyleOptionButton::DefaultButton);
		if (!(state & QStyle::State_Enabled)) {
			state &= ~(QStyle::State_MouseOver | QStyle::State_HasFocus);
		}
        pixmapName = QString("scp-cbp-%1-%2-%3-%4-%5-%6").arg(features).arg(uint(bgrole))
                                                         .arg(state)   .arg(option->direction)
                                                         .arg(option->palette.cacheKey()).arg(r.height());
		
	}
    if (!useCache || !QPixmapCache::find(pixmapName, &pixmap)) {
		pixmap =  QPixmap(r.size());
		pixmap.fill(Qt::transparent);
	//	pixmap.fill(Qt::red);
		QPainter p(&pixmap);
		QStyleOptionButton but = *option;
		but.rect = QRect(QPoint(0, 0), r.size());
	//	### neither Clear nor Source works?
	//	p.setCompositionMode(QPainter::CompositionMode_Clear);
	//	p.setCompositionMode(QPainter::CompositionMode_Source);
	//	p.fillRect(but.rect, Qt::transparent);
	//	p.setCompositionMode(QPainter::CompositionMode_SourceOver);
		p.setFont(painter->font());
                p.setRenderHint(QPainter::Antialiasing, true);
		paintButtonPanel(&p, &but, bgrole);
		p.end();
		if (useCache) {
			QPixmapCache::insert(pixmapName, pixmap);
		//	qDebug() << "inserted into cache:" << pixmapName;
		}
	}
	int rem;
	if (option->rect.width() == r.width()) {
		rem = r.width();
	} else {
		int side = qMin(option->rect.width() / 2, button_inner_width + button_edge_size);
		painter->drawPixmap(r.topLeft(), pixmap, QRect(0, 0, side, r.height()));
		int midw = option->rect.width() - 2 * side;
		rem = option->rect.width() - side;
		r.translate(side, 0);
		while (midw > 0) {
			int w = qMin(button_inner_width, midw);
			rem -= w;
			painter->drawPixmap(r.topLeft(), pixmap, QRect(button_edge_size, 0, w, r.height()));
			r.translate(w, 0);
			midw -= button_inner_width;
		}
	}
	painter->drawPixmap(r.topLeft(), pixmap, QRect(r.width() - rem, 0, rem, r.height()));
}


/*-----------------------------------------------------------------------*/

void paintPanelButtonTool(QPainter *painter, const QStyleOption *option, const QWidget *widget, const QStyle *style)
{
	Q_UNUSED(style);
	QStyleOptionButton button;

	if (widget && !qstrcmp(widget->metaObject()->className(), "QDockWidgetTitleButton")) {
		if (!(option->state & QStyle::State_MouseOver) && !(option->state & QStyle::State_On)) return;
	}
	button.QStyleOption::operator=(*option);
	button.features = QStyleOptionButton::None;
	if (!(button.state & QStyle::State_Enabled) && (option->state & QStyle::State_AutoRaise)) {
		return;
	}
	// ### don't know if tool buttons should have that big frame...
	button.rect.adjust(-1, -1, 1, 1);
	// FIXME bgrole?
	paintCommandButtonPanel(painter, &button, 0);
}


/*-----------------------------------------------------------------------*/

static void paintIndicatorCached(QPainter *painter, const QStyleOption *option, void (*paintIndicator)(QPainter *painter, const QStyleOption *option), bool useCache, const QString &pixmapName)
{
	QPixmap pixmap;

    if (!useCache || !QPixmapCache::find(pixmapName, &pixmap)) {
		pixmap =  QPixmap(option->rect.size());
#if 1
		pixmap.fill(Qt::transparent);
	//	pixmap.fill(Qt::red);
#else
		pixmap.fill(option->palette.color(QPalette::Window));
#endif
		QPainter p(&pixmap);
		QStyleOption opt = *option;
		opt.rect = QRect(QPoint(0, 0), option->rect.size());
	//	p.setCompositionMode(QPainter::CompositionMode_Clear);
	//	p.setCompositionMode(QPainter::CompositionMode_Source);
	//	p.fillRect(opt.rect, Qt::transparent);
	//	p.setCompositionMode(QPainter::CompositionMode_SourceOver);
		p.setFont(painter->font());
		p.setRenderHint(QPainter::Antialiasing, true);
		paintIndicator(&p, &opt);
		p.end();
		if (useCache) {
			QPixmapCache::insert(pixmapName, pixmap);
		//	qDebug() << "inserted into cache:" << pixmapName;
		}
	}
	painter->drawPixmap(option->rect, pixmap);
}


static void paintIndicatorShape(QPainter *painter, const QStyleOption *option, qreal scale, const QPainterPath &shapePath)
{
    // configuration
    const QPalette::ColorRole indicatorRole = QPalette::Text;

    if (option->state & QStyle::State_Sunken || option->state & QStyle::State_On || option->state & QStyle::State_MouseOver) {
        painter->save();
        painter->setPen(Qt::NoPen);
        painter->translate(QRectF(option->rect).center());
        painter->setRenderHint(QPainter::Antialiasing, true);
        QColor color;
        if ((option->state & QStyle::State_MouseOver || option->state & QStyle::State_Sunken) && option->state & QStyle::State_Enabled) {
            color = option->palette.color(QPalette::Highlight);
            if (!(option->state & QStyle::State_Sunken) && !(option->state & QStyle::State_On)) {
                color.setAlpha(80);
            }
        } else if (!(option->state & QStyle::State_Sunken) && option->state & QStyle::State_On) {
            color = option->palette.color(indicatorRole);
            color.setAlpha(80);
        }
        if (color.isValid()) {
            painter->setBrush(color);
            QTransform transform(scale, 0, 0, scale, 0, 0);
            painter->drawPath(transform.map(shapePath));
        }
        if (!(option->state & QStyle::State_Sunken) && option->state & QStyle::State_On) {
            painter->setBrush(option->palette.brush(indicatorRole));
            QTransform transform(scale - 1, 0, 0, scale - 1, 0, 0);
            painter->drawPath(transform.map(shapePath));
        }
        painter->restore();
    }
}


/*-----------------------------------------------------------------------*/

#define csx 0.35
#define csc 0.2

// cross
static const ShapeFactory::Code checkShapeDescription1[] = {
    Pmove(-1 + csc, -1), Pline(0, -csx), Pline(1 - csc, -1), Pline(1, -1 + csc),
    Pline(csx, 0), Pline(1, 1 - csc), Pline(1 - csc, 1), Pline(0, csx),
    Pline(-1 + csc, 1), Pline(-1, 1 - csc), Pline(-csx, 0), Pline(-1, -1 + csc), Pend
};

// checkmark
static const ShapeFactory::Code checkShapeDescription2[] = {
    Pmove(1 - csc, -1), Pline(1, -1 + csc), Pline(csx, 1), Pline(-csx, 1), Pline(-1, csc),
    Pline(-1 + csc, 0), Pline(0, 1 - 2 * csx), Pend
};


static void paintCheckBox(QPainter *painter, const QStyleOption *option)
{
    if (option->state & QStyle::State_NoChange) {
        paintThinFrame(painter, option->rect, option->palette, 30, -10);
        paintThinFrame(painter, option->rect.adjusted(1, 1, -1, -1), option->palette, -50, -60);
        paintThinFrame(painter, option->rect.adjusted(2, 2, -2, -2), option->palette, 0, 60);
        QColor color = option->palette.color(QPalette::Window);
        if (option->state & QStyle::State_Enabled) {
            if (option->state & QStyle::State_Sunken) {
                color = color.darker(110);
            } else if (option->state & QStyle::State_MouseOver) {
                color = color.lighter(106);
            }
        } else {
            color = color.darker(106);
        }
        painter->fillRect(option->rect.adjusted(3, 3, -3, -3), color);
    } else {
        QColor color = option->palette.color(QPalette::Base);
        if (!(option->state & QStyle::State_On) && !(option->state & QStyle::State_Enabled)) {
            color = option->palette.color(QPalette::Window);
        } else if (option->state & QStyle::State_MouseOver) {
            color = color.lighter(105);
        }
        painter->fillRect(option->rect.adjusted(2, 2, -2, -2), color);
        paintRecessedFrame(painter, option->rect, option->palette, RF_Small);
        if (!(option->state & QStyle::State_Sunken)) {
            if (option->state & QStyle::State_Enabled) {
                paintThinFrame(painter, option->rect.adjusted(2, 2, -2, -2), option->palette, 140, 200);
            } else {
                paintThinFrame(painter, option->rect.adjusted(2, 2, -2, -2), option->palette, 180, 180);
            }
        }
        const ShapeFactory::Description description = checkShapeDescription1;
        const qreal scale = (option->rect.width() - 4) * 0.35;
        paintIndicatorShape(painter, option, scale, ShapeFactory::createShape(description));
    }
}


void paintIndicatorCheckBox(QPainter *painter, const QStyleOptionButton *option)
{
	bool useCache = UsePixmapCache;
	QString pixmapName;

	if (/* option->state & (QStyle::State_HasFocus | QStyle::State_MouseOver) ||*/ option->rect.width() * option->rect.height() > 4096) {
		useCache = false;
	}
	if (useCache) {
		uint state = uint(option->state) & (QStyle::State_Enabled | QStyle::State_On | QStyle::State_NoChange | QStyle::State_MouseOver | QStyle::State_Sunken | QStyle::State_HasFocus);
		if (!(state & QStyle::State_Enabled)) {
			state &= ~(QStyle::State_MouseOver | QStyle::State_HasFocus);
		}
		state &= ~(QStyle::State_HasFocus);
        pixmapName = QString("scp-irb-%1-%2-%3-%4-%5").arg(state).arg(option->direction)
                                                      .arg(option->palette.cacheKey())
                                                      .arg(option->rect.width()).arg(option->rect.height());
	}
	paintIndicatorCached(painter, option, paintCheckBox, useCache, pixmapName);
}


/*-----------------------------------------------------------------------*/

static void paintThinBevel(QPainter *painter, const QPainterPath &path, const QColor &dark, const QColor &light, qreal lightAngle = M_PI / 4)
{
    QTransform scaleUp;
    scaleUp.scale(10, 10);
    QList<QPolygonF> bevel = path.toSubpathPolygons(scaleUp);
    Q_FOREACH (QPolygonF polygon, bevel) {
        for (int i = 0; i < polygon.size() - 1; ++i) {
            QLineF line(polygon.at(i) / 10, polygon.at(i + 1) / 10);
            line.setLength(line.length() + 0.20);
            painter->setPen(QPen(blend_color(light, dark, sin(atan2(polygon.at(i + 1).y() - polygon.at(i).y(), polygon.at(i + 1).x() - polygon.at(i).x()) - lightAngle) / 2 + 0.5), 1.0, Qt::SolidLine, Qt::FlatCap));
            painter->drawLine(line);
        }
    }
}


static void paintThinBevel(QPainter *painter, const QPainterPath &path, const QPalette &palette, int dark, int light, qreal lightAngle = M_PI / 4)
{
    paintThinBevel(painter, path, shaded_color(palette.color(QPalette::Window), dark), shaded_color(palette.color(QPalette::Window), light), lightAngle);
}


static inline QPainterPath radioShape(const QRectF rect)
{
    QPainterPath path;
    path.addEllipse(rect);
    return path;
}


static void paintRadioButton(QPainter *painter, const QStyleOption *option)
{
    const qreal lightAngle = option->direction == Qt::LeftToRight ? M_PI / 4 : 3 * M_PI / 4;
    QColor color = option->palette.color(QPalette::Base);
    if (!(option->state & QStyle::State_On) && !(option->state & QStyle::State_Enabled)) {
        color = option->palette.color(QPalette::Window);
    } else if (option->state & QStyle::State_MouseOver) {
        color = color.lighter(105);
    }
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::NoPen);
    painter->setBrush(color);
    painter->drawPath(radioShape(QRectF(option->rect).adjusted(2, 2, -2, -2)));
    paintThinBevel(painter, radioShape(QRectF(option->rect).adjusted(0.5, 0.5, -0.5, -0.5)), option->palette, 39, -26, lightAngle);
    paintThinBevel(painter, radioShape(QRectF(option->rect).adjusted(1.5, 1.5, -1.5, -1.5)), option->palette, -26, -91, lightAngle);
    paintThinBevel(painter, radioShape(QRectF(option->rect).adjusted(2.5, 2.5, -2.5, -2.5)), QColor(0, 0, 0, 15), QColor(0, 0, 0, 30), lightAngle);
    paintThinBevel(painter, radioShape(QRectF(option->rect).adjusted(3.5, 3.5, -3.5, -3.5)), QColor(0, 0, 0, 8), QColor(0, 0, 0, 15), lightAngle);
    paintThinBevel(painter, radioShape(QRectF(option->rect).adjusted(4.5, 4.5, -4.5, -4.5)), QColor(0, 0, 0, 4), QColor(0, 0, 0, 8), lightAngle);
    if (!(option->state & QStyle::State_Sunken)) {
        if (option->state & QStyle::State_Enabled) {
            paintThinBevel(painter, radioShape(QRectF(option->rect).adjusted(2.5, 2.5, -2.5, -2.5)), option->palette, 140, 300, lightAngle);
        } else {
            paintThinBevel(painter, radioShape(QRectF(option->rect).adjusted(2.5, 2.5, -2.5, -2.5)), option->palette, 180, 180, lightAngle);
        }
    }
    painter->restore();
    const qreal scale = (option->rect.width() - 4) * 0.35;
    QPainterPath circlePath;
    const qreal radius = 0.7;
    circlePath.addEllipse(QRectF(-radius, -radius, 2 * radius, 2 * radius));
    paintIndicatorShape(painter, option, scale, circlePath);
}


void paintIndicatorRadioButton(QPainter *painter, const QStyleOptionButton *option)
{
	bool useCache = UsePixmapCache;
	QString pixmapName;

	if (/* option->state & (QStyle::State_HasFocus | QStyle::State_MouseOver) ||*/ option->rect.width() * option->rect.height() > 4096) {
		useCache = false;
	}
	if (useCache) {
		uint state = uint(option->state) & (QStyle::State_Enabled | QStyle::State_On | QStyle::State_MouseOver | QStyle::State_Sunken | QStyle::State_HasFocus);
		if (!(state & QStyle::State_Enabled)) {
			state &= ~(QStyle::State_MouseOver | QStyle::State_HasFocus);
		}
		state &= ~(QStyle::State_HasFocus);
        pixmapName = QString("scp-irb-%1-%2-%3-%4-%5").arg(state).arg(option->direction)
                                                      .arg(option->palette.cacheKey())
                                                      .arg(option->rect.width()).arg(option->rect.height());
	}
	paintIndicatorCached(painter, option, paintRadioButton, useCache, pixmapName);
}


/*-----------------------------------------------------------------------*/

void paintIndicatorMenuCheckMark(QPainter *painter, const QStyleOptionMenuItem *option, const QWidget *widget, const QStyle *style)
{
	QStyleOptionButton buttonOption;

	buttonOption.QStyleOption::operator=(*option);
//	buttonOption.rect.adjust(-2, -2, 2, 2);
//	qDebug("here!");
//	printf("state 0x%08x\n", uint(buttonOption.state));
        if (option->state & QStyle::State_Enabled) {
		if (buttonOption.state & QStyle::State_On) {
			buttonOption.state |= QStyle::State_Sunken;
		}
	} else {
            buttonOption.state &= ~QStyle::State_Sunken;
        }
        if (option->state & QStyle::State_Selected) {
            buttonOption.state |= QStyle::State_MouseOver;
        } else {
            buttonOption.state &= ~QStyle::State_MouseOver;
        }
        if (option->checked) {
            buttonOption.state |= QStyle::State_On;
        } else {
            buttonOption.state &= ~QStyle::State_On;
        }
	if (widget) {
		buttonOption.palette = widget->palette();
		if (option->state & QStyle::State_Enabled) {
			if (option->state & QStyle::State_Active) {
				buttonOption.palette.setCurrentColorGroup(QPalette::Active);
			} else {
				buttonOption.palette.setCurrentColorGroup(QPalette::Inactive);
			}
		} else {
			buttonOption.palette.setCurrentColorGroup(QPalette::Disabled);
		}
	}
	if (option->checkType == QStyleOptionMenuItem::Exclusive) {
		QSize size(style->pixelMetric(QStyle::PM_ExclusiveIndicatorWidth, option, widget), style->pixelMetric(QStyle::PM_ExclusiveIndicatorHeight, option, widget));
                buttonOption.rect = QRect(option->rect.x() + ((option->rect.width() - size.width()) >> 1), option->rect.y() + ((option->rect.height() - size.height()) >> 1), size.width(), size.height());
		paintIndicatorRadioButton(painter, &buttonOption);
	} else {
		QSize size(style->pixelMetric(QStyle::PM_IndicatorWidth, option, widget), style->pixelMetric(QStyle::PM_IndicatorHeight, option, widget));
                buttonOption.rect = QRect(option->rect.x() + ((option->rect.width() - size.width()) >> 1), option->rect.y() + ((option->rect.height() - size.height()) >> 1), size.width(), size.height());
                paintIndicatorCheckBox(painter, &buttonOption);
	}
}

#ifdef QT3_SUPPORT
void paintQ3CheckListIndicator(QPainter *painter, const QStyleOptionQ3ListView *option, const QWidget *widget, const QStyle *style)
{
	if (!option->items.isEmpty()) {
		QStyleOptionButton buttonOption;

		buttonOption.QStyleOption::operator=(*option);
		QSize size(style->pixelMetric(QStyle::PM_IndicatorWidth, option, widget), style->pixelMetric(QStyle::PM_IndicatorHeight, option, widget));
		buttonOption.rect = QRect(option->rect.center() - QPoint(size.width() / 2, size.height() / 2), size);
//		buttonOption.rect.adjust(0, -1, 0, -1);
		paintIndicatorCheckBox(painter, &buttonOption);
	}
}


void paintQ3CheckListExclusiveIndicator(QPainter *painter, const QStyleOptionQ3ListView *option, const QWidget *widget, const QStyle *style)
{
	if (!option->items.isEmpty()) {
		QStyleOptionButton buttonOption;

		buttonOption.QStyleOption::operator=(*option);
		QSize size(style->pixelMetric(QStyle::PM_ExclusiveIndicatorWidth, option, widget), style->pixelMetric(QStyle::PM_ExclusiveIndicatorHeight, option, widget));
		buttonOption.rect = QRect(option->rect.center() - QPoint(size.width() / 2, size.height() / 2), size);
//		buttonOption.rect.adjust(0, -1, 0, -1);
		paintIndicatorRadioButton(painter, &buttonOption);
	}
}
#endif

void paintIndicatorItemViewItemCheck(QPainter *painter, const QStyleOption *option)
{
	QStyleOptionButton buttonOption;

	buttonOption.QStyleOption::operator=(*option);
	buttonOption.state &= ~QStyle::State_MouseOver;
	paintIndicatorCheckBox(painter, &buttonOption);
}


/*-----------------------------------------------------------------------*/

static void paintGrip(QPainter *painter, const QStyleOption *option)
{
//	painter->fillRect(option->rect, Qt::red);
	int d = qMin(option->rect.width(), option->rect.height());
	// good values are 3 (very small), 4 (small), 5 (good), 7 (large), 9 (huge)
	// int d = 5;
	QRectF rect(QRectF(option->rect).center() - QPointF(d / 2.0, d / 2.0), QSizeF(d, d));
	const qreal angle = option->direction == Qt::LeftToRight ? 135.0 : 45.0;
//	const qreal angle = 90;
	QColor color;
        qreal opacity = 0.9;

	painter->save();
	painter->setPen(Qt::NoPen);
	if (option->state & QStyle::State_Enabled) {
		if (option->state & QStyle::State_Sunken) {
			color = option->palette.color(QPalette::Highlight).darker(110);
		} else {
			color = option->palette.color(QPalette::Button);
		}
	} else {
		color = option->palette.color(QPalette::Button);
		opacity = 0.5;
	}

	QConicalGradient gradient1(rect.center(), angle);
	gradient1.setColorAt(0.0, shaded_color(color, -110));
	gradient1.setColorAt(0.25, shaded_color(color, -30));
	gradient1.setColorAt(0.5, shaded_color(color, 180));
	gradient1.setColorAt(0.75, shaded_color(color, -30));
	gradient1.setColorAt(1.0, shaded_color(color, -110));
	painter->setBrush(color);
	painter->drawEllipse(rect);
	painter->setBrush(gradient1);
        // ### merge opacity into color
        painter->setOpacity(opacity);
	painter->drawEllipse(rect);
        painter->setOpacity(1.0);
	if (d > 2) {
		QConicalGradient gradient2(rect.center(), angle);
		gradient2.setColorAt(0.0, shaded_color(color, -40));
		gradient2.setColorAt(0.25, shaded_color(color, 0));
		gradient2.setColorAt(0.5, shaded_color(color, 210));
		gradient2.setColorAt(0.75, shaded_color(color, 0));
		gradient2.setColorAt(1.0, shaded_color(color, -40));
		rect.adjust(1, 1, -1, -1);
		painter->setBrush(color);
		painter->drawEllipse(rect);
		painter->setBrush(gradient2);
		painter->setOpacity(opacity);
		painter->drawEllipse(rect);
		painter->setOpacity(1.0);
		if (d > 8) {
			QConicalGradient gradient3(rect.center(), angle);
			gradient3.setColorAt(0.0, shaded_color(color, -10));
			gradient3.setColorAt(0.25, shaded_color(color, 0));
			gradient3.setColorAt(0.5, shaded_color(color, 180));
			gradient3.setColorAt(0.75, shaded_color(color, 0));
			gradient3.setColorAt(1.0, shaded_color(color, -10));
			rect.adjust(2, 2, -2, -2);
			painter->setBrush(color);
			painter->drawEllipse(rect);
			painter->setBrush(gradient3);
			painter->setOpacity(opacity);
			painter->drawEllipse(rect);
			painter->setOpacity(1.0);
		}
	}
	painter->restore();
}


void paintCachedGrip(QPainter *painter, const QStyleOption *option, QPalette::ColorRole /*bgrole*/)
{
	bool useCache = UsePixmapCache;
	QString pixmapName;

	if (/* option->state & (QStyle::State_HasFocus | QStyle::State_MouseOver) ||*/ option->rect.width() * option->rect.height() > 4096) {
		useCache = false;
	}
	if (useCache) {
		uint state = uint(option->state) & (QStyle::State_Enabled | QStyle::State_On | QStyle::State_MouseOver | QStyle::State_Sunken | QStyle::State_HasFocus);
		if (!(state & QStyle::State_Enabled)) {
			state &= ~(QStyle::State_MouseOver | QStyle::State_HasFocus);
		}
		state &= ~(QStyle::State_HasFocus);
                QByteArray colorName = option->palette.color(QPalette::Button).name().toLatin1();
                pixmapName = QString("scp-isg-%1-%2-%3-%4-%5").arg(state).arg(option->direction)
                                                              .arg(colorName.constData())
                                                              .arg(option->rect.width()).arg(option->rect.height());
	}
	paintIndicatorCached(painter, option, paintGrip, useCache, pixmapName);
}


/*-----------------------------------------------------------------------*/

void paintDialBase(QPainter *painter, const QStyleOption *option)
{
//	painter->fillRect(option->rect, Qt::red);
//	painter->save();
//	painter->setRenderHint(QPainter::Antialiasing, true);
	int d = qMin(option->rect.width(), option->rect.height());
/*	if (d > 20 && option->notchTarget > 0) {
		d += -1;
	}
*/	QRectF r((option->rect.width() - d) / 2.0, (option->rect.height() - d) / 2.0, d, d);
	const qreal angle = option->direction == Qt::LeftToRight ? 135.0 : 45.0;
//	const qreal angle = 90;

	painter->setPen(Qt::NoPen);
	QColor border_color = option->palette.color(QPalette::Window);
#if 0
	{
		QRadialGradient depth_gradient(r.center(), d / 2);
//		depth_gradient.setColorAt(0.0, QColor(0, 0, 0, 255));
		depth_gradient.setColorAt(0.5, QColor(0, 0, 0, 255));
		depth_gradient.setColorAt(1.0, QColor(0, 0, 0, 0));
		painter->setBrush(depth_gradient);
		painter->drawEllipse(r);
	}
#endif
#if 1
	if (option->state & QStyle::State_HasFocus && option->state & QStyle::State_KeyboardFocusChange) {
		painter->setBrush(option->palette.color(QPalette::Highlight).darker(180));
		r.adjust(1, 1, -1, -1);
		painter->drawEllipse(r);
		painter->setBrush(border_color);
		r.adjust(1, 1, -1, -1);
		painter->drawEllipse(r);
		r.adjust(1, 1, -1, -1);
	} else {
		painter->setBrush(border_color);
		r.adjust(1, 1, -1, -1);
		painter->drawEllipse(r);
		r.adjust(1, 1, -1, -1);
		QConicalGradient border_gradient(r.center(), angle);
		if (!(option->state & QStyle::State_Enabled)) {
			border_color = border_color.lighter(120);
		}
		border_gradient.setColorAt(0.0, border_color.darker(180));
		border_gradient.setColorAt(0.3, border_color.darker(130));
		border_gradient.setColorAt(0.5, border_color.darker(170));
		border_gradient.setColorAt(0.7, border_color.darker(130));
		border_gradient.setColorAt(1.0, border_color.darker(180));
		painter->setBrush(border_gradient);
//		painter->setBrush(Qt::blue);
		painter->drawEllipse(r);
		r.adjust(1, 1, -1, -1);
	}
	d -= 6;

	QColor dial_color;
	if (option->state & QStyle::State_Enabled) {
		dial_color = option->palette.color(QPalette::Button).lighter(101);
		if (option->state & QStyle::State_MouseOver) {
			dial_color = dial_color.lighter(103);
		}
	} else {
		dial_color = option->palette.color(QPalette::Window);
	}
	qreal t = option->state & QStyle::State_Enabled ? 2.0 : 1.5;
	if (1) {
		// ###: work around Qt 4.3.0 bug? (this works for 4.3.1)
		QConicalGradient border_gradient(r.center(), angle);
		border_gradient.setColorAt(0.0, dial_color.lighter(120));
		border_gradient.setColorAt(0.2, dial_color);
		border_gradient.setColorAt(0.5, dial_color.darker(130));
		border_gradient.setColorAt(0.8, dial_color);
		border_gradient.setColorAt(1.0, dial_color.lighter(120));
		painter->setPen(QPen(border_gradient, t));
	} else {
		painter->setPen(QPen(Qt::red, t));
	}
#if 0
	QLinearGradient dial_gradient(r.topLeft(), r.bottomLeft());
	dial_gradient.setColorAt(0.0, dial_color.darker(105));
	dial_gradient.setColorAt(0.5, dial_color.lighter(102));
	dial_gradient.setColorAt(1.0, dial_color.lighter(105));
#elif 1
              QLinearGradient dial_gradient(option->direction == Qt::LeftToRight ? r.topLeft() : r.topRight(), option->direction == Qt::LeftToRight ? r.bottomRight() : r.bottomLeft());
//	QLinearGradient dial_gradient(r.topLeft(), r.bottomLeft());
	if (true || option->state & QStyle::State_Enabled) {
#if 1
		dial_gradient.setColorAt(0.0, dial_color.darker(106));
		dial_gradient.setColorAt(1.0, dial_color.lighter(104));
#else
		dial_gradient.setColorAt(0.0, dial_color.lighter(101));
		dial_gradient.setColorAt(0.5, dial_color.darker(103));
		dial_gradient.setColorAt(1.0, dial_color.lighter(104));
#endif
	} else {
		dial_gradient.setColorAt(0.0, dial_color);
		dial_gradient.setColorAt(1.0, dial_color);
	}
#elif 0
	QConicalGradient dial_gradient(r.center(), angle);
	dial_gradient.setColorAt(0.0, dial_color.lighter(102));
	dial_gradient.setColorAt(0.5, dial_color.darker(103));
	dial_gradient.setColorAt(1.0, dial_color.lighter(102));
#else
	QBrush dial_gradient(dial_color);
#endif
	painter->setBrush(dial_gradient);
	t = t / 2;
	painter->drawEllipse(r.adjusted(t, t, -t, -t));

//	painter->setPen(Qt::NoPen);
//	painter->setBrush(dial_color);
//	painter->drawEllipse(r.adjusted(d / 4, d / 4, - d / 4, - d / 4));

#if 0
	QLinearGradient border2_gradient(r.topLeft(), r.bottomRight());
	border2_gradient.setColorAt(1.0, dial_color.darker(425));
	border2_gradient.setColorAt(0.9, dial_color);
	border2_gradient.setColorAt(0.0, dial_color.darker(400));
	painter->setPen(QPen(border2_gradient, 1.3));
	painter->setBrush(Qt::NoBrush);
	painter->drawEllipse(r.adjusted(0.3, 0.3, -0.3, -0.3));
#endif
//	painter->restore();
#endif
}


void paintCachedDialBase(QPainter *painter, const QStyleOptionSlider *option)
{
	bool useCache = UsePixmapCache;
	QString pixmapName;
	QRect r = option->rect;
	int d = qMin(r.width(), r.height());

	if (/* option->state & (QStyle::State_HasFocus | QStyle::State_MouseOver) ||*/ d > 128) {
		useCache = false;
	}
	if (useCache) {
		uint state = uint(option->state) & (QStyle::State_Enabled | QStyle::State_On | QStyle::State_MouseOver | QStyle::State_KeyboardFocusChange | QStyle::State_HasFocus);
		if (!(state & QStyle::State_Enabled)) {
			state &= ~(QStyle::State_MouseOver | QStyle::State_HasFocus | QStyle::State_KeyboardFocusChange);
		}
	//	state &= ~(QStyle::State_HasFocus);
        pixmapName = QString("scp-qdb-%1-%2-%3-%4").arg(state).arg(option->direction)
                                                   .arg(option->palette.cacheKey())
                                                   .arg(d);
	}
	paintIndicatorCached(painter, option, paintDialBase, useCache, pixmapName);
}


void paintIndicatorDial(QPainter *painter, const QStyleOptionSlider *option)
{
	int d = qMin(option->rect.width(), option->rect.height());
	QRect rect(option->rect.center() - QPoint((d - 1) / 2, (d - 1) / 2), QSize(d, d));
	QStyleOptionSlider opt;
	opt.QStyleOption::operator=(*option);
	opt.rect = rect;
	paintCachedDialBase(painter, &opt);
}


/*-----------------------------------------------------------------------*/

void paintBranchChildren(QPainter *painter, const QStyleOption *option)
{
	painter->setBrush(option->palette.color(QPalette::Text));
	painter->setPen(Qt::NoPen);
        const qreal r = qMin(option->rect.width() * 0.5, option->fontMetrics.height() * 0.15);
        const QPointF center = QRectF(option->rect).center();
	painter->drawEllipse(QRectF(center.x() - r, center.y() - r, 2 * r, 2 * r));
}


void paintCachedIndicatorBranchChildren(QPainter *painter, const QStyleOption *option)
{
	bool useCache = UsePixmapCache;
	QString pixmapName;
	QRect r = option->rect;
	int d = qMin(r.width(), r.height());

	if (/* option->state & (QStyle::State_HasFocus | QStyle::State_MouseOver) ||*/ d > 64) {
		useCache = false;
	}
	if (useCache) {
		uint state = uint(option->state) & (QStyle::State_Enabled | QStyle::State_Open);
	//	if (!(state & QStyle::State_Enabled)) {
	//		state &= ~(QStyle::State_MouseOver | QStyle::State_HasFocus | QStyle::State_KeyboardFocusChange);
	//	}
	//	state &= ~(QStyle::State_HasFocus);
        pixmapName = QString("scp-qibc-%1-%2-%3-%4").arg(state).arg(option->direction)
                                                    .arg(option->palette.cacheKey())
                                                    .arg(d);
	}
	paintIndicatorCached(painter, option, paintBranchChildren, useCache, pixmapName);
}


