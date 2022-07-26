/*
 * skulpture_layout.cpp
 *
 */

#include "skulpture_p.h"
#include <QtCore/QSettings>
#include <QFrame>
#include <QApplication>
#include <QShortcut>
#include <QLayout>
#include <QFormLayout>
#include <QLabel>
#include <QCheckBox>

QSize minSize = QSize(0,0);

/*-----------------------------------------------------------------------*/

int SkulptureStyle::Private::verticalTextShift(const QFontMetrics &fontMetrics)
{
    if (fontMetrics.xHeight() == qApp->fontMetrics().xHeight()
        && fontMetrics.ascent() == qApp->fontMetrics().ascent()
        && fontMetrics.descent() == qApp->fontMetrics().descent()
        && fontMetrics.leading() == qApp->fontMetrics().leading()) {
        return textShift;
    }
    return 0;
}


static QFontMetrics styledFontMetrics(const QStyleOption *option, const QWidget *widget)
{
    if (option) {
        return option->fontMetrics;
    } else if (widget) {
        return widget->fontMetrics();
    }
    return qApp->fontMetrics();
}


int SkulptureStyle::Private::textLineHeight(const QStyleOption *option, const QWidget *widget)
{
    QFontMetrics fm = styledFontMetrics(option, widget);
    return fm.height() + (verticalTextShift(fm) & 1);
}


static int fontHeight(const QStyleOption *option, const QWidget *widget)
{
    return styledFontMetrics(option, widget).height();
}


/*-----------------------------------------------------------------------*/

