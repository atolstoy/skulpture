/*
 * skulpture_p.cpp
 *
 */

#include "skulpture_p.h"
#include <QtCore/QSettings>
#include <QtCore/QLocale>
#include <QtWidgets/QSlider>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QWizard>
#include <QtWidgets/QFormLayout>


/*-----------------------------------------------------------------------*/

struct StyleSetting
{
    enum Type {
        Bool,
        Char,
        Frame,
        Alignment,
        Orientation,
        Pixels,
        Points,
        Milliseconds,
        Color,
        Size,
        Parent,
        VarParent,
        Value
    };

    const char * const label;
    int id;
    int type;
    int value;
};


static const struct StyleSetting styleHintSettings[] =
{
    /* entries are stricly sorted in Qt order for future lookup table */
    { "General/EtchDisabledText", QStyle::SH_EtchDisabledText, StyleSetting::Bool, 1 },
///    { "General/DitherDisabledText", QStyle::SH_DitherDisabledText, StyleSetting::Bool, 0 },
    { "ScrollBar/MiddleClickAbsolutePosition", QStyle::SH_ScrollBar_MiddleClickAbsolutePosition, StyleSetting::Bool, 1 },
//    { "ScrollBar/ScrollWhenPointerLeavesControl", QStyle::SH_ScrollBar_ScrollWhenPointerLeavesControl, StyleSetting::Parent, 0 },
//    { "TabWidget/SelectMouseType", QStyle::SH_TabBar_SelectMouseType, StyleSetting::Parent, 0 },
///    { "TabWidget/TabBarAlignment", QStyle::SH_TabBar_Alignment, StyleSetting::Alignment, Qt::AlignCenter },
//    { "ItemView/HeaderArrowAlignment", QStyle::SH_Header_ArrowAlignment, StyleSetting::Parent, 0 },
    { "Slider/SnapToValue", QStyle::SH_Slider_SnapToValue, StyleSetting::Bool, 1 },
//    { "Slider/SloppyKeyEvents", QStyle::SH_Slider_SloppyKeyEvents, StyleSetting::Parent, 0 },
//    { "ProgressDialog/CenterCancelButton", QStyle::SH_ProgressDialog_CenterCancelButton, StyleSetting::Parent, 0 },
//    { "ProgressDialog/TextLabelAlignment", QStyle::SH_ProgressDialog_TextLabelAlignment, StyleSetting::Parent, 0 },
    { "PrintDialog/RightAlignButtons", QStyle::SH_PrintDialog_RightAlignButtons, StyleSetting::Bool, 1 },
//    { "Window/SpaceBelowMenuBar", QStyle::SH_MainWindow_SpaceBelowMenuBar, StyleSetting::Parent, 0 },
    { "FontDialog/SelectAssociatedText", QStyle::SH_FontDialog_SelectAssociatedText, StyleSetting::Bool, 1 },
    { "Menu/AllowActiveAndDisabled", QStyle::SH_Menu_AllowActiveAndDisabled, StyleSetting::Bool, 1 },
//    { "Menu/SpaceActivatesItem", QStyle::SH_Menu_SpaceActivatesItem, StyleSetting::Parent, 0 },
// ### dynamic { "Menu/SubMenuPopupDelay", QStyle::SH_Menu_SubMenuPopupDelay, StyleSetting::Milliseconds, 100 },
///    { "ItemView/FrameOnlyAroundContents", QStyle::SH_ScrollView_FrameOnlyAroundContents, StyleSetting::Bool, 0 },
    { "Menu/AltKeyNavigation", QStyle::SH_MenuBar_AltKeyNavigation, StyleSetting::Bool, 1 },
    { "ComboBox/ListMouseTracking", QStyle::SH_ComboBox_ListMouseTracking, StyleSetting::Bool, 1 },
    { "Menu/MouseTracking", QStyle::SH_Menu_MouseTracking, StyleSetting::Bool, 1 },
    { "Menu/BarMouseTracking", QStyle::SH_MenuBar_MouseTracking, StyleSetting::Bool, 1 },
//    { "ItemView/ChangeHighlightOnFocus", QStyle::SH_ItemView_ChangeHighlightOnFocus, StyleSetting::Parent, 0 },
//    { "Window/ShareActivation", QStyle::SH_Widget_ShareActivation, StyleSetting::Parent, 0 },
//    { "MDI/Workspace/FillSpaceOnMaximize", QStyle::SH_Workspace_FillSpaceOnMaximize, StyleSetting::Parent, 0 },
//    { "ComboBox/Popup", QStyle::SH_ComboBox_Popup, StyleSetting::Parent, 0 },
///    { "MDI/TitleBar/NoBorder", QStyle::SH_TitleBar_NoBorder, StyleSetting::Bool, 0 },
///    { "Slider/StopMouseOverSlider", QStyle::SH_Slider_StopMouseOverSlider, StyleSetting::Bool, 1 },
//    { "General/BlinkCursorWhenTextSelected", QStyle::SH_BlinkCursorWhenTextSelected, StyleSetting::Parent, 0 },
//    { "General/FullWidthSelection", QStyle::SH_RichText_FullWidthSelection, StyleSetting::Bool, 1 },
//    { "Menu/Scrollable", QStyle::SH_Menu_Scrollable, StyleSetting::Parent, 0 },
//    { "GroupBox/TextLabelVerticalAlignment", QStyle::SH_GroupBox_TextLabelVerticalAlignment, StyleSetting::Parent, 0 },
// ### dynamic { "GroupBox/TextLabelColor", QStyle::SH_GroupBox_TextLabelColor, StyleSetting::Color, 0xFF000000 },
//    { "Menu/SloppySubMenus", QStyle::SH_Menu_SloppySubMenus, StyleSetting::Parent, 0 },
// ### dynamic { "ItemView/GridLineColor", QStyle::SH_Table_GridLineColor, StyleSetting::Color, 0xFFD0D0D0 },
// ### dynamic { "LineEdit/PasswordCharacter", QStyle::SH_LineEdit_PasswordCharacter, StyleSetting::Char, 10039 },
//    { "Dialog/DefaultButton", QStyle::SH_DialogButtons_DefaultButton, StyleSetting::Parent, 0 },
//    { "ToolBox/SelectedPageTitleBold", QStyle::SH_ToolBox_SelectedPageTitleBold, StyleSetting::Bool, 1 },
//    { "TabWidget/TabBarPreferNoArrows", QStyle::SH_TabBar_PreferNoArrows, StyleSetting::Parent, 0 },
//    { "ScrollBar/LeftClickAbsolutePosition", QStyle::SH_ScrollBar_LeftClickAbsolutePosition, StyleSetting::Parent, 0 },
//    { "ItemView/Compat/ExpansionSelectMouseType", QStyle::SH_Q3ListViewExpand_SelectMouseType, StyleSetting::Parent, 0 },
// ### dynamic { "General/UnderlineShortcut", QStyle::SH_UnderlineShortcut, StyleSetting::Bool, 0 },
//    { "SpinBox/AnimateButton", QStyle::SH_SpinBox_AnimateButton, StyleSetting::Parent, 0 },
//    { "SpinBox/KeyPressAutoRepeatRate", QStyle::SH_SpinBox_KeyPressAutoRepeatRate, StyleSetting::Parent, 0 },
//    { "SpinBox/ClickAutoRepeatRate", QStyle::SH_SpinBox_ClickAutoRepeatRate, StyleSetting::Parent, 0 },
//    { "Menu/FillScreenWithScroll", QStyle::SH_Menu_FillScreenWithScroll, StyleSetting::Parent, 0 },
//    { "ToolTip/Opacity", QStyle::SH_ToolTipLabel_Opacity, StyleSetting::Parent, 0 },
//    { "Menu/DrawMenuBarSeparator", QStyle::SH_DrawMenuBarSeparator, StyleSetting::Parent, 0 },
//    { "MDI/TitleBar/ModifyNotification", QStyle::SH_TitleBar_ModifyNotification, StyleSetting::Parent, 0 },
//    { "Button/FocusPolicy", QStyle::SH_Button_FocusPolicy, StyleSetting::Parent, 0 },
//    { "Menu/DismissOnSecondClick", QStyle::SH_MenuBar_DismissOnSecondClick, StyleSetting::Parent, 0 },
//    { "MessageBox/UseBorderForButtonSpacing", QStyle::SH_MessageBox_UseBorderForButtonSpacing, StyleSetting::Parent, 0 },
    { "MDI/TitleBar/AutoRaise", QStyle::SH_TitleBar_AutoRaise, StyleSetting::Bool, 1 },
    { "ToolBar/PopupDelay", QStyle::SH_ToolButton_PopupDelay, StyleSetting::Milliseconds, 250 },
// ### dynamic { "General/FocusFrameMask", QStyle::SH_FocusFrame_Mask, StyleSetting::Parent, 0 },
// ### dynamic { "General/RubberBandMask", QStyle::SH_RubberBand_Mask, StyleSetting::Parent, 0 },
// ### dynamic { "General/WindowFrameMask", QStyle::SH_WindowFrame_Mask, StyleSetting::Parent, 0 },
//    { "SpinBox/DisableControlsOnBounds", QStyle::SH_SpinControls_DisableOnBounds, StyleSetting::Parent, 0 },
//    { "Dial/BackgroundRole", QStyle::SH_Dial_BackgroundRole, StyleSetting::Parent, 0 },
//    { "ComboBox/LayoutDirection", QStyle::SH_ComboBox_LayoutDirection, StyleSetting::Parent, 0 },
//    { "ItemView/EllipsisLocation", QStyle::SH_ItemView_EllipsisLocation, StyleSetting::Parent, 0 },
//    { "ItemView/ShowDecorationSelected", QStyle::SH_ItemView_ShowDecorationSelected, StyleSetting::Parent, 0 },
// ### from KDE { "ItemView/ActivateItemOnSingleClick", QStyle::SH_ItemView_ActivateItemOnSingleClick, StyleSetting::Bool, 1 },
//    { "ScrollBar/ContextMenu", QStyle::SH_ScrollBar_ContextMenu, StyleSetting::Parent, 0 },
//    { "ScrollBar/RollBetweenButtons", QStyle::SH_ScrollBar_RollBetweenButtons, StyleSetting::Parent, 0 },
//    { "Slider/AbsoluteSetButtons", QStyle::SH_Slider_AbsoluteSetButtons, StyleSetting::Parent, 0 },
//    { "Slider/PageSetButtons", QStyle::SH_Slider_PageSetButtons, StyleSetting::Parent, 0 },
//    { "Menu/KeyboardSearch", QStyle::SH_Menu_KeyboardSearch, StyleSetting::Parent, 0 },
//    { "TabWidget/ElideMode", QStyle::SH_TabBar_ElideMode, StyleSetting::Parent, 0 },
//    { "Dialog/ButtonLayout", QStyle::SH_DialogButtonLayout, StyleSetting::Value, QDialogButtonBox::KdeLayout },
//    { "ComboBox/PopupFrameStyle", QStyle::SH_ComboBox_PopupFrameStyle, StyleSetting::Parent, 0 },
    { "MessageBox/AllowTextInteraction", QStyle::SH_MessageBox_TextInteractionFlags, StyleSetting::Bool, 1 },
// ### from KDE { "Dialog/ButtonsHaveIcons", QStyle::SH_DialogButtonBox_ButtonsHaveIcons, StyleSetting::Bool, 0 },
//    { "General/SpellCheckUnderlineStyle", QStyle::SH_SpellCheckUnderlineStyle, StyleSetting::Parent, 0 },
    { "MessageBox/CenterButtons", QStyle::SH_MessageBox_CenterButtons, StyleSetting::Bool, 0 },
//    { "Menu/SelectionWrap", QStyle::SH_Menu_SelectionWrap, StyleSetting::Parent, 0 },
//    { "ItemView/MovementWithoutUpdatingSelection", QStyle::SH_ItemView_MovementWithoutUpdatingSelection, StyleSetting::Parent, 0 },
// ### dynamic { "General/ToolTipMask", QStyle::SH_ToolTip_Mask, StyleSetting::Parent, 0 },
//    { "General/FocusFrameAboveWidget", QStyle::SH_FocusFrame_AboveWidget, StyleSetting::Parent, 0 },
//    { "General/FocusIndicatorTextCharFormat", QStyle::SH_TextControl_FocusIndicatorTextCharFormat, StyleSetting::Parent, 0 },
//    { "Dialog/WizardStyle", QStyle::SH_WizardStyle, StyleSetting::Value, QWizard::ModernStyle },
    { "ItemView/ArrowKeysNavigateIntoChildren", QStyle::SH_ItemView_ArrowKeysNavigateIntoChildren, StyleSetting::Bool, 1 },
// ### dynamic { "General/MenuMask", QStyle::SH_Menu_Mask, StyleSetting::Parent, 0 },
//    { "Menu/FlashTriggeredItem", QStyle::SH_Menu_FlashTriggeredItem, StyleSetting::Parent, 0 },
//    { "Menu/FadeOutOnHide", QStyle::SH_Menu_FadeOutOnHide, StyleSetting::Parent, 0 },
//    { "SpinBox/ClickAutoRepeatThreshold", QStyle::SH_SpinBox_ClickAutoRepeatThreshold, StyleSetting::Parent, 0 },
//    { "ItemView/PaintAlternatingRowColorsForEmptyArea", QStyle::SH_ItemView_PaintAlternatingRowColorsForEmptyArea, StyleSetting::Parent, 0 },
//    { "FormLayout/WrapPolicy", QStyle::SH_FormLayoutWrapPolicy, StyleSetting::Value, QFormLayout::DontWrapRows },
//    { "TabWidget/DefaultTabPosition", QStyle::SH_TabWidget_DefaultTabPosition, StyleSetting::Parent, 0 },
//    { "ToolBar/Movable", QStyle::SH_ToolBar_Movable, StyleSetting::Parent, 0 },
    { "FormLayout/FieldGrowthPolicy", QStyle::SH_FormLayoutFieldGrowthPolicy, StyleSetting::Value, QFormLayout::ExpandingFieldsGrow },
//    { "FormLayout/FormAlignment", QStyle::SH_FormLayoutFormAlignment, StyleSetting::Alignment, Qt::AlignLeft | Qt::AlignTop },
    { "FormLayout/LabelAlignment", QStyle::SH_FormLayoutLabelAlignment, StyleSetting::Alignment, Qt::AlignRight | Qt::AlignTop },
//    { "ItemView/DrawDelegateFrame", QStyle::SH_, StyleSetting::Parent, 0 },
//    { "TabWidget/CloseButtonPosition", QStyle::SH_TabBar_CloseButtonPosition, StyleSetting::Parent, 0 },
//    { "DockWidget/ButtonsHaveFrame", QStyle::SH_DockWidget_ButtonsHaveFrame, StyleSetting::Parent, 0 },
    { 0, -1, 0, 0 }
};


