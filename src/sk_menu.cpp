/*
 * skulpture_menu.cpp
 *
 */

#include "skulpture_p.h"
#include <QtGui/QPainter>
#include <QMenu>
#include <QShortcut>
#include <QtWidgets/QApplication>


/*-----------------------------------------------------------------------*/

void paintFrameMenu(QPainter *painter, const QStyleOptionFrame *option)
{
    paintThinFrame(painter, option->rect, option->palette, -60, 160);
    paintThinFrame(painter, option->rect.adjusted(1, 1, -1, -1), option->palette, -20, 60);
}


void paintPanelMenuBar(QPainter *painter, const QStyleOptionFrame *option)
{
    Q_UNUSED(painter); Q_UNUSED(option);
//  paintThinFrame(painter, option->rect, option->palette, -20, 60);
//  painter->fillRect(option->rect.adjusted(1, 1, -1, -1), option->palette.color(QPalette::Window));
}


void paintMenuBarEmptyArea(QPainter *painter, const QStyleOption *option)
{
        Q_UNUSED(painter); Q_UNUSED(option);
//  painter->fillRect(option->rect, option->palette.color(QPalette::Window));
}


/*-----------------------------------------------------------------------*/

extern void paintCommandButtonPanel(QPainter *painter, const QStyleOptionButton *option, const QWidget *widget);

void paintMenuBarItem(QPainter *painter, const QStyleOptionMenuItem *option, const QWidget *widget, const QStyle *style)
{
    QStyleOptionMenuItem opt = *option;
    if (option->state & QStyle::State_Selected || option->state & QStyle::State_MouseOver) {
        QStyleOptionButton button;

        button.QStyleOption::operator=(*option);
        button.features = QStyleOptionButton::None;
        button.state |= QStyle::State_MouseOver;
        painter->save();
        painter->setClipRect(button.rect.adjusted(1, 1, -1, -1));
        // call without widget to get QPalette::Button background
        paintCommandButtonPanel(painter, &button, 0);
        painter->restore();
    } else {
        opt.palette.setColor(QPalette::ButtonText, opt.palette.color(QPalette::WindowText));
    }
    opt.state &= ~QStyle::State_Sunken;
    ((QCommonStyle *) style)->QCommonStyle::drawControl(QStyle::CE_MenuBarItem, &opt, painter, widget);
}


/*-----------------------------------------------------------------------*/

enum MenuMode {
    ButtonMenu,
    ItemViewMenu,
};

enum CheckColumnMode {
    UseIconColumn,
    ShowCheckColumn,
    NoCheckColumn
};

enum IconColumnMode {
    ForceIconColumn,
    ShowIconColumn,
    HideIconColumn
};

enum SeparatorMode {
    TextSeparator,
    IconSeparator,
    ItemSeparator
};

enum SelectionMode {
    TextSelection,
    IconSelection,
    ItemSelection
};


/*-----------------------------------------------------------------------*/

QSize sizeFromContentsMenuItem(const QStyleOptionMenuItem *option, const QSize &contentsSize, const QWidget *widget, const QStyle *style, int menuItemSize, int textLineHeight)
{
    if (option->menuItemType == QStyleOptionMenuItem::Separator) {
        if (option->text.isEmpty()) {
            return QSize(4, 4);
        }
        // ### width?
        return QSize(4, QFontMetrics(option->font).height() + 8);
    }

    int w = contentsSize.width(), h = contentsSize.height();

    // always make room for icon column
    const int iconMargin = 4;
    int iconWidth;
    if (option->maxIconWidth) {
        iconWidth = option->maxIconWidth - 4 + 2 * iconMargin;
    } else {
        iconWidth = style->pixelMetric(QStyle::PM_SmallIconSize, option, widget) + 2 * iconMargin;
    }
    w += iconWidth;

    // add size for check column
    const int checkMargin = 4;
    if (option->checkType != QStyleOptionMenuItem::NotCheckable) {
        w += style->pixelMetric(QStyle::PM_IndicatorWidth, option, widget) + checkMargin;
    }

    // submenu arrow and spacing
    w += option->fontMetrics.height() + 4;
    if (option->menuItemType == QStyleOptionMenuItem::SubMenu || option->text.indexOf(QChar('\t', 0)) >= 0) {
        w += option->fontMetrics.height();
    }

    if (h < textLineHeight) {
        h = textLineHeight;
    }
    h += menuItemSize;

    return QSize(w, qMax(h, QSize(0,0).height()));
}