int SkulptureStyle::pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
    switch (metric) {
        /* entries are stricly sorted in Qt order for future lookup table */
        case PM_ButtonMargin: return 2 * d->pushButtonSize;
        case PM_ButtonDefaultIndicator: return 0;
        case PM_MenuButtonIndicator: return fontHeight(option, widget);
        case PM_ButtonShiftHorizontal: {
            Qt::LayoutDirection direction;
            if (option) {
                direction = option->direction;
            } else if (widget) {
                direction = widget->layoutDirection();
            } else {
                direction = QApplication::layoutDirection();
            }
            return direction == Qt::LeftToRight ? 1 : -1;
        }
        case PM_ButtonShiftVertical: return 1;

        case PM_DefaultFrameWidth: return 2; // 3 for command buttons
        case PM_SpinBoxFrameWidth: return 2;
        case PM_ComboBoxFrameWidth: return 2; // 3 for non-editable combobox (in button mode)

        case PM_MaximumDragDistance: return -1;

        case PM_ScrollBarExtent: {
            if (d->scrollBarSize > 0) {
                return d->scrollBarSize;
            }
             // do not depend on widget font size
            int extent = ((fontHeight(0, 0) >> 1) & ~1) + 9;
#if 0
            if (option && (option->state & QStyle::State_Horizontal)) {
                return (qMax(extent, qApp->globalStrut().height()) & ~1) + 1;
            } else {
                return (qMax(extent, qApp->globalStrut().width()) & ~1) + 1;
            }
#else
            return extent;
#endif
        }
        case PM_ScrollBarSliderMin: {
            if (d->scrollBarLength > 0) {
                return d->scrollBarLength;
            }
            return fontHeight(0, 0) + 1;
        }

        case PM_SliderThickness:
        case PM_SliderControlThickness: {
            if (d->sliderSize > 0) {
                return d->sliderSize + 4;
            }
            int thickness = (fontHeight(option, widget) & ~1) + 5;
            if (option && !(option->state & QStyle::State_Horizontal)) {
                return (qMax(thickness, minSize.width()) & ~1) + 1;
            } else {
                return (qMax(thickness, minSize.height()) & ~1) + 1;
            }
        }

        case PM_SliderLength: {
            if (d->sliderLength > 0) {
                return d->sliderLength;
            }
            int length = (fontHeight(option, widget)) + 6;
            if (option && !(option->state & QStyle::State_Horizontal)) {
                return qMax(length, minSize.height());
            } else {
                return qMax(length, minSize.width());
            }
        }
        case PM_SliderTickmarkOffset: {
            if (const QStyleOptionSlider *sliderOption = qstyleoption_cast<const QStyleOptionSlider *>(option)) {
                if (sliderOption->tickPosition == QSlider::TicksAbove) {
                    return 5;
                } else if (sliderOption->tickPosition == QSlider::TicksBothSides) {
                    return 5;
                } else if (sliderOption->tickPosition == QSlider::TicksBelow) {
                    return 0;
                }
            }
            return 0;
        }
        case PM_SliderSpaceAvailable: {
            return QCommonStyle::pixelMetric(metric, option, widget);
        }

        case PM_DockWidgetSeparatorExtent: return ((qMax(fontHeight(option, widget), 16) >> 1) & ~1) - 1;
        case PM_DockWidgetHandleExtent: return 8;
        case PM_DockWidgetFrameWidth: return 2;

        case PM_TabBarTabOverlap: return 2;
        case PM_TabBarTabHSpace: return 2 * d->tabBarSize + (fontHeight(option, widget) & ~1);
        case PM_TabBarTabVSpace: return 0;
        case PM_TabBarBaseHeight: return 2;
        case PM_TabBarBaseOverlap: return 2;

        case PM_ProgressBarChunkWidth: return fontHeight(option, widget) >> 1;

        case PM_SplitterWidth: return ((qMax(fontHeight(option, widget), 16) >> 1) & ~1) - 1;
        case PM_TitleBarHeight: return d->textLineHeight(option, widget) + 4;

        case PM_MenuScrollerHeight: return (fontHeight(option, widget) >> 1) + 2;
        case PM_MenuHMargin: return 1;
        case PM_MenuVMargin: return 1;
        case PM_MenuPanelWidth: return 1;
        case PM_MenuTearoffHeight: return (fontHeight(option, widget) >> 1) + 2;
        case PM_MenuDesktopFrameWidth: return 0;

        case PM_MenuBarPanelWidth: return 0;
        case PM_MenuBarItemSpacing: return 0;
        case PM_MenuBarVMargin: return 0;
        case PM_MenuBarHMargin: return 0;

        case PM_IndicatorWidth:
        case PM_IndicatorHeight:
        case PM_ExclusiveIndicatorWidth:
        case PM_ExclusiveIndicatorHeight:
#ifdef QT3_SUPPORT
        case PM_CheckListButtonSize:
        case PM_CheckListControllerSize:
#endif
        {
            // do not use strut width, because label is usually wide enough
            return qMax(d->textLineHeight(option, widget), minSize.height());
        }

        case PM_DialogButtonsSeparator: return 6;
        case PM_DialogButtonsButtonWidth: return 64;
        case PM_DialogButtonsButtonHeight: return 16;

        case PM_MdiSubWindowFrameWidth: return 3;
        case PM_MdiSubWindowMinimizedWidth: return fontHeight(option, widget) * 12;

        case PM_HeaderMargin: return 3;
        case PM_HeaderMarkSize: return 5;
        case PM_HeaderGripMargin: return 4;
        case PM_TabBarTabShiftHorizontal: return 0;
        case PM_TabBarTabShiftVertical: return 0;
        case PM_TabBarScrollButtonWidth: return (fontHeight(option, widget) & ~1) + 1;

        case PM_ToolBarFrameWidth: return 1;
        case PM_ToolBarHandleExtent: return 9;
        case PM_ToolBarItemSpacing: return 2;
        case PM_ToolBarItemMargin: return 0;
        case PM_ToolBarSeparatorExtent: return 4;
        case PM_ToolBarExtensionExtent: return 12;

        case PM_SpinBoxSliderHeight: return 2;
/*
        case PM_DefaultTopLevelMargin: {
            if (d->dialogMargins >= 0) {
                return d->dialogMargins;
            }
            return (fontHeight(option, widget) >> 1);
        }
        case PM_DefaultChildMargin: {
            if (d->widgetMargins >= 0) {
                return d->widgetMargins;
            }
            return (fontHeight(option, widget) >> 1);
        }
        case PM_DefaultLayoutSpacing: {
            if (d->verticalSpacing >= 0) {
                return d->verticalSpacing;
            }
            return ((fontHeight(option, widget) * 3) >> 3);
        }*/

        case PM_ToolBarIconSize:
            return QCommonStyle::pixelMetric(metric, option, widget);
        case PM_ListViewIconSize: return pixelMetric(PM_SmallIconSize, option, widget);
        case PM_IconViewIconSize: return pixelMetric(PM_LargeIconSize, option, widget);
        case PM_SmallIconSize: {
            int iconSize = d->textLineHeight(option, widget);
            return iconSize;
        }
        case PM_LargeIconSize: {
            const bool hasSvgIcons = false;
            int iconSize = 2 * fontHeight(option, widget);
            if (!hasSvgIcons) {
                if (iconSize < 28) {
                    return 22;
                } else if (iconSize < 40) {
                    return 32;
                } else if (iconSize < 56) {
                    return 48;
                } else if (iconSize < 96) {
                    return 64;
                }
            }
            return iconSize;
        }
        case PM_FocusFrameVMargin: return 2;
        case PM_FocusFrameHMargin: return 2;

        case PM_ToolTipLabelFrameWidth: return 1;
        case PM_CheckBoxLabelSpacing: {
            if (d->labelSpacing >= 0) {
                return d->labelSpacing;
            }
            return (fontHeight(option, widget) >> 1) - 2;
        }
        case PM_TabBarIconSize: return pixelMetric(PM_SmallIconSize, option, widget);
        case PM_SizeGripSize: return 13; // ### make this variable
        case PM_DockWidgetTitleMargin: return 2;

        case PM_MessageBoxIconSize: return pixelMetric(PM_LargeIconSize, option, widget);
        case PM_ButtonIconSize: return pixelMetric(PM_SmallIconSize, option, widget);

        case PM_DockWidgetTitleBarButtonMargin: return 0;
        case PM_RadioButtonLabelSpacing:  {
            if (d->labelSpacing >= 0) {
                return d->labelSpacing;
            }
            return (fontHeight(option, widget) >> 1) - 2;
        }
        case PM_LayoutLeftMargin:
        case PM_LayoutTopMargin:
        case PM_LayoutRightMargin:
        case PM_LayoutBottomMargin: {
           /* PixelMetric metric = QStyle::PM_DefaultChildMargin;
            if ((option && (option->state & QStyle::State_Window)) || (widget && widget->isWindow())) {
                metric = QStyle::PM_DefaultTopLevelMargin;
            }*/
            return 10;//pixelMetric(metric, option, widget);
        }

        case PM_LayoutHorizontalSpacing:
        case PM_LayoutVerticalSpacing: {
            return -1;
        }

        case PM_TabBar_ScrollButtonOverlap: return 0;
        case PM_TextCursorWidth: {
            if (d->textCursorWidth > 0) {
                return qMax(1, int(d->textCursorWidth + 0.5));
            }
            return qMax(1, (fontHeight(option, widget) + 8) / 12);
        }

        case PM_TabCloseIndicatorWidth:
        case PM_TabCloseIndicatorHeight: {
            return (fontHeight(option, widget) & ~1);
        }
        case PM_ScrollView_ScrollBarSpacing: return 0;
        case PM_ScrollView_ScrollBarOverlap: return 0;

        case PM_SubMenuOverlap: return -2;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
        case PM_TreeViewIndentation: return fontHeight(option, widget);
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
        case PM_HeaderDefaultSectionSizeHorizontal: return 10 + 6 * fontHeight(option, widget);
        case PM_HeaderDefaultSectionSizeVertical: return 6 + fontHeight(option, widget);
#endif
        case PM_CustomBase: // avoid warning
            break;
    }
    return ParentStyle::pixelMetric(metric, option, widget);
}