extern int getRubberBandMask(QStyleHintReturnMask *mask, const QStyleOption *option, const QWidget *widget);
extern int getWindowFrameMask(QStyleHintReturnMask *mask, const QStyleOptionTitleBar *option, const QWidget *widget);

int SkulptureStyle::styleHint(StyleHint hint, const QStyleOption *option, const QWidget *widget, QStyleHintReturn *returnData) const
{
//	return ParentStyle::styleHint(hint, option, widget, returnData);
	// TODO implement caching
	const StyleSetting *setting = &styleHintSettings[0];
	QVariant value;

	switch (hint) {
            case QStyle::SH_Menu_SubMenuPopupDelay: {
                return d->subMenuDelay;
            }
            case QStyle::SH_TabBar_Alignment: {
                return d->centerTabs ? Qt::AlignHCenter : Qt::AlignLeft;
            }
            case QStyle::SH_Slider_StopMouseOverSlider: {
                // this works around a bug with Qt 4.4.2
                return qobject_cast<const QSlider *>(widget) != 0;
            }
            case QStyle::SH_ItemView_ActivateItemOnSingleClick: {
                if (d->useSingleClickToActivateItems != -1) {
                    return d->useSingleClickToActivateItems;
                }
                // use platform setting
                break;
            }
            case QStyle::SH_DialogButtonBox_ButtonsHaveIcons: {
                // ### use KDE setting
                return 0;
            }
#if 1
            case QStyle::SH_GroupBox_TextLabelColor: {
                QPalette palette;
                if (option) {
                    palette = option->palette;
                } else if (widget) {
                    palette = widget->palette();
                }
                return palette.color(QPalette::WindowText).rgba();
            }
#endif
            case QStyle::SH_Table_GridLineColor: {
                QPalette palette;
                if (option) {
                    palette = option->palette;
                } else if (widget) {
                    palette = widget->palette();
                }
                return palette.color(QPalette::Base).darker(120).rgba();
            }
            case QStyle::SH_LineEdit_PasswordCharacter: {
                QFontMetrics fm = option ? option->fontMetrics : (widget ? widget->fontMetrics() : QFontMetrics(QFont()));
                for (int i = 0; i < d->passwordCharacters.length(); ++i) {
                    if (fm.inFont(d->passwordCharacters.at(i))) {
                        return d->passwordCharacters.at(i).unicode();
                    }
                }
                return int('*');
            }
            case QStyle::SH_UnderlineShortcut: {
                if (d->hideShortcutUnderlines) {
                    return (d->shortcut_handler->underlineShortcut(widget));
                }
                return true;
            }
            case QStyle::SH_RubberBand_Mask: {
                QStyleHintReturnMask *mask = qstyleoption_cast<QStyleHintReturnMask *>(returnData);
                if (mask) {
                    return getRubberBandMask(mask, option, widget);
                }
                return 0;
            }
            case QStyle::SH_WindowFrame_Mask: {
                QStyleHintReturnMask *mask = qstyleoption_cast<QStyleHintReturnMask *>(returnData);
                const QStyleOptionTitleBar *titleBarOption = qstyleoption_cast<const QStyleOptionTitleBar *>(option);
                if (mask && titleBarOption) {
                    return getWindowFrameMask(mask, titleBarOption, widget);
                }
                return 0;
            }
            case QStyle::SH_Menu_SupportsSections: {
                return true;
            }
            default:
                break;
        }

	while (setting->label) {
		if (setting->id == int(hint)) {
			break;
		}
		++setting;
	}
	if (setting->label) {
		value = setting->value;
		switch (setting->type) {
			case StyleSetting::Parent:
				value = ParentStyle::styleHint(hint, option, widget, returnData);
				break;
			case StyleSetting::Bool:
				value = setting->value != 0;
				break;
			case StyleSetting::Char:
				value = QString(QChar(setting->value));
				break;
			case StyleSetting::Color:
				value = QChar('#', 0) + QString::number(value.toInt() - qRgba(0, 0, 0, 255), 16);
				break;
		}
	} else {
		value = ParentStyle::styleHint(hint, option, widget, returnData);
		setting = 0;
	}
#if 1
	if (setting && d->settings && setting->type != StyleSetting::Parent && !d->settings->contains(QLatin1String(setting->label))) {
		d->settings->setValue(QLatin1String(setting->label), value);
	}
#endif
	if (setting) {
		if (d->settings) {
			value = d->settings->value(QLatin1String(setting->label), value);
		}
		switch (setting->type) {
			case StyleSetting::Color:
				value = qRgba(0, 0, 0, 255);//FIXME + QLocale::c().toInt(value.toString().mid(1), 0, 16);
				break;
			case StyleSetting::Bool:
				value = value.toBool();
				break;
			case StyleSetting::Char:
				QString s = value.toString();
				if (s.size() == 1) {
					return s.at(0).unicode();
				}
				return setting->value;
		}
	}
	return value.toInt();
}


