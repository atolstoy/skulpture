/*
 * skulpture_color.cpp
 *
 */

#include "skulpture_p.h"
#include <QtWidgets/QApplication>


/*-----------------------------------------------------------------------*/

QColor shaded_color(const QColor &color, int shade)
{
#if 1
	const qreal contrast = 1.0;
	int r, g, b;
	color.getRgb(&r, &g, &b);
	int gray = qGray(r, g, b);
	gray = qMax(r, qMax(g, b));
	gray = (r + b + g + 3 * gray) / 6;
        if (color.alpha() == 0) {
            gray = 200;
        }
	if (shade < 0) {
		qreal k = 220.0 / 255.0 * shade;
		k *= contrast;
		int a = 255;
		if (gray > 0) {
			a = int(k * 255 / (0 - gray));
			if (a < 0) a = 0;
			if (a > 255) a = 255;
		}
		return QColor(0, 0, 0, a);
	} else {
		qreal k = (255 - 220.0) / (255.0) * shade;
		k *= contrast;
		int a = 255;
		if (gray < 255) {
			a = int(k * 255 / (255 - gray));
			if (a < 0) a = 0;
			if (a > 255) a = 255;
		}
		return QColor(255, 255, 255, a);
	}
#else
	if (shade < 0) {
		return QColor(0, 0, 0, -shade);
	} else {
		return QColor(255, 255, 255, shade);
	}
#endif
}


QColor blend_color(const QColor &c0, const QColor &c1, qreal blend)
{
#if 0 // more exact, but probably slower
	QColor c;

	blend = qMin(1.0, qMax(0.0, blend));
	c.setRgbF(
		c0.redF() * (1.0 - blend) + c1.redF() * blend,
		c0.greenF() * (1.0 - blend) + c1.greenF() * blend,
		c0.blueF() * (1.0 - blend) + c1.blueF() * blend,
		c0.alphaF() * (1.0 - blend) + c1.alphaF() * blend
	);
	return c;
#else
	int b = int(0.5 + 256.0 * blend);
	b = qMin(256, qMax(0, b));
	QRgb rgba0 = c0.rgba();
	QRgb rgba1 = c1.rgba();
	return QColor(
		qRed(rgba0) + (((qRed(rgba1) - qRed(rgba0)) * b) >> 8),
		qGreen(rgba0) + (((qGreen(rgba1) - qGreen(rgba0)) * b) >> 8),
		qBlue(rgba0) + (((qBlue(rgba1) - qBlue(rgba0)) * b) >> 8),
		qAlpha(rgba0) + (((qAlpha(rgba1) - qAlpha(rgba0)) * b) >> 8)
	);
#endif
}


/*-----------------------------------------------------------------------*/

enum ColorScheme {
    NormalColorScheme, // allow 3D effects
    DarkColorScheme, // too dark, no 3D effects
    BrightColorScheme // too bright, no 3D effects
};


ColorScheme guessColorScheme(const QPalette &palette, QPalette::ColorGroup colorGroup = QPalette::Active, QPalette::ColorRole colorRole = QPalette::Window)
{
    const QColor windowColor = palette.color(colorGroup, colorRole);
    int r, g, b;
    windowColor.getRgb(&r, &g, &b);
    int brightness = qGray(r, g, b);

    if (brightness > 230) {
        return BrightColorScheme;
    } else if (brightness < 40) {
        return DarkColorScheme;
    }
    return NormalColorScheme;
}


static void computeAlternateBase(QPalette &palette, QPalette::ColorGroup colorGroup)
{
    switch (guessColorScheme(palette, colorGroup, QPalette::Base)) {
        case DarkColorScheme:
            palette.setColor(colorGroup, QPalette::AlternateBase, palette.color(colorGroup, QPalette::Base).lighter(103));
            break;
        case BrightColorScheme:
        case NormalColorScheme:
            palette.setColor(colorGroup, QPalette::AlternateBase, palette.color(colorGroup, QPalette::Base).darker(103));
            break;
    }
}


static void copyColorGroup(QPalette &palette, QPalette::ColorGroup fromColorGroup, QPalette::ColorGroup toColorGroup)
{
    for (int role = int(QPalette::WindowText); role <= int(QPalette::LinkVisited); ++role) {
        QPalette::ColorRole colorRole = QPalette::ColorRole(role);
        palette.setColor(toColorGroup, colorRole, palette.color(fromColorGroup, colorRole));
    }
}