/*-----------------------------------------------------------------------*/
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
int SkulptureStyle::layoutSpacing(QSizePolicy::ControlType control1, QSizePolicy::ControlType control2, Qt::Orientation orientation, const QStyleOption *option, const QWidget *widget) const
#else
int SkulptureStyle::layoutSpacingImplementation(QSizePolicy::ControlType control1, QSizePolicy::ControlType control2, Qt::Orientation orientation, const QStyleOption *option, const QWidget *widget) const
#endif
{
    Q_UNUSED(control2);

    if (orientation == Qt::Horizontal) {
        if (control1 == QSizePolicy::Label) {
            if (d->labelSpacing >= 0) {
                return d->labelSpacing + 2;
            }
            return fontHeight(option, widget) >> 1;
        }
        if (d->horizontalSpacing >= 0) {
            return d->horizontalSpacing;
        }
        return fontHeight(option, widget) >> 1;
    }
    if (control1 & (QSizePolicy::CheckBox | QSizePolicy::RadioButton)
     && control2 & (QSizePolicy::CheckBox | QSizePolicy::RadioButton)) {
        if (d->verticalSpacing >= 0) {
            return qMax(0, d->verticalSpacing - 2);
        }
        return pixelMetric(PM_LayoutVerticalSpacing, option, widget) - 2;
    }
    if (d->verticalSpacing >= 0) {
        return d->verticalSpacing;
    }
    return pixelMetric(PM_LayoutVerticalSpacing, option, widget);
}