/*-----------------------------------------------------------------------*/
/*
 * Read color out of current settings group
 *
 * color - where to store the resulting color
 * s - Settings
 * colorName - name of color, such as "checkMark", first letter should be not be capitalized
 * n - for multiple color entries, returns the "n"th color (n = 1, 2, ...) otherwise use n = 0.
 *
 * This will do the following:
 *
 * 1. check if "custom<ColorName>Color<n>" is set to "false" -> return false with "color" unmodified
 * 2. read color entry from "<colorName>Color<n>"
 * 3. read color opacity from "<colorName>Color<n>Opacity"
 * 4. return true with "color" modified accordingly
 *
 */

static bool readSettingsColor(QColor &color, const QSettings &s, const QString &colorName, int n = 0)
{
    QString cName = colorName + QLatin1String("Color");
    if (n > 0) {
        cName += QString::number(n);
    }
    if (s.value(QLatin1String("custom") + cName.at(0).toUpper() + cName.mid(1), true).toBool() == false) {
        return false;
    }
    QString val = s.value(cName).toString();
    if (!val.isEmpty()) {
        QColor c = QColor(val);
        if (c.isValid()) {
            color = c;
            int opacity = s.value(cName + QLatin1String("Opacity"), -1).toInt();
            if (opacity >= 0 && opacity <= 255) {
                color.setAlpha(opacity);
            }
            return true;
        }
    }
    return false;
}