static void computeColorGroups(QPalette &palette, bool kdeMode = false, bool makeDisabledWidgetsTransparent = true)
{
    // ### Is this used by pre-Qt 4.5 for HLine / VLine ?
    palette.setColor(QPalette::Disabled, QPalette::Dark, shaded_color(palette.color(QPalette::Active, QPalette::Window), -20));
    palette.setColor(QPalette::Disabled, QPalette::Light, shaded_color(palette.color(QPalette::Active, QPalette::Window), 60));

    if (!kdeMode) {
        // compute remaining colors in Active group
        computeAlternateBase(palette, QPalette::Active);

        // copy Active group to Inactive group
        copyColorGroup(palette, QPalette::Active, QPalette::Inactive);

        // compute remaining colors in Inactive group
        computeAlternateBase(palette, QPalette::Inactive);
    }

    if (!kdeMode || makeDisabledWidgetsTransparent) {
        // create Disabled group
        QColor disabledBackgroundColor = palette.color(QPalette::Active, QPalette::Window);
        QColor disabledForegroundColor;
        switch (guessColorScheme(palette, QPalette::Active, QPalette::Window)) {
            case DarkColorScheme:
                disabledForegroundColor = palette.color(QPalette::Active, QPalette::Window).lighter(125);
                break;
            case BrightColorScheme:
            case NormalColorScheme:
                disabledForegroundColor = palette.color(QPalette::Active, QPalette::Window).darker(125);
                break;
        }

        palette.setColor(QPalette::Disabled, QPalette::Window, disabledBackgroundColor);
        palette.setColor(QPalette::Disabled, QPalette::WindowText, disabledForegroundColor);
        palette.setColor(QPalette::Disabled, QPalette::Base, disabledBackgroundColor);
        palette.setColor(QPalette::Disabled, QPalette::Text, disabledForegroundColor);
        palette.setColor(QPalette::Disabled, QPalette::Link, disabledForegroundColor);
        palette.setColor(QPalette::Disabled, QPalette::LinkVisited, disabledForegroundColor);
        palette.setColor(QPalette::Disabled, QPalette::Button, disabledBackgroundColor);
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, disabledForegroundColor);
        palette.setColor(QPalette::Disabled, QPalette::Highlight, disabledForegroundColor);
        palette.setColor(QPalette::Disabled, QPalette::HighlightedText, disabledBackgroundColor);

        computeAlternateBase(palette, QPalette::Disabled);
    }
}


QPalette SkulptureStyle::standardPalette() const
{
    QPalette palette(QColor(205, 205, 205));

    palette.setColor(QPalette::Active, QPalette::Base, QColor(229, 229, 229));
    palette.setColor(QPalette::Active, QPalette::Text, QColor(0, 0, 0));
    palette.setColor(QPalette::Active, QPalette::Link, QColor(80, 40, 120));
    palette.setColor(QPalette::Active, QPalette::LinkVisited, QColor(80, 50, 80));
    palette.setColor(QPalette::Active, QPalette::Highlight, QColor(114, 174, 211));
    palette.setColor(QPalette::Active, QPalette::HighlightedText, QColor(0, 0, 0));
    palette.setColor(QPalette::Active, QPalette::Window, QColor(200, 200, 200));
    palette.setColor(QPalette::Active, QPalette::WindowText, QColor(0, 0, 0));
//    palette.setColor(QPalette::Active, QPalette::Button, QColor(205, 205, 205));
    palette.setColor(QPalette::Active, QPalette::ButtonText, QColor(0, 0, 0));
    palette.setColor(QPalette::Active, QPalette::Shadow, QColor(0, 0, 0));
    palette.setColor(QPalette::Active, QPalette::BrightText, QColor(240, 240, 240));
    palette.setColor(QPalette::Inactive, QPalette::ToolTipBase, QColor(240, 230, 190));
    palette.setColor(QPalette::Inactive, QPalette::ToolTipText, QColor(0, 0, 0));

    computeColorGroups(palette);
    return palette;
}


void SkulptureStyle::polish(QPalette &palette)
{
    ParentStyle::polish(palette);
    computeColorGroups(palette, qApp->inherits("KApplication"), d->makeDisabledWidgetsTransparent);
}