/*-----------------------------------------------------------------------*/

extern QSize sizeFromContentsToolButton(const QStyleOptionToolButton *option, const QSize &contentsSize, const QWidget *widget, const QStyle *style, int toolButtonSize);
extern QSize sizeFromContentsMenuItem(const QStyleOptionMenuItem *option, const QSize &contentsSize, const QWidget *widget, const QStyle *style, int menuItemSize, int textLineHeight);


/*-----------------------------------------------------------------------*/

static inline QSize sizeFromContentsPushButton(const QStyleOptionButton *option, const QSize &contentsSize, const QWidget *widget, const QStyle *style, int pushButtonSize, int textLineHeight)
{
    Q_UNUSED(style);

    // width
    int w = contentsSize.width() + (fontHeight(option, widget) & ~1);
    if (!option->text.isEmpty()) {
        w += 6 + 2 * pushButtonSize;
        const int sizeBase = qMin(64, 4 * fontHeight(option, widget));
        const int sizeIncrement = qMin(32, qMin(sizeBase, qMax(1, 2 * pushButtonSize)));
        if (w < sizeBase) {
            w = sizeBase;
        } else {
            w = ((w - sizeBase + sizeIncrement - 1) / sizeIncrement) * sizeIncrement + sizeBase;
        }
    }

    // height
    int h = qMax(contentsSize.height(), textLineHeight);
    h += 2 * pushButtonSize + 4;

    return QSize(w, h);
}


static inline QSize sizeFromContentsComboBox(const QStyleOptionComboBox *option, const QSize &contentsSize, const QWidget *widget, const QStyle *style, int widgetSize, int textLineHeight)
{
//    if (contentsSize.height() > 16) {
//        fh = contentsSize.height() - 2;
//    }
    return ((const QCommonStyle *) style)->QCommonStyle::sizeFromContents(QStyle::CT_ComboBox, option, QSize(contentsSize.width(), textLineHeight + 2 * widgetSize), widget);
}


static inline QSize sizeFromContentsLineEdit(const QStyleOptionFrame *option, const QSize &contentsSize, const QWidget *widget, const QStyle *style, int widgetSize, int textLineHeight)
{
    Q_UNUSED(widget); Q_UNUSED(style);

    int fw = option->lineWidth;
    int fh = textLineHeight;
//    if (contentsSize.height() > 14) {
//        fh = contentsSize.height() - 4;
//    }
    return QSize(contentsSize.width() + 6 + 2 * fw, fh + 2 * (widgetSize + fw));
}