/*-----------------------------------------------------------------------*/
/*
 * Read gradient out of current settings group
 *
 * gradient - where to store the resulting gradient (only the color stops are modified)
 * s - Settings
 * gradientName - name of gradient, such as "activeTabTop", first letter should be not be capitalized
 *
 * This returns true with "gradient" colors modified accordingly, or false, if no color was found.
 *
 * Limitations:
 *
 * Maximum number of gradients per background: 9
 * Maximum number of colors per gradient: 2
 *
 */

#define MIN_STOP 0      // must be 0
#define MAX_STOP 100

static inline qreal topToStop(int v)
{
    if (v <= MIN_STOP) {
        return qreal(0.00000);
    } else if (v >= MAX_STOP) {
        return qreal(0.99999);
    }
    return v / qreal(MAX_STOP) - qreal(0.00001);
}


static inline qreal bottomToStop(int v)
{
    if (v <= 0) {
        return qreal(0.00001);
    } else if (v >= MAX_STOP) {
        return qreal(1.00000);
    }
    return v / qreal(MAX_STOP) + qreal(0.00001);
}


static bool readSettingsGradient(QGradient &gradient, const QSettings &s, const QString &gradientName)
{
    QColor background(s.value(gradientName + QLatin1String("background")).toString());
    if (!background.isValid()) {
        return false;
    }

    bool hasTop = false;
    bool hasBottom = false;
    int numGradients = s.value(gradientName + QLatin1String("numGradients"), 0).toInt();
    numGradients = qMin(numGradients, 9); // single digit limit
    for (int i = 1; i <= numGradients; ++i) {
        QString gradientPrefix = gradientName + QChar('g', 0) + QChar('0' + i, 0);

        int top = s.value(gradientPrefix + QLatin1String("Top"), -1).toInt();
        int bottom = s.value(gradientPrefix + QLatin1String("Bottom"), -1).toInt();
        if (top == MIN_STOP) {
            hasTop = true;
        }
        if (bottom == MAX_STOP) {
            hasBottom = true;
        }
        if (top >= 0 && bottom >= 0) {
            QColor c[9 + 1];
            int k = 0;
            for (int n = 1; n <= 9; ++n) { // single digit limit
                if (readSettingsColor(c[n], s, gradientPrefix, n)) {
                    ++k;
                } else {
                    // force continous color numbering
                    break;
                }
            }
            // k colors are found, spread lineary between top ... bottom
            if (k > 1) {
                // FIXME blindly assumes k == 2 for now
                gradient.setColorAt(topToStop(top), c[1]);
                gradient.setColorAt(bottomToStop(bottom), c[2]);
            } else if (k == 1) {
                gradient.setColorAt(topToStop(top), c[1]);
                gradient.setColorAt(bottomToStop(bottom), c[1]);
            }
        }
    }
    // FIXME does not detect "holes" between gradients
    // that are to be "filled" with background
    if (!hasTop) {
        gradient.setColorAt(0, background);
    }
    if (!hasBottom) {
        gradient.setColorAt(1, background);
    }
    return true;
}


/*-----------------------------------------------------------------------*/
/*
 * Read domino settings out of current settings group
 *
 */

void SkulptureStyle::Private::readDominoSettings(const QSettings &s)
{
    static const char * const gradientName[] = {
        "tabTop", "tabBottom", "activeTabTop", "activeTabBottom",
        "btn", "checkItem", "header", "scrollBar", "scrollBarGroove"
    };
    static const char * const colorName[] = {
        "checkMark", "groupBoxBackground", "popupMenu",
        "selMenuItem", "toolTip"
    };

    for (uint i = 0; i < array_elements(gradientName); ++i) {
        QGradient gradient;
        if (readSettingsGradient(gradient, s, QLatin1String(gradientName[i]) + QLatin1String("Surface_"))) {
#if 0
            printf("domino: gradient[%s]=", gradientName[i]);
            QGradientStops stops = gradient.stops();
            for (int i = 0; i < stops.size(); ++i) {
                QGradientStop stop = stops.at(i);
                QColor color = stop.second;
                printf("(%.6g=#%2x%2x%2x/a=#%2x)", stop.first, color.red(), color.green(), color.blue(), color.alpha());
            }
            printf("\n");
#endif
            // only save stops from the gradient
//            dominoGradientStops[i] = gradient.stops();
        }
    }
    for (uint i = 0; i < array_elements(colorName); ++i) {
        QColor color;
        if (readSettingsColor(color, s, QLatin1String(colorName[i]))) {
#if 0
            printf("domino: color[%s]=#%2x%2x%2x/a=#%2x\n", colorName[i], color.red(), color.green(), color.blue(), color.alpha());
#endif
//            dominoCustomColors[i] = color;
        }
    }
    animateProgressBars = s.value(QLatin1String("animateProgressBar"), animateProgressBars).toBool();
    centerTabs = s.value(QLatin1String("centerTabs"), centerTabs).toBool();
#if 0
    readSettingsColor(buttonContourColor, s, QLatin1String("buttonContour"));
    readSettingsColor(buttonDefaultContourColor, s, QLatin1String("buttonDefaultContour"));
    readSettingsColor(buttonMouseOverContourColor, s, QLatin1String("buttonMouseOverContour"));
    readSettingsColor(buttonPressedContourColor, s, QLatin1String("buttonPressedContour"));
    readSettingsColor(indicatorButtonColor, s, QLatin1String("indicatorButton"));
    readSettingsColor(indicatorColor, s, QLatin1String("indicator"));
    readSettingsColor(rubberBandColor, s, QLatin1String("rubberBand"));
    readSettingsColor(textEffectButtonColor, s, QLatin1String("textEffectButton"));
    readSettingsColor(textEffectColor, s, QLatin1String("textEffect"));

    highlightToolBtnIcon = s.value(QLatin1String("highlightToolBtnIcon"), highlightToolBtnIcon).toBool();
    indentPopupMenuItems = s.value(QLatin1String("indentPopupMenuItems"), indentPopupMenuItems).toBool();
    smoothScrolling = s.value(QLatin1String("smoothScrolling"), smoothScrolling).toBool();
    tintGroupBoxBackground = s.value(QLatin1String("tintGroupBoxBackground"), tintGroupBoxBackground).toBool();
    indicateFocus = s.value(QLatin1String("indicateFocus"), indicateFocus).toBool();

    drawButtonSunkenShadow = s.value(QLatin1String("drawButtonSunkenShadow"), drawButtonSunkenShadow).toBool();
    drawFocusUnderline = s.value(QLatin1String("drawFocusUnderline"), drawFocusUnderline).toBool();
    drawPopupMenuGradient = s.value(QLatin1String("drawPopupMenuGradient"), drawPopupMenuGradient).toBool();
    drawTextEffect = s.value(QLatin1String("drawTextEffect"), drawTextEffect).toBool();
    drawToolButtonAsButton = s.value(QLatin1String("drawToolButtonAsButton"), drawToolButtonAsButton).toBool();
    drawTriangularExpander = s.value(QLatin1String("drawTriangularExpander"), drawTriangularExpander).toBool();
#endif
}


