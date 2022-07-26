/*
 * skulpture_icons.cpp
 *
 */

#include "skulpture_p.h"
#include "sk_factory.h"
#include <QtCore/QSettings>
#include <QtWidgets/QStyleOption>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QFrame>
#include <QtGui/QPainter>
#include <cstdlib>


/*-----------------------------------------------------------------------*/

QPixmap SkulptureStyle::standardPixmap(QStyle::StandardPixmap standardPixmap, const QStyleOption *option, const QWidget *widget) const
{
	return ParentStyle::standardPixmap(standardPixmap, option, widget);
}


/*-----------------------------------------------------------------------*/
/**
 * decorationShape - get shape for window decoration button
 *
 * The coordinate system is -1 ... 1 for each dimension, with
 * (0, 0) being at the center, and positive coordinates pointing
 * down and to the right.
 *
 */

static const ShapeFactory::Code titleBarMenuButtonDescription[] = {
    Pmove(0, 0.6), Pline(0.6, 0), Pline(0, -0.6), Pline(-0.6, 0), Pend
};

static const ShapeFactory::Code titleBarCloseButtonDescription[] = {
	#define kx3 0.3
	Pmove(-1, -1), Pline(0, -kx3), Pline(1, -1), Pline(kx3, 0), Pline(1, 1), Pline(0, kx3), Pline(-1, 1), Pline(-kx3, 0), Pend
};

static const ShapeFactory::Code titleBarShadeButtonDescription[] = {
	Pmove(-1, -0.4), Pline(0, -0.6), Pline(1, -0.4), Pline(0, -1), Pend
};

static const ShapeFactory::Code titleBarUnshadeButtonDescription[] = {
	Pmove(-1, -1), Pline(0, -0.8), Pline(1, -1), Pline(0, -0.4), Pend
};

static const ShapeFactory::Code titleBarMinButtonDescription[] = {
	Pmove(-1, 0.4), Pline(0, 0.6), Pline(1, 0.4), Pline(0, 1), Pend
};

static const ShapeFactory::Code titleBarMaxButtonDescription[] = {
	#define kx1 0.8
	#define kx2 0.55
	Pmove(0, -1), Pline(1, 0), Pline(0, 1), Pline(-1, 0), Pclose,
	Pmove(0, -kx2), Pline(-kx1, 0), Pline(0, kx2), Pline(kx1, 0), Pend,
};

static const ShapeFactory::Code titleBarNormalButtonDescription[] = {
    Pmove(0, -1), Pline(1, 0), Pline(0, 1), Pline(-1, 0), Pclose,
    Pmove(0, -kx1), Pline(-kx2, 0), Pline(0, kx1), Pline(kx2, 0), Pend,
};

static const ShapeFactory::Code titleBarHelpButtonDescription[] = {
	Pmove(0.0305, 0.513), Pline(-0.0539, 0.513), Pline(0.0117, 0.227), Pline(0.22, -0.0859), Pline(0.38, -0.323),
	Pline(0.417, -0.491), Pline(0.279, -0.767), Pline(-0.0609, -0.87), Pline(-0.342, -0.814), Pline(-0.445, -0.692),
	Pline(-0.383, -0.568), Pline(-0.321, -0.456), Pline(-0.368, -0.373), Pline(-0.483, -0.339), Pline(-0.64, -0.396),
	Pline(-0.71, -0.555), Pline(-0.512, -0.827), Pline(0.0281, -0.947), Pline(0.649, -0.783), Pline(0.797, -0.516),
	Pline(0.73, -0.31), Pline(0.476, -0.0625), Pline(0.111, 0.255), Pclose,
	Pmove(0.00234, 0.681), Pline(0.165, 0.726), Pline(0.232, 0.834), Pline(0.164, 0.943),
	Pline(0.00234, 0.988), Pline(-0.158, 0.943), Pline(-0.225, 0.834), Pline(-0.158, 0.726), Pend
};

static const ShapeFactory::Code titleBarStickyButtonDescription[] = {
	Pmove(0, -1), Pline(0.2, -0.2), Pline(1, 0), Pline(0.2, 0.2), Pline(0, 1),
	Pline(-0.2, 0.2), Pline(-1, 0), Pline(-0.2, -0.2), Pend
};

