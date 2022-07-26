/*
 * skulpture_mdi.cpp
 *
 */

#include "skulpture_p.h"
#include <QtGui/QPainter>
#include <QtWidgets/QMdiSubWindow>
#include <QtCore/QSettings>
#include <cmath>


/*-----------------------------------------------------------------------*/

QRect subControlRectTitleBar(const QStyleOptionTitleBar *option, QStyle::SubControl subControl, const QWidget *widget, const QStyle *style)
{
    QRect r = ((const QCommonStyle *) style)->QCommonStyle::subControlRect(QStyle::CC_TitleBar, option, subControl, widget);
    if (subControl != QStyle::SC_TitleBarSysMenu) {
        return r.adjusted(option->direction == Qt::LeftToRight ? -2 : 2, -2, option->direction == Qt::LeftToRight ? -3 : 3, -3);
    } else {
        return r.adjusted(0, -1, 0, -1);
    }
}


/*-----------------------------------------------------------------------*/

void paintFrameWindow(QPainter *painter, const QStyleOptionFrame *option)
{
//	painter->fillRect(option->rect, option->palette.color(QPalette::Window));
#if 0
	paintThinFrame(painter, option->rect.adjusted(0, 0, 0, 0), option->palette, -60, 160);
	paintThinFrame(painter, option->rect.adjusted(1, 1, -1, -1), option->palette, -20, 60);
#else
	paintThinFrame(painter, option->rect.adjusted(0, 0, 0, 0), option->palette, -90, 355);
	paintThinFrame(painter, option->rect.adjusted(1, 1, -1, -1), option->palette, -40, 100);
#endif
//	paintThinFrame(painter, option->rect.adjusted(4, 7 + option->fontMetrics.height(), -4, -4), option->palette, 60, -20);
#if 0
	painter->setPen(Qt::red);
	painter->setBrush(Qt::NoBrush);
	painter->drawRect(option->rect.adjusted(0, 0, -1, -1));
#endif
}


static void getTitleBarPalette(QPalette &palette)
{
	QSettings settings(QLatin1String("Trolltech"));
	settings.beginGroup(QLatin1String("Qt"));

	if (settings.contains(QLatin1String("KWinPalette/activeBackground"))) {
		palette.setColor(QPalette::Window, QColor(settings.value(QLatin1String("KWinPalette/inactiveBackground")).toString()));
		palette.setColor(QPalette::WindowText, QColor(settings.value(QLatin1String("KWinPalette/inactiveForeground")).toString()));
		palette.setColor(QPalette::Highlight, QColor(settings.value(QLatin1String("KWinPalette/activeBackground")).toString()));
		palette.setColor(QPalette::HighlightedText, QColor(settings.value(QLatin1String("KWinPalette/activeForeground")).toString()));
	} else {
		palette.setColor(QPalette::Window, QColor(0, 0, 0, 20));
		palette.setColor(QPalette::WindowText, QColor(0, 0, 0, 255));
		QColor barColor = palette.color(QPalette::Highlight);
		barColor.setHsvF(barColor.hueF(), barColor.saturationF() * 0.9, 0.25);
		palette.setColor(QPalette::Highlight, barColor);
		palette.setColor(QPalette::HighlightedText, QColor(255, 255, 255, 240));
	}
}