/*-----------------------------------------------------------------------*/

void SkulptureStyle::Private::readSettings(const QSettings &s)
{
    // defaults
    animateProgressBars = true;
    verticalArrowMode = SkulptureMode;
    horizontalArrowMode = SkulptureMode;
    hideShortcutUnderlines = true;
    centerTabs = false;
    makeDisabledWidgetsTransparent = true;
    transparentPlacesPanel = false;
    forceSpacingAndMargins = false;
    visualizeLayouts = false;
    useIconColumnForCheckIndicators = false;
    useSelectionColorForCheckedIndicators = false;
    useSelectionColorForSelectedMenuItems = false;
    useSingleClickToActivateItems = -1;
    expensiveShadows = true;
    allowTabulations = false;

    dialogMargins = -1;
    horizontalSpacing = -1;
    labelSpacing = -1;
    menuBarSize = -1;
    menuItemSize = -1;
    pushButtonSize = -1;
    scrollBarSize = -1;
    scrollBarLength = -1;
    sliderSize = -1;
    sliderLength = -1;
    tabBarSize = -1;
    toolButtonSize = -1;
    verticalSpacing = -1;
    widgetMargins = -1;
    widgetSize = -1;
    textShift = 0;

    buttonGradient = 0;
    buttonRoundness = 0;

    passwordCharacters = QString(QChar(ushort(10039)));
    textCursorWidth = 0;

    subMenuDelay = 100;

    // legacy settings import: domino 0.4
    QString dominoConfigFile = s.value(QLatin1String("LegacyImport/DominoConfiguration")).toString();
    if (!dominoConfigFile.isEmpty()) {
        QSettings s(dominoConfigFile, QSettings::IniFormat);
        s.beginGroup(QLatin1String("Settings"));
        readDominoSettings(s);
        s.endGroup();
    }

    // native settings
    animateProgressBars = s.value(QLatin1String("ProgressBar/AnimateProgressBars"), animateProgressBars).toBool();
    if (s.contains(QLatin1String("ScrollBar/AllowScrollBarSliderToCoverArrows"))
        && !s.contains(QLatin1String("ScrollBar/VerticalArrowMode"))) {
        verticalArrowMode = s.value(QLatin1String("ScrollBar/AllowScrollBarSliderToCoverArrows"), true).toBool() ? SkulptureMode : WindowsMode;
    } else {
        QString mode = s.value(QLatin1String("ScrollBar/VerticalArrowMode"), QLatin1String("Covered")).toString();
        if (mode == QLatin1String("Top")) {
            verticalArrowMode = NextMode;
        } else if (mode == QLatin1String("Bottom")) {
            verticalArrowMode = PlatinumMode;
        } else if (mode == QLatin1String("BottomTop") || mode == QLatin1String("Bottom/Top")) {
            verticalArrowMode = WindowsMode;
        } else if (mode == QLatin1String("KDEMode")) {
            verticalArrowMode = KDEMode;
        } else if (mode == QLatin1String("Covered")) {
            verticalArrowMode = SkulptureMode;
        } else if (mode == QLatin1String("None")) {
            verticalArrowMode = NoArrowsMode;
        } else {
            verticalArrowMode = SkulptureMode;
        }
    }
    if (s.contains(QLatin1String("ScrollBar/AllowScrollBarSliderToCoverArrows"))
        && !s.contains(QLatin1String("ScrollBar/HorizontalArrowMode"))) {
        horizontalArrowMode = s.value(QLatin1String("ScrollBar/AllowScrollBarSliderToCoverArrows"), true).toBool() ? SkulptureMode : WindowsMode;
    } else {
        QString mode = s.value(QLatin1String("ScrollBar/HorizontalArrowMode"), QLatin1String("Covered")).toString();
        if (mode == QLatin1String("Left")) {
            horizontalArrowMode = NextMode;
        } else if (mode == QLatin1String("Right")) {
            horizontalArrowMode = PlatinumMode;
        } else if (mode == QLatin1String("RightLeft") || mode == QLatin1String("Right/Left")) {
            horizontalArrowMode = WindowsMode;
        } else if (mode == QLatin1String("KDEMode")) {
            horizontalArrowMode = KDEMode;
        } else if (mode == QLatin1String("Covered")) {
            horizontalArrowMode = SkulptureMode;
        } else if (mode == QLatin1String("None")) {
            horizontalArrowMode = NoArrowsMode;
        } else {
            horizontalArrowMode = SkulptureMode;
        }
    }
    hideShortcutUnderlines = s.value(QLatin1String("General/HideShortcutUnderlines"), hideShortcutUnderlines).toBool();
    makeDisabledWidgetsTransparent = s.value(QLatin1String("General/MakeDisabledWidgetsTransparent"), makeDisabledWidgetsTransparent).toBool();
    transparentPlacesPanel = s.value(QLatin1String("Views/TransparentPlacesPanel"), transparentPlacesPanel).toBool();
    forceSpacingAndMargins = s.value(QLatin1String("Layout/ForceSpacingAndMargins"), forceSpacingAndMargins).toBool();
    visualizeLayouts = s.value(QLatin1String("Layout/VisualizeLayouts"), visualizeLayouts).toBool();
    useIconColumnForCheckIndicators = s.value(QLatin1String("Menus/UseIconColumnForCheckIndicators"), useIconColumnForCheckIndicators).toBool();
    useSelectionColorForCheckedIndicators = s.value(QLatin1String("General/UseSelectionColorForCheckedIndicators"), useSelectionColorForCheckedIndicators).toBool();
    useSelectionColorForSelectedMenuItems = s.value(QLatin1String("Menus/UseSelectionColorForSelectedMenuItems"), useSelectionColorForSelectedMenuItems).toBool();
    if (s.contains(QLatin1String("General/UseSingleClickToActivateItems"))) {
        bool singleClick = s.value(QLatin1String("General/UseSingleClickToActivateItems"), true).toBool();
        useSingleClickToActivateItems = singleClick ? 1 : 0;
    }
    expensiveShadows = s.value(QLatin1String("General/ExpensiveShadows"), expensiveShadows).toBool();
    allowTabulations = s.value(QLatin1String("General/AllowTabulations"), allowTabulations).toBool();

    dialogMargins = s.value(QLatin1String("Layout/DialogMargins"), dialogMargins).toInt();
    horizontalSpacing = s.value(QLatin1String("Layout/HorizontalSpacing"), horizontalSpacing).toInt();
    labelSpacing = s.value(QLatin1String("Layout/LabelSpacing"), labelSpacing).toInt();
    menuBarSize = s.value(QLatin1String("Layout/MenuBarSize"), menuBarSize).toInt();
    menuItemSize = s.value(QLatin1String("Layout/MenuItemSize"), menuItemSize).toInt();
    pushButtonSize = s.value(QLatin1String("Layout/PushButtonSize"), pushButtonSize).toInt();
    scrollBarSize = s.value(QLatin1String("Layout/ScrollBarSize"), scrollBarSize).toInt();
    scrollBarLength = s.value(QLatin1String("Layout/ScrollBarLength"), scrollBarLength).toInt();
    sliderSize = s.value(QLatin1String("Layout/SliderSize"), sliderSize).toInt();
    sliderLength = s.value(QLatin1String("Layout/SliderLength"), sliderLength).toInt();
    tabBarSize = s.value(QLatin1String("Layout/TabBarSize"), tabBarSize).toInt();
    toolButtonSize = s.value(QLatin1String("Layout/ToolButtonSize"), toolButtonSize).toInt();
    verticalSpacing = s.value(QLatin1String("Layout/VerticalSpacing"), verticalSpacing).toInt();
    widgetMargins = s.value(QLatin1String("Layout/WidgetMargins"), widgetMargins).toInt();
    widgetSize = s.value(QLatin1String("Layout/WidgetSize"), widgetSize).toInt();
    textShift = s.value(QLatin1String("General/TextShift"), textShift).toInt();

    buttonGradient = s.value(QLatin1String("General/ButtonGradientIntensity"), buttonGradient).toInt();
    buttonRoundness = s.value(QLatin1String("General/ButtonRoundness"), buttonRoundness).toInt();

    passwordCharacters = s.value(QLatin1String("General/PasswordCharacters"), passwordCharacters).toString();
    styleSheetFileName = s.value(QLatin1String("General/StyleSheetFileName"), QString()).toString();
    textCursorWidth = s.value(QLatin1String("General/TextCursorWidth"), (double) textCursorWidth).toDouble();

    subMenuDelay = s.value(QLatin1String("Menus/SubMenuDelay"), subMenuDelay).toInt();

    // apply defaults
    if (widgetSize < 0) {
        widgetSize = 2;
    }
    if (pushButtonSize < 0) {
        pushButtonSize = 2;
    }
    if (tabBarSize < 0) {
        tabBarSize = 2;
    }
    if (menuItemSize < 0) {
        menuItemSize = 2;
    }
}