static const ShapeFactory::Code titleBarUnstickyButtonDescription[] = {
	Pmove(0, -0.2), Pline(1, 0), Pline(0, 0.2), Pline(-1, 0), Pend
};

static const ShapeFactory::Code titleBarAboveButtonDescription[] = {
	Pmove(0, -0.2), Pline(1, 0), Pline(0, 0.2), Pline(-1, 0), Pclose,
	Pmove(-1, -0.4), Pline(0, -0.6), Pline(1, -0.4), Pline(0, -1), Pend
};

static const ShapeFactory::Code titleBarBelowButtonDescription[] = {
	Pmove(0, -0.2), Pline(1, 0), Pline(0, 0.2), Pline(-1, 0), Pclose,
	Pmove(-1, 0.4), Pline(0, 0.6), Pline(1, 0.4), Pline(0, 1), Pend
};

static const ShapeFactory::Code titleBarUnaboveButtonDescription[] = {
	Pmove(0, -0.2), Pline(1, 0), Pline(0, 0.2), Pline(-1, 0), Pclose,
	Pmove(-1, -1), Pline(0, -0.8), Pline(1, -1), Pline(0, -0.4), Pend
};

static const ShapeFactory::Code titleBarUnbelowButtonDescription[] = {
	Pmove(0, -0.2), Pline(1, 0), Pline(0, 0.2), Pline(-1, 0), Pclose,
	Pmove(-1, 1), Pline(0, 0.8), Pline(1, 1), Pline(0, 0.4), Pend
};

static const ShapeFactory::Code toolBarHorizontalExtensionButtonDescription[] = {
	Pmove(-1, -1), Pline(0, 0), Pline(-1, 1), Pline(-0.5, 0), Pclose,
	Pmove(0, -1), Pline(1, 0), Pline(0, 1), Pline(0.5, 0), Pend
};

static const ShapeFactory::Code toolBarVerticalExtensionButtonDescription[] = {
	Pmove(-1, -1), Pline(0, -0.5), Pline(1, -1), Pline(0, 0), Pclose,
	Pmove(-1, 0), Pline(0, 0.5), Pline(1, 0), Pline(0, 1), Pend
};

static const ShapeFactory::Code * const titleBarButtonDescriptions[] = {
	titleBarMenuButtonDescription,
	titleBarMinButtonDescription,
	titleBarMaxButtonDescription,
	titleBarCloseButtonDescription,
	titleBarNormalButtonDescription,
	titleBarShadeButtonDescription,
	titleBarUnshadeButtonDescription,
	titleBarHelpButtonDescription
};