void paintTitleBar(QPainter *painter, const QStyleOptionTitleBar *option, const QWidget *widget, const QStyle *style)
{
	QColor barColor;
	QColor textColor;

	painter->save();
        qreal opacity = painter->opacity();

	QPalette palette = option->palette;

        if (qobject_cast<const QMdiSubWindow *>(widget)) {
		if (widget->objectName() != QLatin1String("SkulpturePreviewWindow")) {
			getTitleBarPalette(palette);
		}
	}
	if (option->state & QStyle::State_Active) {
		barColor = palette.color(QPalette::Highlight);
		textColor = palette.color(QPalette::HighlightedText);
	} else {
		barColor = palette.color(QPalette::Window);
		textColor = palette.color(QPalette::WindowText);
	}

	QLinearGradient barGradient(option->rect.topLeft() + QPoint(-1, -1), option->rect.bottomLeft() + QPoint(-1, -2));
//	barGradient.setColorAt(0.0, option->palette.color(QPalette::Window));
//	barGradient.setColorAt(0.3, barColor);
//	barGradient.setColorAt(0.7, barColor);
        barGradient.setColorAt(0.0, barColor.darker(105));
        barGradient.setColorAt(1.0, barColor.lighter(120));
//	barGradient.setColorAt(1.0, option->palette.color(QPalette::Window));
//	painter->fillRect(option->rect.adjusted(-1, -1, 1, -2), barGradient);
//	painter->fillRect(option->rect.adjusted(-1, -1, 1, -2), barColor);

#if 1
	{
		QRect r = option->rect.adjusted(-4, -7, 4, 0);
		QRect lr = r.adjusted(6, 2, -6/* - 55*/, -1);
	//	QRect lr = r.adjusted(6, 2, -70, -1);

		if (true || option->state & QStyle::State_Active) {
			painter->fillRect(lr, barColor);
		}

		QStyleOptionTitleBar buttons = *option;
	//	buttons.subControls &= ~QStyle::SC_TitleBarLabel;
		buttons.subControls = QStyle::SC_TitleBarSysMenu;
		buttons.rect.adjust(3, -2, -4, -1);
                painter->setOpacity(option->state & QStyle::State_Active ? opacity : 0.7 * opacity);
		((QCommonStyle *) style)->QCommonStyle::drawComplexControl(QStyle::CC_TitleBar, &buttons, painter, widget);
		buttons = *option;
#if 0
		buttons.subControls &= ~(QStyle::SC_TitleBarLabel | QStyle::SC_TitleBarSysMenu);
		((QCommonStyle *) style)->QCommonStyle::drawComplexControl(QStyle::CC_TitleBar, &buttons, painter, widget);
#else
		QStyleOption opt = *option;
		QIcon icon;

		for (int i = 1; i <= 7; ++i) {
			QStyle::SubControl sc = (QStyle::SubControl) (1 << i);
			if (option->subControls & sc & ~(QStyle::SC_TitleBarContextHelpButton)) {
				QRect rect = style->subControlRect(QStyle::CC_TitleBar, option, sc, widget);
                                QIcon::Mode iconMode = QIcon::Normal;
                                if (option->activeSubControls & sc) {
                                    iconMode = QIcon::Active;
                                }
                                opt.palette.setColor(QPalette::Text, textColor);
                                icon = style->standardIcon((QStyle::StandardPixmap)(QStyle::SP_TitleBarMenuButton + i), &opt, widget);
                                icon.paint(painter, rect, Qt::AlignCenter, iconMode);
			}
		}
#endif
                painter->setOpacity(opacity);
#if 0
		QRect buttonRect = option->rect.adjusted(300, 1, -90, -6);
		paintThinFrame(painter, buttonRect, option->palette, -180, 40);
		paintThinFrame(painter, buttonRect.adjusted(-1, -1, 1, 1), option->palette, 40, -180);
#endif

		{
			QLinearGradient labelGradient(lr.topLeft(), lr.bottomLeft());
#if 0
			labelGradient.setColorAt(0.0, QColor(0, 0, 0, 50));
			labelGradient.setColorAt(0.5, QColor(0, 0, 0, 0));
			labelGradient.setColorAt(0.55, QColor(0, 0, 0, 20));
			labelGradient.setColorAt(1.0, QColor(0, 0, 0, 0));
#elif 1
			labelGradient.setColorAt(0.0, QColor(255, 255, 255, 10));
			labelGradient.setColorAt(0.5, QColor(255, 255, 255, 40));
			labelGradient.setColorAt(0.55, QColor(255, 255, 255, 0));
			labelGradient.setColorAt(1.0, QColor(255, 255, 255, 20));
#else
			labelGradient.setColorAt(0.0, QColor(0, 0, 0, 30));
			labelGradient.setColorAt(1.0, QColor(255, 255, 255, 60));
#endif
			painter->fillRect(lr, labelGradient);
		}

		QLinearGradient barGradient(r.topLeft(), r.bottomLeft());
		barGradient.setColorAt(0.0, QColor(255, 255, 255, 200));
		barGradient.setColorAt(0.2, QColor(255, 255, 255, 80));
		barGradient.setColorAt(0.5, QColor(255, 255, 255, 30));
		barGradient.setColorAt(1.0, QColor(255, 255, 255, 0));
		painter->fillRect(r, barGradient);
#if 0
		QRadialGradient dialogGradient2(r.left() + r.width() / 2, r.top(), r.height());
		dialogGradient2.setColorAt(0.0, QColor(255, 255, 225, 70));
		dialogGradient2.setColorAt(1.0, QColor(0, 0, 0, 0));
		painter->save();
		painter->translate(r.center());
		painter->scale(r.width() / 2.0 / r.height(), 1);
		painter->translate(-r.center());
		painter->fillRect(r.adjusted(1, 1, -1, -1), dialogGradient2);
		painter->restore();
#endif
		paintThinFrame(painter, lr, option->palette, -30, 90);
		paintThinFrame(painter, lr.adjusted(-1, -1, 1, 1), option->palette, 90, -30);

	}
#endif


#if 0
//	paintThinFrame(painter, option->rect.adjusted(0, 0, 0, -1), option->palette, -30, 80);
	paintThinFrame(painter, option->rect.adjusted(-1, -1, 1, 0), option->palette, 80, -30);
	painter->fillRect(option->rect.adjusted(0, 0, 0, -1), barColor);

	// FIXME: adjust rect for new shadow // paintRecessedFrameShadow(painter, option->rect.adjusted(-1, -1, 1, 0), RF_Large);
	{
		QRect labelRect = option->rect.adjusted(20, 0, -250, 0);
		painter->fillRect(labelRect, option->palette.color(QPalette::Window));
		paintThinFrame(painter, labelRect.adjusted(0, 0, 0, 1), option->palette, -30, 80);
	}
#endif

	if (option->subControls & QStyle::SC_TitleBarLabel) {
		QRect labelRect;
		if (qobject_cast<const QMdiSubWindow *>(widget)) {
			QFont font = painter->font();
			font.setBold(true);
                        if (option->direction == Qt::LeftToRight) {
                            labelRect = option->rect.adjusted(option->fontMetrics.height() + 10, -1, -2, -3);
                        } else {
                            labelRect = option->rect.adjusted(1, -1, -option->fontMetrics.height() - 11, -3);
                        }
		//	font.setPointSizeF(10);
			painter->setFont(font);
		} else
                {
			labelRect = style->subControlRect(QStyle::CC_TitleBar, option, QStyle::SC_TitleBarLabel, widget);
		}
                painter->setOpacity(opacity);
		painter->setPen(QColor(0, 0, 0, 25));
                style->drawItemText(painter, labelRect.adjusted(1, 1, 1, 1), Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine, option->palette, true, option->text, QPalette::NoRole);
		//painter->drawText(labelRect.adjusted(1, 1, 1, 1), Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine, option->text);
		painter->setOpacity(option->state & QStyle::State_Active ? opacity : 0.7 * opacity);
		painter->setPen(textColor);
                style->drawItemText(painter, labelRect, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine, option->palette, true, option->text, QPalette::NoRole);
                //painter->drawText(labelRect, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine, option->text);
	}
/*
	if (!option->icon.isNull()) {
		labelRect.setWidth(16);
		labelRect.setHeight(16);
		painter->drawPixmap(labelRect.adjusted(0, -1, 0, -1), option->icon.pixmap(QSize(16, 16)));
	}
*/	painter->restore();
}


int getWindowFrameMask(QStyleHintReturnMask *mask, const QStyleOptionTitleBar *option, const QWidget *widget)
{
	Q_UNUSED(widget);
	mask->region = option->rect;

	// TODO get total dimensions of workspace and don't use masks on corners
	/*if (option->rect.topLeft() != QPoint(0, 0))*/ {
//		mask->region -= QRect(option->rect.topLeft(), QSize(1, 1));
	}
//	mask->region -= QRect(option->rect.topRight(), QSize(1, 1));
//	mask->region -= QRect(option->rect.bottomLeft(), QSize(1, 1));
//	mask->region -= QRect(option->rect.bottomRight(), QSize(1, 1));

	// try new style
//	mask->region -= QRect(option->rect.topLeft(), QSize(6, 1));
//	mask->region -= QRect(option->rect.topRight() - QPoint(5, 0), QSize(6, 1));
	return 1;
}