int SkulptureStyle::skulpturePrivateMethod(SkulptureStyle::SkulpturePrivateMethod id, void *data)
{
    switch (id) {
        case SPM_SupportedMethods: {
            return SPM_SetSettingsFileName;
        }
        case SPM_SetSettingsFileName: {
            SkMethodDataSetSettingsFileName *md = (SkMethodDataSetSettingsFileName *) data;
            if (md && md->version >= 1) {
                QSettings s(md->fileName, QSettings::IniFormat);
                if (s.status() == QSettings::NoError) {
                    d->readSettings(s);
                    return 1;
                }
            }
            return 0;
        }
        default:
            return 0;
    }
}


/*-----------------------------------------------------------------------*/

void paintNothing(QPainter */*painter*/, const QStyleOption */*option*/)
{
	//
}


void paintDefault(QPainter */*painter*/, const QStyleOption */*option*/)
{
	//
}


/*-----------------------------------------------------------------------*/

void paintCommandButtonPanel(QPainter *painter, const QStyleOptionButton *option, const QWidget *widget);
void paintPushButtonBevel(QPainter *painter, const QStyleOptionButton *option, const QWidget *widget, const QStyle *style);
void paintTabWidgetFrame(QPainter *painter, const QStyleOptionTabWidgetFrame *option, const QWidget *widget);
void paintIndicatorCheckBox(QPainter *painter, const QStyleOptionButton *option);
void paintIndicatorItemViewItemCheck(QPainter *painter, const QStyleOption *option);
#ifdef QT3_SUPPORT
void paintQ3CheckListIndicator(QPainter *painter, const QStyleOptionQ3ListView *option, const QWidget *widget, const QStyle *style);
void paintQ3CheckListExclusiveIndicator(QPainter *painter, const QStyleOptionQ3ListView *option, const QWidget *widget, const QStyle *style);
#endif
void paintIndicatorRadioButton(QPainter *painter, const QStyleOptionButton *option);
void paintIndicatorSpinDown(QPainter *painter, const QStyleOption *option);
void paintIndicatorSpinUp(QPainter *painter, const QStyleOption *option);
void paintIndicatorSpinMinus(QPainter *painter, const QStyleOption *option);
void paintIndicatorSpinPlus(QPainter *painter, const QStyleOption *option);
void paintIndicatorArrowDown(QPainter *painter, const QStyleOption *option);
void paintIndicatorArrowLeft(QPainter *painter, const QStyleOption *option);
void paintIndicatorArrowRight(QPainter *painter, const QStyleOption *option);
void paintIndicatorArrowUp(QPainter *painter, const QStyleOption *option);
void paintHeaderSortIndicator(QPainter *painter, const QStyleOptionHeader *option);
void paintStyledFrame(QPainter *painter, const QStyleOptionFrame *frame, const QWidget *widget, const QStyle *style);
void paintFrameLineEdit(QPainter *painter, const QStyleOptionFrame *frame);
void paintPanelLineEdit(QPainter *painter, const QStyleOptionFrame *frame, const QWidget *widget, const QStyle *style);
void paintFrameDockWidget(QPainter *painter, const QStyleOptionFrame *frame);
void paintFrameWindow(QPainter *painter, const QStyleOptionFrame *frame);
void paintToolBarSeparator(QPainter *painter, const QStyleOptionToolBar *option);
void paintToolBarHandle(QPainter *painter, const QStyleOptionToolBar *option);
void paintScrollArea(QPainter *painter, const QStyleOption *option);
void paintPanelToolBar(QPainter *painter, const QStyleOptionToolBar *option);
void paintIndicatorMenuCheckMark(QPainter *painter, const QStyleOptionMenuItem *option, const QWidget *widget, const QStyle *style);
void paintFrameGroupBox(QPainter *painter, const QStyleOptionFrame *option);
void paintFrameFocusRect(QPainter *painter, const QStyleOptionFocusRect *option, const QWidget *widget);
void paintPanelButtonTool(QPainter *painter, const QStyleOption *option, const QWidget *widget, const QStyle *style);
void paintSizeGrip(QPainter *painter, const QStyleOption *option);
void paintScrollAreaCorner(QPainter *painter, const QStyleOption *option, const QWidget *widget, const QStyle *style);
void paintPanelItemViewItem(QPainter *painter, const QStyleOptionViewItem *option, const QWidget *widget, const QStyle *style);
void paintIndicatorTabClose(QPainter *painter, const QStyleOption *option, const QWidget *widget, const QStyle *style);

