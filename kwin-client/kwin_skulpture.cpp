/*
 * kwin_skulpture.cpp - Skulpture window decoration for KDE
 *
 * Copyright (c) 2008 Christoph Feck <christoph@maxiom.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "kwin_skulpture.h"


#include <QtCore/QCoreApplication>
#include <QtCore/QLibrary>
#include <QtCore/QPluginLoader>
#include <QtGui/QStyle>
#include <QtGui/QStyleFactory>
#include <QtGui/QStyleOption>
#include <QtGui/QStylePainter>
#include <QtGui/QStylePlugin>


/*-----------------------------------------------------------------------*/
/**
 * The Skulpture window decoration for KDE actually is a wrapper.
 * It uses Qt's MDI window decoration. As such, it is very lightweight.
 *
 */

extern "C" KDE_EXPORT KDecorationFactory *create_factory()
{
	return new SkulptureDecorationFactory();
}


/*-----------------------------------------------------------------------*/
/*
 * When KWin is already running, it doesn't notice that
 * a new style has been installed, and QStyleFactory will
 * not find the new style.
 *
 * This function works around this Qt problem by manually
 * loading the style.
 *
 */

static QStyle *loadStyle(const QString &styleName)
{
    QLibrary library;
    Q_FOREACH (const QString &path, QCoreApplication::libraryPaths()) {
        library.setFileName(path + QLatin1String("/styles/") + styleName);
        if (library.load()) {
            QString libName = library.fileName();
            library.unload();
            QPluginLoader plugin(libName);
            QStyleFactoryInterface *factory = qobject_cast<QStyleFactoryInterface*>(plugin.instance());
            if (factory) {
                return factory->create(styleName);
            }
        }
    }
    return 0;
}


/*-----------------------------------------------------------------------*/

QtMdiDecorationButton::QtMdiDecorationButton(ButtonType type, KCommonDecoration *parent)
	: KCommonDecorationButton(type, parent)
{
	init();
}


void QtMdiDecorationButton::init()
{
        const QtMdiDecoration *deco = (const QtMdiDecoration *) decoration();
        if (deco->onScreen() >= 0) {
            setAttribute(Qt::WA_PaintOnScreen, deco->onScreen() == 0 ? false : true);
        }
	setAttribute(Qt::WA_NoSystemBackground, true);
	setAutoFillBackground(false);
	setFocusPolicy(Qt::NoFocus);
	setAttribute(Qt::WA_OpaquePaintEvent, false);
	setAttribute(Qt::WA_Hover, true);
}


QtMdiDecorationButton::~QtMdiDecorationButton()
{
	/* */
}


void QtMdiDecorationButton::reset(unsigned long /*changed*/)
{
	/* NOTE: must be implemented, because it is declared pure */
}


void QtMdiDecorationButton::initStyleOption(QStyleOptionButton &opt)
{
        const QtMdiDecoration *deco = (const QtMdiDecoration *) decoration();
        const bool isActive = deco->isActive();
        QFont font = deco->options()->font(isActive);

	opt.init(this);
        font.setPointSizeF(font.pointSizeF() * deco->symbolSize);
        opt.fontMetrics = QFontMetrics(font);
	if (isDown()) {
		opt.state |= QStyle::State_Selected;
	} else {
		opt.state &= ~QStyle::State_Selected;
	}
	if (isActive) {
		opt.state |= QStyle::State_Active;
	} else {
		opt.state &= ~QStyle::State_Active;
	}
        opt.palette.setColor(QPalette::Window, deco->options()->color(KCommonDecoration::ColorTitleBar, isActive));
}