static const ShapeFactory::Code * const titleBarCustomDescriptions[] = {
	0, // CustomBase intentionally left blank
	titleBarStickyButtonDescription,
	titleBarUnstickyButtonDescription,
	0, // this used to be titleBarIconDescription
	titleBarAboveButtonDescription,
	titleBarBelowButtonDescription,
	titleBarUnaboveButtonDescription,
	titleBarUnbelowButtonDescription
};

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
QIcon SkulptureStyle::standardIcon(QStyle::StandardPixmap standardIcon, const QStyleOption *option, const QWidget *widget) const
#else
QIcon SkulptureStyle::standardIconImplementation(QStyle::StandardPixmap standardIcon, const QStyleOption *option, const QWidget *widget) const
#endif
{
	const ShapeFactory::Code *code = 0;
        int numStates = 1;
	int size = 10;

	if (standardIcon > QStyle::SP_CustomBase) {
		if (standardIcon - QStyle::SP_CustomBase < sizeof(titleBarCustomDescriptions) / sizeof(titleBarCustomDescriptions[0])) {
			code = titleBarCustomDescriptions[standardIcon - QStyle::SP_CustomBase];
                        numStates = 3;
		}
	} else if (/*standardIcon >= QStyle::SP_TitleBarMenuButton && */standardIcon <= QStyle::SP_TitleBarContextHelpButton) {
		code = titleBarButtonDescriptions[standardIcon - QStyle::SP_TitleBarMenuButton];
                numStates = 3;
        } else {
		switch (standardIcon) {
			case QStyle::SP_ToolBarHorizontalExtensionButton:
				code = toolBarHorizontalExtensionButtonDescription;
                                numStates = 2;
                                size = 8;
				break;
			case QStyle::SP_ToolBarVerticalExtensionButton:
				code = toolBarVerticalExtensionButtonDescription;
                                numStates = 2;
                                size = 8;
				break;
			default:
				break;
		}
	}
	if (code) {
                QIcon icon;
		bool dock = qobject_cast<const QDockWidget *>(widget) != 0;
		if (dock) {
			size = 14;
                        numStates = 2;
                }
		qreal s = size / 2.0;
                if (numStates == 3) {
                    if (widget && !qstrcmp(widget->metaObject()->className(), "KLineEditButton")) {
                        s = qMin(22, widget->fontMetrics().height()) * 0.25;
                        size += 2;
                    } else if (widget && !qstrcmp(widget->metaObject()->className(), "CloseButton")) {
                        s = qMin(20, widget->fontMetrics().height()) * 0.25;
                    } else {
                        size += 4;
                        if (option) {
                            s = qMin(22, option->fontMetrics.height()) * 0.3;
                        }
                    }
                }
                for (int state = 0; state < numStates; ++state) {
                    QImage image(size, size, QImage::Format_ARGB32);
                    image.fill(0);
                    QPainter painter(&image);
                    painter.setRenderHint(QPainter::Antialiasing, true);
                    painter.translate(size / 2.0, size / 2.0);
                    if (dock) {
                            painter.scale(s - 2, s - 2);
                    } else {
                            painter.scale(s, s);
                    }
                    painter.setPen(Qt::NoPen);
                    QPalette palette;
                    if (option) {
                        palette = option->palette;
                    }
                    QPalette::ColorRole role = QPalette::Text;
                    QColor shapeColor;
                    if (numStates == 2) {
                        if (state == 0) {
                            role = QPalette::WindowText;
                        } else {
                            role = QPalette::ButtonText;
                        }
                        shapeColor = palette.color(role);
                    } else if (numStates == 3) {
                        if (state == 1) {
                            QColor glowColor = palette.color(role);
                            if (standardIcon == QStyle::SP_TitleBarCloseButton) {
                                glowColor = QColor(255, 0, 0, 100);
                            } else {
                                glowColor.setAlpha(50);
                            }
                            painter.fillRect(QRectF(-1.5, -1.5, 3, 3), glowColor);
                            QPainterPath path = ShapeFactory::createShape(code);
                            shapeColor = palette.color(role);
//                            painter.setPen(QPen(shapeColor, 0.3));
                            painter.drawRect(QRectF(-1.5, -1.5, 3, 3));
                            painter.setPen(Qt::NoPen);
                            painter.setBrush(shapeColor);
                            painter.drawPath(path);
                        } else {
                            shapeColor = palette.color(role);
                        }
                    }
                    QColor shadowColor = option ? option->palette.color(QPalette::Shadow) : Qt::black;
                    shadowColor.setAlpha(25);
                    painter.translate(1 / s, 1 / s);
                    painter.setBrush(shadowColor);
                    painter.drawPath(ShapeFactory::createShape(code));
                    painter.translate(-1 / s, -1 / s);
                    painter.setBrush(shapeColor);
                    painter.drawPath(ShapeFactory::createShape(code));
                    painter.end();
                    QIcon::Mode iconMode;
                    switch (state) {
                        case 1:
                            iconMode = QIcon::Active;
                            break;
                        case 2:
                            iconMode = QIcon::Selected;
                            break;
                        default:
                            iconMode = QIcon::Normal;
                            break;
                    }
                    icon.addPixmap(QPixmap::fromImage(image), iconMode);
                }
                return icon;
	}
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        return ParentStyle::standardIcon(standardIcon, option, widget);
#else
	return ParentStyle::standardIconImplementation(standardIcon, option, widget);
#endif
}


QPixmap SkulptureStyle::generatedIconPixmap(QIcon::Mode iconMode, const QPixmap &pixmap, const QStyleOption *option) const
{
	return ParentStyle::generatedIconPixmap(iconMode, pixmap, option);
}


QRect SkulptureStyle::itemPixmapRect(const QRect &rectangle, int alignment, const QPixmap & pixmap) const
{
	return ParentStyle::itemPixmapRect(rectangle, alignment, pixmap);
}


void SkulptureStyle::drawItemPixmap(QPainter *painter, const QRect &rectangle, int alignment, const QPixmap &pixmap) const
{
	ParentStyle::drawItemPixmap(painter, rectangle, alignment, pixmap);
}