void paintMenuBarEmptyArea(QPainter *painter, const QStyleOption *option);
void paintPanelMenuBar(QPainter *painter, const QStyleOptionFrame *frame);
void paintMenuBarItem(QPainter *painter, const QStyleOptionMenuItem *option, const QWidget *widget, const QStyle *style);
void paintFrameMenu(QPainter *painter, const QStyleOptionFrame *frame);
void paintMenuItem(QPainter *painter, const QStyleOptionMenuItem *option, const QWidget *widget, const QStyle *style);

void paintTabBarTabShape(QPainter *painter, const QStyleOptionTab *option, const QWidget *widget, const QStyle *style);
void paintTabBarTabLabel(QPainter *painter, const QStyleOptionTab *option, const QWidget *widget, const QStyle *style);
void paintFrameTabBarBase(QPainter *painter, const QStyleOptionTabBarBase *option, const QWidget *widget);
void paintToolBoxTabShape(QPainter *painter, const QStyleOptionToolBox *option);
void paintToolBoxTabLabel(QPainter *painter, const QStyleOptionToolBox *option, const QWidget *widget, const QStyle *style);
void paintHeaderEmptyArea(QPainter *painter, const QStyleOption *option);
void paintHeaderSection(QPainter *painter, const QStyleOptionHeader *option, const QWidget *widget, const QStyle *style);
void paintHeaderLabel(QPainter *painter, const QStyleOptionHeader *option, const QWidget *widget, const QStyle *style);
void paintIndicatorBranch(QPainter *painter, const QStyleOption *option);

void paintScrollBarSlider(QPainter *painter, const QStyleOptionSlider *option);
void paintScrollBarAddLine(QPainter *painter, const QStyleOptionSlider *option);
void paintScrollBarSubLine(QPainter *painter, const QStyleOptionSlider *option);
void paintScrollBarFirst(QPainter *painter, const QStyleOptionSlider *option);
void paintScrollBarLast(QPainter *painter, const QStyleOptionSlider *option);
void paintScrollBarPage(QPainter *painter, const QStyleOptionSlider *option);
void paintProgressBarGroove(QPainter *painter, const QStyleOptionProgressBar *option);
void paintProgressBarContents(QPainter *painter, const QStyleOptionProgressBar *option, const QWidget *widget, const QStyle *style);
void paintProgressBarLabel(QPainter *painter, const QStyleOptionProgressBar *option, const QWidget *widget, const QStyle *style);
void paintSplitter(QPainter *painter, const QStyleOption *option);
void paintDockWidgetTitle(QPainter *painter, const QStyleOptionDockWidget *option, const QWidget *widget, const QStyle *style);
void paintRubberBand(QPainter *paint, const QStyleOptionRubberBand *option);
void paintComboBoxLabel(QPainter *painter, const QStyleOptionComboBox *option, const QWidget *widget, const QStyle *style);
void paintToolButtonLabel(QPainter *painter, const QStyleOptionToolButton *option, const QWidget *widget, const QStyle *style);


/*-----------------------------------------------------------------------*/

SkulptureStyle::Private::Private()
{
	init();
}


SkulptureStyle::Private::~Private()
{
	delete shortcut_handler;
	delete settings;
}


void SkulptureStyle::Private::init()
{
	shortcut_handler = new ShortcutHandler(this);
	timer = 0;
	updatingShadows = false;
        oldEdit = 0;
#if 0
	settings = new QSettings(QSettings::IniFormat,
		QSettings::UserScope,
		QLatin1String("SkulptureStyle"),
		QLatin1String(""));
#else
	settings = 0;
#endif

	QSettings s(QSettings::IniFormat, QSettings::UserScope, QLatin1String("SkulptureStyle"), QLatin1String(""));
	readSettings(s);

	register_draw_entries();
}