void QtMdiDecorationButton::paintEvent(QPaintEvent */* event */)
{
	const QtMdiDecoration *deco = (const QtMdiDecoration *) decoration();
	QPainter painter(this);
	QStyleOptionButton opt;
	initStyleOption(opt);
        const int iconSize = deco->iconSize > 0 ? deco->iconSize : parentWidget()->style()->pixelMetric(QStyle::PM_SmallIconSize, &opt, this);
        QIcon::Mode iconMode = QIcon::Normal;

        if (isDown()) {
            iconMode = QIcon::Selected;
        } else if (opt.state & QStyle::State_MouseOver) {
            iconMode = QIcon::Active;
        }
	QStyle::StandardPixmap sp;
	switch (type()) {
		case OnAllDesktopsButton: sp = QStyle::StandardPixmap(QStyle::SP_CustomBase + (deco->isOnAllDesktops() ? 2 : 1)); break;
		case MenuButton:	sp = QStyle::SP_TitleBarMenuButton; break;
		case AboveButton:	sp = QStyle::StandardPixmap(QStyle::SP_CustomBase + (deco->keepAbove() ? 6 : 4)); break;
		case BelowButton:	sp = QStyle::StandardPixmap(QStyle::SP_CustomBase + (deco->keepBelow() ? 7 : 5)); break;
		case HelpButton:	sp = QStyle::SP_TitleBarContextHelpButton; break;
		case MinButton:		sp = QStyle::SP_TitleBarMinButton; break;
		case MaxButton:	sp = deco->maximizeMode() == KDecoration::MaximizeFull ? QStyle::SP_TitleBarNormalButton : QStyle::SP_TitleBarMaxButton; break;
		case ShadeButton:	sp = deco->isShade() ? QStyle::SP_TitleBarUnshadeButton : QStyle::SP_TitleBarShadeButton; break;
		case CloseButton:	sp = QStyle::SP_TitleBarCloseButton; break;
		default:			sp = QStyle::SP_CustomBase; break;
	}

        opt.palette.setColor(QPalette::Text, deco->options()->color(KCommonDecoration::ColorFont, opt.state & QStyle::State_Active));
        const int shift = opt.state & QStyle::State_Selected ? 1 : 0;
        const bool useIcon = (sp == QStyle::SP_TitleBarMenuButton) && deco->useIcon();
        const QIcon icon = useIcon ? deco->icon().pixmap(iconSize, iconSize, iconMode) : parentWidget()->style()->standardIcon(sp, &opt, this);
	painter.setOpacity(opt.state & QStyle::State_Active ? 1.0 : 0.7);
	icon.paint(&painter, rect().adjusted(shift, shift, shift, shift), Qt::AlignCenter, iconMode);
}


/*-----------------------------------------------------------------------*/

QtMdiDecoration::QtMdiDecoration(KDecorationBridge *bridge, KDecorationFactory *factory, QStyle *s)
	: KCommonDecoration(bridge, factory), style(s)
{
        /* */
}


QtMdiDecoration::~QtMdiDecoration()
{
	/* */
}


QString QtMdiDecoration::visibleName() const
{
	return i18n("Sculpture");
}

QPixmap QtMdiDecoration::createShadowPixmap() const
{
    QImage image(65, 65, QImage::Format_ARGB32_Premultiplied);
    image.fill(0);

    QPainter p(&image);
    p.setRenderHints(QPainter::Antialiasing, true);
    p.setPen(Qt::NoPen);

    QRect rect(0, 0, 65, 65);
    for (int i = 0; i < 32; ++i) {
        p.setBrush(QColor(0, 0, 0, i / 3));
        p.drawRoundedRect(rect, 20, 20);
        rect.adjust(1, 1, -1, -1);
    }
    p.end();
    return QPixmap::fromImage(image);
}

void QtMdiDecoration::initStyleOption(QStyleOption &opt)
{
	opt.init(widget());
	opt.rect.adjust(layoutMetric(LM_OuterPaddingLeft, true), layoutMetric(LM_OuterPaddingTop, true),
		-layoutMetric(LM_OuterPaddingRight, true), -layoutMetric(LM_OuterPaddingBottom, true));
	opt.fontMetrics = QFontMetrics(options()->font(isActive()));
	if (isActive()) {
		opt.state |= QStyle::State_Active;
	} else {
		opt.state &= ~QStyle::State_Active;
	}
}


bool QtMdiDecoration::decorationBehaviour(DecorationBehaviour behaviour) const
{
	switch (behaviour) {
		case DB_MenuClose:
		case DB_WindowMask:
		case DB_ButtonHide:
			return true;
		default:
			return KCommonDecoration::decorationBehaviour(behaviour);
	}
}


void QtMdiDecoration:: updateWindowShape()
{
	QStyleOptionTitleBar option;
	initStyleOption(option);
	QStyleHintReturnMask mask;
	if ((widget()->style())->styleHint(QStyle::SH_WindowFrame_Mask, &option, widget(), &mask)) {
		setMask(mask.region);
	} else {
		setMask(QRegion(widget()->rect()));
	}
}