static inline QSize sizeFromContentsSpinBox(const QStyleOptionSpinBox *option, const QSize &contentsSize, const QWidget *widget, const QStyle *style)
{
    Q_UNUSED(option); Q_UNUSED(widget); Q_UNUSED(style);

    int fw = option->frame ? style->proxy()->pixelMetric(QStyle::PM_SpinBoxFrameWidth, option, widget) : 0;
    int bw = option->buttonSymbols != QAbstractSpinBox::NoButtons ? qMax(style->pixelMetric(QStyle::PM_ScrollBarExtent, option, widget), minSize.width()) : 0;
    return contentsSize + QSize(2 * fw + bw + 8, 2 * fw);
}


static inline QSize sizeFromContentsGroupBox(const QStyleOptionGroupBox *option, const QSize &contentsSize, const QWidget *widget, const QStyle *style)
{
    Q_UNUSED(widget); Q_UNUSED(style);

    if (!(option->features & QStyleOptionFrame::Flat)) {
        return contentsSize + QSize(fontHeight(option, widget) & ~1, 0);
    }
    return contentsSize;
}


static inline QSize sizeFromContentsTabBarTab(const QStyleOptionTab *option, const QSize &contentsSize, const QWidget *widget, const QStyle *style, int tabBarSize, int textShift)
{
    Q_UNUSED(widget); Q_UNUSED(style);

    if (int(option->shape) & 2) {
        return (contentsSize + QSize(4 + 2 * tabBarSize, 0)).expandedTo(minSize);
    }
    if (!option->icon.isNull()) {
        textShift = 0;
    }
    return (contentsSize + QSize(0, 2 + 2 * tabBarSize + (textShift & 1))).expandedTo(minSize);
}


static inline QSize sizeFromContentsProgressBar(const QStyleOptionProgressBar *option, const QSize &contentsSize, const QWidget *widget, const QStyle *style, int widgetSize, int textShift)
{
    Q_UNUSED(widget); Q_UNUSED(style);

    if (option->version >= 2 && ((const QStyleOptionProgressBar *) option)->state == Qt::Vertical) {
        return contentsSize + QSize(2 * widgetSize, 6);
    }
    return contentsSize + QSize(6, (textShift & 1) + 2 * widgetSize - 6);
}


/*-----------------------------------------------------------------------*/

#define CT_CASE(ct, so) \
    case CT_## ct: \
        if (option && option->type == QStyleOption::SO_## so) { \
            return sizeFromContents ## ct((const QStyleOption ## so *) option, contentsSize, widget, this); \
        } \
        break


QSize SkulptureStyle::sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &contentsSize, const QWidget *widget) const
{
    switch (type) {
        case CT_PushButton:
            if (option && option->type == QStyleOption::SO_Button) {
                return sizeFromContentsPushButton((const QStyleOptionButton *) option, contentsSize, widget, this, d->pushButtonSize, d->textLineHeight(option, widget));
            }
            break;
        case CT_CheckBox:
        case CT_RadioButton:
            return ParentStyle::sizeFromContents(type, option, contentsSize, widget) + QSize(0, 2 * d->widgetSize - 4);
            break;

        case CT_ToolButton:
            if (option && option->type == QStyleOption::SO_ToolButton) {
                return sizeFromContentsToolButton((const QStyleOptionToolButton *) option, contentsSize, widget, this, d->toolButtonSize);
            }
            break;
        case CT_ComboBox:
            if (option && option->type == QStyleOption::SO_ComboBox) {
                return sizeFromContentsComboBox((const QStyleOptionComboBox *) option, contentsSize, widget, this, d->widgetSize, d->textLineHeight(option, widget));
            }
            break;

        case CT_Splitter:
#ifdef QT3_SUPPORT
        case CT_Q3DockWindow:
#endif
            return contentsSize;
        case CT_ProgressBar:
            if (option && option->type == QStyleOption::SO_ProgressBar) {
                return sizeFromContentsProgressBar((const QStyleOptionProgressBar *) option, contentsSize, widget, this, d->widgetSize, d->verticalTextShift(styledFontMetrics(option, widget)));
            }
            break;

        case CT_MenuItem:
            if (option && option->type == QStyleOption::SO_MenuItem) {
                return sizeFromContentsMenuItem((const QStyleOptionMenuItem *) option, contentsSize, widget, this, 2 * d->menuItemSize, d->textLineHeight(option, widget));
            }
            break;
        case CT_MenuBarItem: {
            int h = 2 * (d->menuBarSize >= 0 ? d->menuBarSize : 3) + d->textLineHeight(option, widget);
            return QSize(contentsSize.width() + 2 + (((fontHeight(option, widget) * 7) >> 3) & ~1), h).expandedTo(minSize);
        }
        case CT_MenuBar: return contentsSize;
        case CT_Menu: return contentsSize;

        case CT_TabBarTab:
            if (option && option->type == QStyleOption::SO_Tab) {
                return sizeFromContentsTabBarTab((const QStyleOptionTab *) option, contentsSize, widget, this, d->tabBarSize, d->verticalTextShift(styledFontMetrics(option, widget)));
            }
            break;

        case CT_Slider:
        case CT_ScrollBar:
#ifdef QT3_SUPPORT
        case CT_Q3Header:
#endif
            return contentsSize;

        case CT_LineEdit:
            if (option && option->type == QStyleOption::SO_Frame) {
                return sizeFromContentsLineEdit((const QStyleOptionFrame *) option, contentsSize, widget, this, d->widgetSize, d->textLineHeight(option, widget));
            }
            break;
        CT_CASE(SpinBox, SpinBox);

        case CT_SizeGrip: return contentsSize;
        case CT_TabWidget: return contentsSize + QSize(4, 4);
        case CT_DialogButtons: return contentsSize;
        case CT_HeaderSection: break;

        CT_CASE(GroupBox, GroupBox);
        case CT_MdiControls: break;
        case CT_ItemViewItem: break;

        case CT_CustomBase: // avoid warning
            break;
    }
    return ParentStyle::sizeFromContents(type, option, contentsSize, widget);
}