void SkulptureStyle::Private::register_draw_entries()
{
	for (uint i = 0; i < sizeof(draw_primitive_entry) / sizeof(Private::DrawElementEntry); ++i) {
		draw_primitive_entry[i].func = 0;
	}
	for (uint i = 0; i < sizeof(draw_element_entry) / sizeof(Private::DrawElementEntry); ++i) {
		draw_element_entry[i].func = 0;
	}

        /* entries are stricly sorted in Qt order for future lookup table */

#define register_primitive(pe, f, so) draw_primitive_entry[QStyle::PE_ ## pe].type = QStyleOption::SO_ ## so; draw_primitive_entry[QStyle::PE_ ## pe].func = (drawElementFunc *) paint ## f;

        /* PRIMITIVE ELEMENT */
#ifdef QT3_SUPPORT
// Qt 3.x compatibility
//###	register_primitive(Q3CheckListController, Nothing, Default);
	register_primitive(Q3CheckListExclusiveIndicator, Q3CheckListExclusiveIndicator, Default);
	register_primitive(Q3CheckListIndicator, Q3CheckListIndicator, Default);
	register_primitive(Q3DockWindowSeparator, ToolBarSeparator, Default);
//###	register_primitive(Q3Separator, Q3Separator, Default);
#endif
// Qt 4.0 Frames
	register_primitive(Frame, StyledFrame, Frame);
	register_primitive(FrameDefaultButton, Nothing, Button);
	register_primitive(FrameDockWidget, FrameDockWidget, Frame);
	register_primitive(FrameFocusRect, FrameFocusRect, FocusRect);
	register_primitive(FrameGroupBox, FrameGroupBox, Frame);
	register_primitive(FrameLineEdit, FrameLineEdit, Frame);
	register_primitive(FrameMenu, FrameMenu, Default); // ### Qt 4.3 calls FrameMenu with SO_ToolBar for a toolbar
	register_primitive(FrameStatusBarItem, Nothing, Default);
	register_primitive(FrameTabWidget, TabWidgetFrame, TabWidgetFrame);
	register_primitive(FrameWindow, FrameWindow, Frame);
	register_primitive(FrameButtonBevel, PanelButtonTool, Default);
	register_primitive(FrameButtonTool, PanelButtonTool, Default);
	register_primitive(FrameTabBarBase, FrameTabBarBase, TabBarBase);
// Qt 4.0 Panels
	register_primitive(PanelButtonCommand, CommandButtonPanel, Button);
	register_primitive(PanelButtonBevel, PanelButtonTool, Default);
	register_primitive(PanelButtonTool, PanelButtonTool, Default);
	register_primitive(PanelMenuBar, PanelMenuBar, Frame);
	register_primitive(PanelToolBar, PanelToolBar, Frame);
	register_primitive(PanelLineEdit, PanelLineEdit, Frame);
// Qt 4.0 Indicators
	register_primitive(IndicatorArrowDown, IndicatorArrowDown, Default);
	register_primitive(IndicatorArrowLeft, IndicatorArrowLeft, Default);
	register_primitive(IndicatorArrowRight, IndicatorArrowRight, Default);
	register_primitive(IndicatorArrowUp, IndicatorArrowUp, Default);
	register_primitive(IndicatorBranch, IndicatorBranch, Default);
	register_primitive(IndicatorButtonDropDown, PanelButtonTool, Default);
	register_primitive(IndicatorItemViewItemCheck, IndicatorItemViewItemCheck, Default);
	register_primitive(IndicatorCheckBox, IndicatorCheckBox, Button);
	register_primitive(IndicatorDockWidgetResizeHandle, Splitter, Default);
	register_primitive(IndicatorHeaderArrow, HeaderSortIndicator, Header);
	register_primitive(IndicatorMenuCheckMark, IndicatorMenuCheckMark, MenuItem);
//	register_primitive(IndicatorProgressChunk, , );
	register_primitive(IndicatorRadioButton, IndicatorRadioButton, Button);
	register_primitive(IndicatorSpinDown, IndicatorSpinDown, Default);
	register_primitive(IndicatorSpinMinus, IndicatorSpinMinus, Default);
	register_primitive(IndicatorSpinPlus, IndicatorSpinPlus, Default);
	register_primitive(IndicatorSpinUp, IndicatorSpinUp, Default);
	register_primitive(IndicatorToolBarHandle, ToolBarHandle, ToolBar);
	register_primitive(IndicatorToolBarSeparator, ToolBarSeparator, Default);
//	register_primitive(PanelTipLabel, , );
//	register_primitive(IndicatorTabTear, , );
// Qt 4.2 additions
	register_primitive(PanelScrollAreaCorner, ScrollAreaCorner, Default);
// ###	register_primitive(Widget, , );
// Qt 4.3 additions
// TODO register_primitive(IndicatorColumnViewArrow, , );
// Qt 4.4 additions
//	register_primitive(IndicatorItemViewItemDrop, , );
	register_primitive(PanelItemViewItem, PanelItemViewItem, ViewItem);
//	register_primitive(PanelItemViewRow, , );
//	register_primitive(PanelStatusBar, , );
// Qt 4.5 additions
	register_primitive(IndicatorTabClose, IndicatorTabClose, Default);
//      register_primitive(PanelMenu, , );

#define register_element(ce, f, so) draw_element_entry[QStyle::CE_ ## ce].type = QStyleOption::SO_ ## so; draw_element_entry[QStyle::CE_ ## ce].func = (drawElementFunc *) paint ## f;

	/* CONTROL ELEMENT */
// Qt 4.0 Buttons
//	register_element(PushButton, , );
	register_element(PushButtonBevel, PushButtonBevel, Button);
//	register_element(PushButtonLabel, , );
//	register_element(CheckBox, , );
//	register_element(CheckBoxLabel, , );
//	register_element(RadioButton, , );
//	register_element(RadioButtonLabel, , );
// Qt 4.0 Controls
//	register_element(TabBarTab, , );
	register_element(TabBarTabShape, TabBarTabShape, Tab);
	register_element(TabBarTabLabel, TabBarTabLabel, Tab);
//	register_element(ProgressBar, , ProgressBar);
	register_element(ProgressBarGroove, ProgressBarGroove, ProgressBar);
	register_element(ProgressBarContents, ProgressBarContents, ProgressBar);
	register_element(ProgressBarLabel, ProgressBarLabel, ProgressBar);
// Qt 4.0 Menus
	register_element(MenuItem, MenuItem, MenuItem);
//	register_element(MenuScroller, , );
//	register_element(MenuVMargin, , );
//	register_element(MenuHMargin, , );
//	register_element(MenuTearoff, , );
	register_element(MenuEmptyArea, Nothing, Default);
	register_element(MenuBarItem, MenuBarItem, MenuItem);
	register_element(MenuBarEmptyArea, MenuBarEmptyArea, Default);
// Qt 4.0 more Controls
	register_element(ToolButtonLabel, ToolButtonLabel, ToolButton);
//	register_element(Header, , );
	register_element(HeaderSection, HeaderSection, Header);
	register_element(HeaderLabel, HeaderLabel, Header);
//	register_element(Q3DockWindowEmptyArea, , );
//	register_element(ToolBoxTab, , );
        register_element(SizeGrip, SizeGrip, Default);
	register_element(Splitter, Splitter, Default);
	register_element(RubberBand, RubberBand, RubberBand);
	register_element(DockWidgetTitle, DockWidgetTitle, DockWidget);
// Qt 4.0 ScrollBar
	register_element(ScrollBarAddLine, ScrollBarAddLine, Slider);
	register_element(ScrollBarSubLine, ScrollBarSubLine, Slider);
	register_element(ScrollBarAddPage, ScrollBarPage, Slider);
	register_element(ScrollBarSubPage, ScrollBarPage, Slider);
	register_element(ScrollBarSlider, ScrollBarSlider, Slider);
	register_element(ScrollBarFirst, ScrollBarFirst, Slider);
	register_element(ScrollBarLast, ScrollBarLast, Slider);
// Qt 4.0 even more Controls
//	register_element(FocusFrame, , );
	register_element(ComboBoxLabel, ComboBoxLabel, ComboBox);
// Qt 4.1 additions
	register_element(ToolBar, PanelToolBar, ToolBar);
// Qt 4.3 additions
	register_element(ToolBoxTabShape, ToolBoxTabShape, ToolBox);
	register_element(ToolBoxTabLabel, ToolBoxTabLabel, ToolBox);
	register_element(HeaderEmptyArea, HeaderEmptyArea, Default);
	register_element(ColumnViewGrip, Splitter, Default);
// Qt 4.4 additions
//	register_element(ItemViewItem, , );
}


/*-----------------------------------------------------------------------*/

//#include "skulpture_p.moc"