void QtMdiDecoration:: updateCaption()
{
    QStyleOption option;
    initStyleOption(option);

    bool border = !(maximizeMode() == MaximizeFull && !options()->moveResizeMaximizedWindows());
    border = borderWidth > 0 && border;

    int captionWidth = option.rect.width() - buttonsLeftWidth() - buttonsRightWidth() - (border ? 16 : 4);
    QRect labelRect = QRect((border ? 8 : 2) + buttonsLeftWidth(), 0, captionWidth, titleHeight + 4);
    labelRect.translate(option.rect.topLeft());
    labelRect.adjust(-4, 2, 4, -1);

    widget()->update(labelRect);
}

int QtMdiDecoration::layoutMetric(LayoutMetric lm, bool respectWindowState, const KCommonDecorationButton *button) const
{
    bool border = !(maximizeMode() == MaximizeFull && !options()->moveResizeMaximizedWindows());

    switch (lm) {
        case LM_BorderLeft:
        case LM_BorderRight:
            return border ? borderWidth : 0;
        case LM_BorderBottom:
            return border ? borderHeight : 0;
        case LM_TitleBorderLeft:
        case LM_TitleBorderRight:
            return borderWidth > 0 && border ? 4 : 0;
        case LM_TitleEdgeLeft:
        case LM_TitleEdgeRight:
            return borderWidth > 0 && border ? 8 : 0;
        case LM_TitleEdgeTop:
            return borderWidth > 0 && border ? 2 : 0;
        case LM_TitleEdgeBottom:
            return 1;
        case LM_TitleHeight:
            return titleHeight + 2;
        case LM_ButtonHeight:
            return borderWidth > 0 && border ? titleHeight : titleHeight + 3;
        case LM_ButtonWidth:
            return titleHeight;
        case LM_ButtonSpacing:
            return 0;
        case LM_ExplicitButtonSpacer:
            return 2;
        case LM_ButtonMarginTop:
            return 0;
        case LM_OuterPaddingLeft:
        case LM_OuterPaddingTop:
        case LM_OuterPaddingRight:
        case LM_OuterPaddingBottom:
            return renderShadows && maximizeMode() != MaximizeFull ? 32 : 0;
    }
    return KCommonDecoration::layoutMetric(lm, respectWindowState, button);
}


KCommonDecorationButton *QtMdiDecoration::createButton(ButtonType type)
{
	return new QtMdiDecorationButton(type, this);
}


void QtMdiDecoration::init()
{
	QWidget wid;
        KConfig configFile(QLatin1String("kwinskulpturerc"));
        KConfigGroup conf(&configFile, "General");
        QString styleName = conf.readEntry("QtStyle", QString());
        if (!styleName.isEmpty()) {
            style = loadStyle(styleName);
            if (!style) {
                style = QStyleFactory::create(styleName);
            }
        }
        if (style) {
        	wid.setStyle(style);
        }
	if (wid.style()) {
		QStyleOptionTitleBar option;
		option.init(&wid);
		option.fontMetrics = QFontMetrics(options()->font(true));
		borderWidth = wid.style()->pixelMetric(QStyle::PM_MdiSubWindowFrameWidth, &option, 0);
		titleHeight = wid.style()->pixelMetric(QStyle::PM_TitleBarHeight, &option, 0);
		noBorder = wid.style()->styleHint(QStyle::SH_TitleBar_NoBorder, &option, 0);
		autoRaise = wid.style()->styleHint(QStyle::SH_TitleBar_AutoRaise, &option, 0);
	} else {
		borderWidth = 4;
		titleHeight = 20;
		noBorder = false;
		autoRaise = false;
	}
	coloredFrame = conf.readEntry("UseTitleBarBorderColors", false);
	contrastFrame = conf.readEntry("UseExtraContrastBorder", false);
	renderShadows = conf.readEntry("RenderShadows", true) && KWindowSystem::compositingActive();
        showIcon = conf.readEntry("UseApplicationIcon", true);
        onlyActiveFrame = conf.readEntry("DrawOnlyActiveFrames", false);
        paintOnScreen = KWindowSystem::compositingActive() ? -1 : 0;
        if (conf.hasKey("PaintOnScreen")) {
            paintOnScreen = conf.readEntry("PaintOnScreen", false) ? 1 : 0;
        }
        textShift = conf.readEntry("TextShift", 0);
        textShiftMaximized = conf.readEntry("TextShiftMaximized", 0);
        QChar c = QChar('l', 0);
        QString align = conf.readEntry("TitleAlign", QString());
        if (!align.isEmpty()) {
            c = align.at(0).toLower();
        }
        switch (c.toAscii()) {
            case 'c':
                titleAlignment = Qt::AlignHCenter;
                break;
            case 'r':
                titleAlignment = Qt::AlignRight;
                break;
            default:
                titleAlignment = Qt::AlignLeft;
                break;
        }
        centerFullWidth = conf.readEntry("CenterFullWidth", true);
        iconSize = conf.readEntry("IconSize", -1);
        symbolSize = conf.readEntry("SymbolSize", 1.0);
        if (conf.readEntry("TitleHeight", -1) >= 0) {
            titleHeight = QFontMetrics(options()->font(true)).height() + conf.readEntry("TitleHeight", 8) - 4;
        }
        if (conf.readEntry("BorderSize", -1) >= 0) {
            borderWidth = conf.readEntry("BorderSize", borderWidth);
        }
        borderHeight = borderWidth;
        if (conf.readEntry("BottomBorderSize", -1) >= 0) {
            borderHeight = conf.readEntry("BottomBorderSize", borderWidth);
        }
	if (contrastFrame) {
		borderWidth += 1;
                borderHeight += 1;
	}
	KCommonDecoration::init();
	if (wid.style()) widget()->setStyle(wid.style());
	widget()->setAutoFillBackground(false);
	widget()->setAttribute(Qt::WA_NoSystemBackground, true);
	widget()->setAttribute(Qt::WA_OpaquePaintEvent, true);
        if (paintOnScreen >= 0) {
            widget()->setAttribute(Qt::WA_PaintOnScreen, paintOnScreen == 0 ? false : true);
        }
        if (renderShadows) {
            shadowPixmap = createShadowPixmap();
        } else {
            shadowPixmap = QPixmap();
        }
}