/*-----------------------------------------------------------------------*/

inline QRect remainingHorizontalVisualRect(const QRect &rect, const QStyleOption *option, int width)
{
    return rect.adjusted(option->direction == Qt::LeftToRight ? width : 0, 0, option->direction != Qt::LeftToRight ? -width : 0, 0);
}


inline QRect horizontalVisualRect(const QRect &rect, const QStyleOption *option, int width)
{
    QRect res = rect;

    if (option->direction != Qt::LeftToRight) {
        res.setLeft(rect.left() + rect.width() - width);
    } else {
        res.setWidth(width);
    }
    return res;
}


inline QRect rightHorizontalVisualRect(const QRect &rect, const QStyleOption *option, int width)
{
    QRect res = rect;

    if (option->direction != Qt::LeftToRight) {
        res.setWidth(width);
    } else {
        res.setLeft(rect.left() + rect.width() - width);
    }
    return res;
}


/*-----------------------------------------------------------------------*/

void paintMenuItem(QPainter *painter, const QStyleOptionMenuItem *option, const QWidget *widget, const QStyle *style)
{
    // configuration
    const MenuMode menuMode = ButtonMenu;
    const CheckColumnMode checkColumnMode = option->maxIconWidth ? (/*option->menuHasCheckableItems ? UseIconColumn :*/ NoCheckColumn) : NoCheckColumn;
    const bool showUncheckedIndicator = true;
    const bool showShortcut = true;
    const IconColumnMode iconColumnMode = checkColumnMode == UseIconColumn && option->menuHasCheckableItems ? ForceIconColumn : option->maxIconWidth ? ShowIconColumn : ShowIconColumn;
    const SeparatorMode separatorMode = TextSeparator;
    const SelectionMode selectionMode = ItemSelection;

    // layout
    // ### make margins configurable
    const int checkMargin = 4;
    const int iconMargin = 4;
    const int checkSize = option->menuHasCheckableItems ? qMax(style->pixelMetric(QStyle::PM_ExclusiveIndicatorWidth, option, widget), style->pixelMetric(QStyle::PM_IndicatorWidth, option, widget)) : 0;
    const int iconSize = style->pixelMetric(QStyle::PM_SmallIconSize, option, widget);
    const int checkColumnWidth = checkSize + 2 * checkMargin;
    // FIXME qMax(checkSize, iconSize) for useIconColum
    const int iconColumnWidth = iconColumnMode == HideIconColumn ? 0 : option->maxIconWidth ? option->maxIconWidth - 4 + 2 * iconMargin : iconSize + 2 * iconMargin;
    const QRect itemRect = option->rect;
    QRect iconRect = horizontalVisualRect(itemRect, option, iconColumnWidth);
    QRect textRect = remainingHorizontalVisualRect(itemRect, option, iconColumnWidth);

    // background color
    QPalette::ColorRole menuBackgroundRole;
    QPalette::ColorRole menuForegroundRole;
    QBrush menuBackground;
    QBrush iconBackground;
    switch (menuMode) {
        case ButtonMenu:
            menuBackgroundRole = QPalette::Window;
            menuForegroundRole = QPalette::WindowText;
            iconBackground = QColor(0, 0, 0, 10);
            break;
        case ItemViewMenu:
            menuBackgroundRole = QPalette::Base;
            menuForegroundRole = QPalette::Text;
            iconBackground = option->palette.brush(QPalette::Active, QPalette::Window);
            break;
    }

    // background
    menuBackground = option->palette.brush(QPalette::Active, menuBackgroundRole);
    painter->fillRect(textRect, menuBackground);
    if (!iconRect.isEmpty()) {
        if (!iconBackground.isOpaque()) {
            painter->fillRect(iconRect, menuBackground);
        }
    }
    painter->fillRect(iconRect, iconBackground);
//    painter->fillRect(iconRect.adjusted(2, 2, -2, -2), QColor(0, 255, 0, 120));

    // separator
    if (option->menuItemType == QStyleOptionMenuItem::Separator) {
        QRect separatorRect;
        switch (separatorMode) {
            case IconSeparator:
                separatorRect = iconRect;
                break;
            case TextSeparator:
                separatorRect = textRect;
                break;
            case ItemSeparator:
                separatorRect = itemRect;
                break;
        }
        // ### make separator margins configurable
        separatorRect.adjust(0, 1, 0, -1);
        if (option->text.isEmpty()) {
            paintThinFrame(painter, separatorRect, option->palette, 60, -20);
        } else {
            const int textFlags = Qt::AlignLeft | Qt::AlignVCenter | Qt::TextDontClip | Qt::TextSingleLine;
            QColor bgcolor = menuBackground.color();
            paintThinFrame(painter, separatorRect, option->palette, -10, -20);
            paintThinFrame(painter, separatorRect.adjusted(1, 1, -1, -1), option->palette, -30, 80, menuBackgroundRole);
            QLinearGradient gradient(separatorRect.topLeft(), separatorRect.bottomLeft());
            gradient.setColorAt(0.0, shaded_color(bgcolor, 90));
            gradient.setColorAt(0.2, shaded_color(bgcolor, 60));
            gradient.setColorAt(0.5, shaded_color(bgcolor, 0));
            gradient.setColorAt(0.51, shaded_color(bgcolor, -10));
            gradient.setColorAt(1.0, shaded_color(bgcolor, -20));
            painter->fillRect(separatorRect.adjusted(1, 1, -1, -1), gradient);
            // ### margins
            separatorRect = remainingHorizontalVisualRect(separatorRect, option, 8);
            style->drawItemText(painter, separatorRect, textFlags, option->palette, true, option->text, menuForegroundRole);
        }
        return;
    }

    // selection background
    painter->save();
    if (option->state & QStyle::State_Selected) {
        QRect selectionRect;
        switch (selectionMode) {
            case IconSelection:
                selectionRect = iconRect;
                break;
            case TextSelection:
                selectionRect = textRect;
                break;
            case ItemSelection:
                selectionRect = itemRect;
                break;
        }
        switch (menuMode) {
            case ButtonMenu: {
                QStyleOptionButton button;
                button.QStyleOption::operator=(*option);
                button.features = QStyleOptionButton::None;
                button.state |= QStyle::State_MouseOver;
                button.rect = selectionRect.adjusted(-1, -1, 1, 1);
                paintCommandButtonPanel(painter, &button, 0);
                menuForegroundRole = QPalette::ButtonText;
                break;
            }
            case ItemViewMenu: {
                QColor color = option->palette.color(QPalette::Active, QPalette::Highlight);
                color.setAlpha(option->state & QStyle::State_Enabled ? 180 : 40);
                painter->fillRect(selectionRect, color);
                if (option->state & QStyle::State_Enabled) {
                    paintThinFrame(painter, selectionRect, option->palette, -20, -20);
                }
                menuForegroundRole = QPalette::HighlightedText;
                break;
            }
        }
        painter->setPen(option->palette.color(QPalette::Active, menuForegroundRole));
    }

    // arrow
    if (option->menuItemType == QStyleOptionMenuItem::SubMenu) {
        const int arrowWidth = option->fontMetrics.height();
        const QRect arrowRect = rightHorizontalVisualRect(textRect, option, arrowWidth);
        QStyleOptionMenuItem opt = *option;
        opt.rect = arrowRect;
        QFont font = painter->font();
        font.setPointSizeF(font.pointSizeF() / 1.19);
        opt.fontMetrics = QFontMetrics(font);
        opt.state &= QStyle::State_Enabled;
        opt.palette.setColor(QPalette::ButtonText, option->palette.color(option->state & QStyle::State_Enabled ? QPalette::Active : QPalette::Disabled, menuForegroundRole));
        style->drawPrimitive((option->direction == Qt::RightToLeft) ? QStyle::PE_IndicatorArrowLeft : QStyle::PE_IndicatorArrowRight, &opt, painter, widget);
    }

    // check
    if (option->checkType != QStyleOptionMenuItem::NotCheckable) {
        QRect checkRect;
        switch (checkColumnMode)
        {
            case UseIconColumn:
                checkRect = iconRect;
                if (option->checked) {
                    // when using the icon colum, we do not show an icon for checked items
                    iconRect = QRect();
                }
                break;
            case NoCheckColumn:
            case ShowCheckColumn:
                checkRect = horizontalVisualRect(textRect, option, checkColumnWidth);
                textRect = remainingHorizontalVisualRect(textRect, option, checkColumnWidth - checkMargin);
                break;
        }
//        painter->fillRect(checkRect.adjusted(2, 2, -2, -2), QColor(0, 0, 255, 120));
        if (option->checked || option->state & QStyle::State_Selected || (showUncheckedIndicator && option->checkType == QStyleOptionMenuItem::NonExclusive)) {
            QStyleOptionMenuItem opt = *option;
            opt.rect = checkRect;
            style->drawPrimitive(QStyle::PE_IndicatorMenuCheckMark, &opt, painter, widget);
        }
    } else if (checkColumnMode == ShowCheckColumn) {
        textRect = remainingHorizontalVisualRect(textRect, option, checkColumnWidth - checkMargin);
    }
//    painter->fillRect(textRect.adjusted(2, 2, -2, -2), QColor(255, 0, 0, 120));

    // text
    if (!option->text.isEmpty()) {
        const int shortcutPos = option->text.indexOf(QChar('\t', 0));
        int textFlags = Qt::AlignLeft | Qt::AlignVCenter | Qt::TextShowMnemonic | Qt::TextDontClip | Qt::TextSingleLine;
        if (!style->styleHint(QStyle::SH_UnderlineShortcut, option, widget)) {
            textFlags |= Qt::TextHideMnemonic;
        }
        if (showShortcut && shortcutPos >= 0) {
            const int shortcutWidth = option->rect.width() * 0.25;// +  option->fontMetrics.width();
            const QRect shortcutRect = rightHorizontalVisualRect(textRect, option, shortcutWidth);
//            painter->fillRect(shortcutRect, QColor(255, 220, 0, 120));
            style->drawItemText(painter, shortcutRect, textFlags, option->palette, option->state & QStyle::State_Enabled, option->text.mid(shortcutPos + 1), menuForegroundRole);
        }
        QFont font = option->font;
        if (option->menuItemType == QStyleOptionMenuItem::DefaultItem) {
            font.setBold(true);
        }
        painter->setFont(font);
        // ### textMargin
        textRect = remainingHorizontalVisualRect(textRect, option, 4);
        style->drawItemText(painter, textRect, textFlags, option->palette, option->state & QStyle::State_Enabled, option->text.left(shortcutPos), menuForegroundRole);
    }

    // icon
    if (iconColumnMode != HideIconColumn && !option->icon.isNull() && !iconRect.isEmpty()) {
        QIcon::Mode mode;
        if (option->state & QStyle::State_Enabled) {
            if (option->state & QStyle::State_Selected) {
                mode = QIcon::Active;
            } else {
                mode = QIcon::Normal;
            }
        } else {
            mode = QIcon::Disabled;
        }
        iconRect = QRect((iconRect.left() + iconRect.right() + 2 - iconSize) >> 1, (iconRect.top() + iconRect.bottom() + 2 - iconSize) >> 1, iconSize, iconSize);
        option->icon.paint(painter, iconRect, Qt::AlignCenter, mode, QIcon::Off);
    }
    painter->restore();
}