/*-----------------------------------------------------------------------*/

extern QRect subElementRectDockWidget(QStyle::SubElement element, const QStyleOptionDockWidget *option, const QWidget *widget, const QStyle *style);
extern QRect subElementRectComboBoxFocusRect(const QStyleOptionComboBox *option, const QWidget *widget, const QStyle *style);


/*-----------------------------------------------------------------------*/

static inline QRect subElementRectFrameContents(const QStyleOption *option, const QWidget *widget, const QStyle *style)
{
    Q_UNUSED(style);

    if (widget && widget->inherits("KHTMLView")) {
        QWidget *window;
        // case 1: the parent widget is a QFrame, and already has a sunken frame
        //  do not show any frame (KMail mail view, Kopete chat view)
        if (QFrame *frame = qobject_cast<QFrame *>(widget->parentWidget())) {
            if (frame->frameShape() == QFrame::StyledPanel) {
                // ### fix Kopete frame
                // frame->setFrameShadow(QFrame::Sunken);
                return option->rect;
            }
        } else if ((window = widget->window()) && window->inherits("KonqMainWindow")) {
            // case 2: the html view covers the full width of window:
            //  do not show side frames (Konqueror)
            return option->rect.adjusted(0, 2, 0, -2);
        } else {
            // case 3: detect KMail 4.2
            while ((widget = widget->parentWidget())) {
                if (widget->inherits("KMReaderWin")) {
                    return option->rect;
                }
            }
        }
    }
    return option->rect.adjusted(2, 2, -2, -2);
}


static inline QRect subElementRectLineEditContents(const QStyleOptionFrame *option, const QWidget *widget, const QStyle *style, int textShift)
{
    Q_UNUSED(widget); Q_UNUSED(style);

    int fw = option->lineWidth;
    int adjust = 0;
    if (textShift & 1) {
        adjust = -1;
        textShift &= ~1;
    }
    return option->rect.adjusted(fw + 2, fw + ((-textShift) >> 1), -fw - 2, -fw + ((-textShift) >> 1) + adjust);
}


/*-----------------------------------------------------------------------*/