void QtMdiDecoration::paintEvent(QPaintEvent */*event */)
{
	bool border = !(maximizeMode() == MaximizeFull && !options()->moveResizeMaximizedWindows());
//	QWidget *w = widget();
	QStylePainter painter(widget());

	QStyleOptionTitleBar option;
	initStyleOption(option);

	// draw the shadow
	if (renderShadows && maximizeMode() != MaximizeFull) {
	    QRect rect = widget()->rect();
	    QRegion region = rect;
	    region -= option.rect;
	    painter.setClipRegion(region);
	    rect.adjust(8, 12, -8, -4);
	    qDrawBorderPixmap(&painter, rect, QMargins(32, 32, 32, 32), shadowPixmap);
	}

	// draw the title bar
	QRect rect = option.rect;
	painter.setClipRegion(rect);
	option.titleBarState = option.state;
	option.subControls = QStyle::SC_TitleBarLabel; // | QStyle::SC_TitleBarSysMenu;
	if (noBorder) {
		option.rect.setHeight(titleHeight);
	} else {
		option.rect.adjust(4, 4, -4, -4);
		option.rect.setHeight(titleHeight);
	}
        if (!border || borderWidth == 0) {
		option.rect.adjust(-6, 0, 6, 0);
	}
	option.text = QString();
#if 0
	option.icon = decoration()->icon();
	option.titleBarFlags = Qt::WindowSystemMenuHint;
#else
	option.titleBarFlags = 0;
#endif
//	option.palette = options()->palette(ColorTitleBar, isActive());
	if (coloredFrame && border && (!onlyActiveFrame || isActive())) {
		painter.fillRect(rect, options()->color(ColorTitleBar, isActive()));
	} else {
		painter.fillRect(rect, option.palette.color(QPalette::Window));
                if (borderHeight >= borderWidth + 2) {
                    QPalette palette;
                    palette.setCurrentColorGroup(QPalette::Disabled);
                    qDrawShadeLine(&painter, 1, rect.bottom() + qMax(2, borderWidth) - borderHeight, rect.right(), rect.bottom() + qMax(borderWidth, 2) - borderHeight, palette);
                }
	}
	QFont font = options()->font(isActive());
	painter.setFont(font);
	option.palette.setColor(QPalette::Highlight, options()->color(ColorTitleBar, true));
	option.palette.setColor(QPalette::HighlightedText, options()->color(ColorFont, true));
	option.palette.setColor(QPalette::Window, options()->color(ColorTitleBar, false));
	option.palette.setColor(QPalette::WindowText, options()->color(ColorFont, false));
//	option.rect = painter.style()->subControlRect(QStyle::CC_TitleBar, &option, QStyle::SC_TitleBarCloseButton, widget());
	painter.drawComplexControl(QStyle::CC_TitleBar, option);
	// draw the title caption
	// TODO use MDI code
	painter.save();
        border = borderWidth > 0 && border;
	int captionWidth = rect.width() - buttonsLeftWidth() - buttonsRightWidth() - (border ? 16 : 4);
	option.text = option.fontMetrics.elidedText(caption(), Qt::ElideMiddle, captionWidth);
	QRect labelRect = QRect((border ? 8 : 2) + buttonsLeftWidth(), 0, captionWidth, titleHeight + 4);
	labelRect.translate(rect.topLeft());
        Qt::Alignment alignment = titleAlignment;
        if (titleAlignment & Qt::AlignHCenter && centerFullWidth) {
            if (labelRect.left() > (rect.width() - option.fontMetrics.boundingRect(option.text).width()) >> 1) {
                alignment = Qt::AlignLeft;
            } else if (labelRect.right() < (rect.width() + option.fontMetrics.boundingRect(option.text).width()) >> 1) {
                alignment = Qt::AlignRight;
            } else {
                labelRect = rect;
                labelRect.setHeight(titleHeight + 4);
            }
        }
        if (border) {
            labelRect.adjust(0, -textShift, 0, -textShift);
        } else {
            labelRect.adjust(0, -textShiftMaximized, 0, -textShiftMaximized);
        }
	painter.setPen(QColor(0, 0, 0, 25));
	painter.drawText(labelRect.adjusted(1, 1, 1, 1), alignment | Qt::AlignVCenter | Qt::TextSingleLine, option.text);
        QColor color = options()->color(ColorFont, isActive());
	if (!(option.state & QStyle::State_Active)) {
             color.setAlpha((color.alpha() * 180) >> 8);
        }
	painter.setPen(color);
	painter.drawText(labelRect, alignment | Qt::AlignVCenter | Qt::TextSingleLine, option.text);
	painter.restore();

	// draw the frame
	if (border) {
		QStyleOptionFrame frameOptions;
		initStyleOption(frameOptions);
		if (coloredFrame) {
			frameOptions.palette.setColor(QPalette::Window, options()->color(ColorTitleBar, isActive()));
		}
		frameOptions.lineWidth = borderWidth;
		if (contrastFrame) {
			QBrush outerLineFill = QColor(170, 170, 170);
		//	QBrush outerLineFill = Qt::red;

			painter.fillRect(frameOptions.rect.adjusted(0, 0, 0, -frameOptions.rect.height() + 1), outerLineFill);
			painter.fillRect(frameOptions.rect.adjusted(0, frameOptions.rect.height() - 1, 0, 0), outerLineFill);
			painter.fillRect(frameOptions.rect.adjusted(0, 0, -frameOptions.rect.width() + 1, 0), outerLineFill);
			painter.fillRect(frameOptions.rect.adjusted(frameOptions.rect.width() - 1, 0, 0, 0), outerLineFill);
			frameOptions.rect.adjust(1, 1, -1, -1);
		}
                QRegion region = rect;
                region -= rect.adjusted(borderWidth, borderWidth, -borderWidth, -borderWidth);
                painter.setClipRegion(region);
		painter.drawPrimitive(QStyle::PE_FrameWindow, frameOptions);
	}
}