/*-----------------------------------------------------------------------*/

void paintMenuTitle(QPainter *painter, const QStyleOptionToolButton *option, const QWidget *widget, const QStyle *style)
{
    const QPalette::ColorRole bgrole = QPalette::Window;
    QColor bgcolor = option->palette.color(bgrole);
    QStyleOptionToolButton opt = *option;
    opt.state &= ~(QStyle::State_Sunken | QStyle::State_On | QStyle::State_Selected | QStyle::State_HasFocus);
    opt.palette.setColor(QPalette::ButtonText, option->palette.color(QPalette::WindowText));
    paintThinFrame(painter, opt.rect, option->palette, -10, -20);
    paintThinFrame(painter, opt.rect.adjusted(1, 1, -1, -1), opt.palette, -30, 80, bgrole);
    QLinearGradient gradient(opt.rect.topLeft(), opt.rect.bottomLeft());
    gradient.setColorAt(0.0, shaded_color(bgcolor, 90));
    gradient.setColorAt(0.2, shaded_color(bgcolor, 60));
    gradient.setColorAt(0.5, shaded_color(bgcolor, 0));
    gradient.setColorAt(0.51, shaded_color(bgcolor, -10));
    gradient.setColorAt(1.0, shaded_color(bgcolor, -20));
    painter->fillRect(opt.rect.adjusted(1, 1, -1, -1), gradient);
    ((QCommonStyle *) style)->QCommonStyle::drawComplexControl(QStyle::CC_ToolButton, &opt, painter, widget);
}