QRect SkulptureStyle::subElementRect(SubElement element, const QStyleOption *option, const QWidget *widget) const
{
    switch (element) {
        case SE_PushButtonContents:
        case SE_PushButtonFocusRect:

        case SE_CheckBoxIndicator:
        case SE_CheckBoxContents:
        case SE_CheckBoxFocusRect:
        case SE_CheckBoxClickRect:

        case SE_RadioButtonIndicator:
        case SE_RadioButtonContents:
        case SE_RadioButtonFocusRect:
        case SE_RadioButtonClickRect:
            break;

        case SE_ComboBoxFocusRect:
            if (option->type == QStyleOption::SO_ComboBox) {
                return subElementRectComboBoxFocusRect((const QStyleOptionComboBox *) option, widget, this);
            }
            break;

        case SE_SliderFocusRect:
#ifdef QT3_SUPPORT
        case SE_Q3DockWindowHandleRect:
#endif
            break;

        case SE_ProgressBarGroove:
        case SE_ProgressBarContents:
        case SE_ProgressBarLabel:
            return option->rect;
#ifdef QT3_SUPPORT
        case SE_DialogButtonAccept:
        case SE_DialogButtonReject:
        case SE_DialogButtonApply:
        case SE_DialogButtonHelp:
        case SE_DialogButtonAll:
        case SE_DialogButtonAbort:
        case SE_DialogButtonIgnore:
        case SE_DialogButtonRetry:
        case SE_DialogButtonCustom:
            break;
#endif
        case SE_ToolBoxTabContents:
            return option->rect.adjusted(11, 0, -6, 0);

        case SE_HeaderLabel:
        case SE_HeaderArrow:
            break;

        case SE_TabWidgetTabBar:
        case SE_TabWidgetTabPane:
        case SE_TabWidgetTabContents:
            break;
        case SE_TabWidgetLeftCorner:
        case SE_TabWidgetRightCorner:
            if (option->type == QStyleOption::SO_TabWidgetFrame) {
                if (((const QStyleOptionTabWidgetFrame *) option)->lineWidth == 0) {
                    break;
                }
                if ((((const QStyleOptionTabWidgetFrame *) option)->shape & 3) == 1) {
                    return QCommonStyle::subElementRect(element, option, widget).adjusted(1, -1, -1, -1);
                }
            }
            return QCommonStyle::subElementRect(element, option, widget).adjusted(1, 1, -1, 1);

        case SE_ItemViewItemCheckIndicator:
            break;

        case SE_TabBarTearIndicator:

        case SE_TreeViewDisclosureItem:
            break;

        case SE_LineEditContents:
            if (option->type == QStyleOption::SO_Frame) {
                return subElementRectLineEditContents((const QStyleOptionFrame *) option, widget, this, d->textShift);
            }
            break;
        case SE_FrameContents:
            return subElementRectFrameContents(option, widget, this);

        case SE_DockWidgetCloseButton:
        case SE_DockWidgetFloatButton:
        case SE_DockWidgetTitleBarText:
        case SE_DockWidgetIcon:
            if (option->type == QStyleOption::SO_DockWidget) {
                return subElementRectDockWidget(element, (const QStyleOptionDockWidget *) option, widget, this);
            }
            break;

        case SE_CheckBoxLayoutItem:
        case SE_ComboBoxLayoutItem:
        case SE_DateTimeEditLayoutItem:
            break;
        //case SE_DialogButtonBoxLayoutItem: break;
        case SE_LabelLayoutItem:
        case SE_ProgressBarLayoutItem:
        case SE_PushButtonLayoutItem:
        case SE_RadioButtonLayoutItem:
        case SE_SliderLayoutItem:
        case SE_SpinBoxLayoutItem:
        case SE_ToolButtonLayoutItem:
            break;

        case SE_FrameLayoutItem:
        case SE_GroupBoxLayoutItem:
        case SE_TabWidgetLayoutItem:
            break;

        case SE_ItemViewItemDecoration:
        case SE_ItemViewItemText:
        case SE_ItemViewItemFocusRect:
            break;

        case SE_TabBarTabLeftButton:
        case SE_TabBarTabRightButton:
        case SE_TabBarTabText:
            break;
        case SE_ShapedFrameContents:
            break;

        case SE_ToolBarHandle:
            break;

        case SE_CustomBase: // avoid warning
            break;
    }
    return ParentStyle::subElementRect(element, option, widget);
}