/*-----------------------------------------------------------------------*/

SkulptureDecorationFactory::SkulptureDecorationFactory()
{
    style = loadStyle(QLatin1String("skulpture"));
}


SkulptureDecorationFactory::~SkulptureDecorationFactory()
{
	/* */
}


KDecoration *SkulptureDecorationFactory::createDecoration(KDecorationBridge *bridge)
{
        return (new QtMdiDecoration(bridge, this, style))->decoration();
}


bool SkulptureDecorationFactory::reset(unsigned long changed)
{
	resetDecorations(changed);
	return true;
}


bool SkulptureDecorationFactory::supports(Ability ability) const
{
	switch (ability)
	{
		case AbilityAnnounceButtons:
		case AbilityButtonMenu:
		case AbilityButtonHelp:
		case AbilityButtonShade:
		case AbilityButtonMinimize:
		case AbilityButtonMaximize:
		case AbilityButtonClose:
		case AbilityButtonSpacer:
                        return true;
                // the following three are only supported with Skulpture Qt style
		case AbilityButtonOnAllDesktops:
		case AbilityButtonAboveOthers:
		case AbilityButtonBelowOthers:
			return true;
		case AbilityAnnounceColors:
		case AbilityColorTitleBack:
		case AbilityColorTitleFore:
			return true;
		case AbilityProvidesShadow:
		case AbilityUsesAlphaChannel:
			return true;
		default:
			return false;
	}
}


/*-----------------------------------------------------------------------*/

#include "kwin_skulpture.moc"