/*-----------------------------------------------------------------------*/

void SkulptureStyle::Private::polishFormLayout(QFormLayout *layout)
{
    if (layout->labelAlignment() & Qt::AlignVCenter) {
        return;
    }
    int addedHeight = -1;
    for (int row = 0; row < layout->rowCount(); ++row) {
        QLayoutItem *labelItem = layout->itemAt(row, QFormLayout::LabelRole);
        if (!labelItem) {
            continue;
        }
        QLayoutItem *fieldItem = layout->itemAt(row, QFormLayout::FieldRole);
        if (!fieldItem) {
            continue;
        }
        QWidget *label = labelItem->widget();
        if (!label) {
            continue;
        }
        int labelHeight;
        if (addedHeight < 0) {
#if 0
            // fixed value in Qt
            static const int verticalMargin = 1;
            QStyleOptionFrame option;
            option.initFrom(label);
            option.lineWidth = label->style()->pixelMetric(QStyle::PM_DefaultFrameWidth, &option, label);
            option.midLineWidth = 0;
            option.rect = QRect(0, 0, 10, fontHeight(option, label) + 2 * verticalMargin);
            // label should be aligned centered to LineEdit, so use its size
            addedHeight = label->style()->sizeFromContents(QStyle::CT_LineEdit, &option, option.rect.size(), label).height() - fontHeight(option, height);
#else
            addedHeight = 4 + 2 * widgetSize;
#endif
        }
        if (qobject_cast<QLabel *>(label)) {
            labelHeight = label->sizeHint().height() + addedHeight;
        } else if (qobject_cast<QCheckBox *>(label)) {
            labelHeight = label->sizeHint().height();
        } else {
            continue;
        }
        int fieldHeight = fieldItem->sizeHint().height();
        /* for large fields, we don't center */
        if (fieldHeight <= 2 * fontHeight(0, label) + addedHeight) {
            if (fieldHeight > labelHeight) {
                labelHeight = fieldHeight;
            }
        } else {
            if (verticalTextShift(label->fontMetrics()) & 1) {
                labelHeight += 1;
            }
        }
        if (qobject_cast<QCheckBox *>(label)) {
            label->setMinimumHeight(labelHeight);
        } else {
            // QFormLayout determines label size as height * 7 / 4, so revert that
            label->setMinimumHeight((labelHeight * 4 + 6) / 7);
        }
    }
}


void SkulptureStyle::Private::polishLayout(QLayout *layout)
{
    if (forceSpacingAndMargins) {
        if (QFormLayout *formLayout = qobject_cast<QFormLayout *>(layout)) {
            if (formLayout->spacing() >= 2) {
                formLayout->setSpacing(-1);
            }
        } else
        if (QGridLayout *gridLayout = qobject_cast<QGridLayout *>(layout)) {
            if (gridLayout->spacing() >= 2) {
                gridLayout->setSpacing(-1);
            }
        } else if (QBoxLayout *boxLayout = qobject_cast<QBoxLayout *>(layout)) {
            if (boxLayout->spacing() >= 2) {
                boxLayout->setSpacing(-1);
            }
        } else {
            if (layout->spacing() >= 2) {
                layout->setSpacing(-1);
            }
        }
        if (layout->contentsMargins().left()  >= 4 &&
            layout->contentsMargins().right() >= 4 &&
            layout->contentsMargins().top()   >= 4 &&
            layout->contentsMargins().bottom() >= 4) {
            layout->setContentsMargins(-1,-1,-1,-1);
        }
    }
    if (QFormLayout *formLayout = qobject_cast<QFormLayout *>(layout)) {
        polishFormLayout(formLayout);
    }
    // recurse into layouts
    for (int i = 0; i < layout->count(); ++i) {
        QLayoutItem *item = layout->itemAt(i);
        if (QLayout *layout = item->layout()) {
            polishLayout(layout);
        }
    }
}


