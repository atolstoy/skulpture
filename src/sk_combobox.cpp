/*
 * skulpture_combobox.cpp
 *
 */

#include "skulpture_p.h"
#include <QtGui/QPainter>
#include <QtWidgets/QApplication>


/*-----------------------------------------------------------------------*/

extern void paintComplexControlArea(QPainter *painter, const QStyleOption *option);

void paintComboBox(QPainter *painter, const QStyleOptionComboBox *option, const QWidget *widget, const QStyle *style)
{
    QStyleOptionComboBox opt = *option;
    const bool buttonMode = false; //!option->editable;
    QRect rect = style->subControlRect(QStyle::CC_ComboBox, option, QStyle::SC_ComboBoxArrow, widget);
    if (option->subControls & (QStyle::SC_ComboBoxFrame | QStyle::SC_ComboBoxEditField)) {
        if (buttonMode) {
            QStyleOptionButton buttonOption;
            buttonOption.QStyleOption::operator=(opt);
            if (buttonOption.state & QStyle::State_On) {
                buttonOption.state |= QStyle::State_Sunken;
            } else {
                buttonOption.state &= ~QStyle::State_Sunken;
            }
            buttonOption.state &= ~QStyle::State_On;

            // separator position
            opt.rect = rect;
            if (option->direction == Qt::LeftToRight) {
                opt.rect.setWidth(1);
            } else {
                opt.rect.setLeft(rect.left() + rect.width() - 1);
            }
            if (option->frame) {
                style->drawPrimitive(QStyle::PE_PanelButtonCommand, &buttonOption, painter, widget);
                QColor color = option->palette.color(QPalette::Button);
                if (!(opt.state & QStyle::State_On)) {
                    opt.rect.translate(option->direction == Qt::LeftToRight ? -1 : 1, 0);
                }
                painter->fillRect(opt.rect, shaded_color(color, option->state & QStyle::State_Enabled ? -30 : -15));
                if (option->state & QStyle::State_Enabled) {
                    opt.rect.translate(option->direction == Qt::LeftToRight ? 1 : -1, 0);
                    painter->fillRect(opt.rect, shaded_color(color, 80));
                }
            } else {
                QColor bg = option->palette.color(QPalette::Button);
                painter->fillRect(option->rect, bg);
                painter->fillRect(opt.rect, shaded_color(bg, -15));
            }
        } else {
            int fw = option->frame ? style->pixelMetric(QStyle::PM_ComboBoxFrameWidth, option, widget) : 0;
            QColor color = option->palette.color(QPalette::Base);
            if (option->state & QStyle::State_Enabled) {
                if (option->state & QStyle::State_HasFocus && option->editable) {
                    color = blend_color(color, option->palette.color(QPalette::Highlight), 0.15);
                } else if (option->state & QStyle::State_MouseOver /*&& !option->editable*/) {
                    color = color.lighter(103);
                }
            }
            QRect edit = style->subControlRect(QStyle::CC_ComboBox, option, QStyle::SC_ComboBoxFrame, widget).adjusted(fw, fw, -fw, -fw);
            painter->fillRect(edit, color);
            if (false && option->state & QStyle::State_Enabled && option->rect.height() <= 64) {
                QLinearGradient panelGradient(option->rect.topLeft(), option->rect.bottomLeft());
                if (color.valueF() > 0.9) {
                    panelGradient.setColorAt(0.0, shaded_color(color, -20));
                }
                panelGradient.setColorAt(0.6, shaded_color(color, 0));
                panelGradient.setColorAt(1.0, shaded_color(color, 10));
                painter->fillRect(edit, panelGradient);
            }
            opt.rect = rect;
            if (!(option->activeSubControls & QStyle::SC_ComboBoxArrow)) {
                opt.state &= ~QStyle::State_MouseOver;
            }
            paintComplexControlArea(painter, &opt);
            if (option->subControls & QStyle::SC_ComboBoxFrame && option->frame) {
                QStyleOptionFrame frameOpt;
                frameOpt.QStyleOption::operator=(*option);
                frameOpt.rect = style->subControlRect(QStyle::CC_ComboBox, option, QStyle::SC_ComboBoxFrame, widget);
                frameOpt.state |= QStyle::State_Sunken;
                frameOpt.lineWidth = fw;
                frameOpt.midLineWidth = 0;
                style->drawPrimitive(QStyle::PE_FrameLineEdit, &frameOpt, painter, widget);
            }
        }
    }

    // arrow
    if (option->subControls & (QStyle::SC_ComboBoxArrow)) {
        opt.rect = rect;
        opt.state &= /*QStyle::State_MouseOver |*/ QStyle::State_Enabled;
        if (buttonMode) {
            opt.state &= ~QStyle::State_MouseOver;
            if (option->state & QStyle::State_On) {
                int sx = style->pixelMetric(QStyle::PM_ButtonShiftHorizontal, option, widget);
                int sy = style->pixelMetric(QStyle::PM_ButtonShiftVertical, option, widget);
                opt.rect.adjust(sx, sy, sx, sy);
            }
        } else {
            opt.palette.setColor(QPalette::ButtonText, opt.palette.color(option->state & QStyle::State_Enabled ? QPalette::WindowText : QPalette::Text));
        }
        style->drawPrimitive(QStyle::PE_IndicatorArrowDown, &opt, painter, widget);
    }

    // focus frame
    if ((option->state & QStyle::State_HasFocus) && !option->editable) {
        QStyleOptionFocusRect focus;
        focus.QStyleOption::operator=(*option);
        focus.rect = style->subElementRect(QStyle::SE_ComboBoxFocusRect, option, widget);
        focus.state |= QStyle::State_FocusAtBorder;
        focus.backgroundColor = option->palette.color(buttonMode ? QPalette::Button : QPalette::Base);
        style->drawPrimitive(QStyle::PE_FrameFocusRect, &focus, painter, widget);
    }
}


void paintComboBoxLabel(QPainter *painter, const QStyleOptionComboBox *option, const QWidget *widget, const QStyle *style)
{
    QStyleOptionComboBox opt = *option;
    const bool buttonMode = false; //!option->editable;
    if (!buttonMode) {
        opt.palette.setColor(QPalette::Base, QColor(0, 0, 0, 0));
    } else {
        painter->save();
        painter->setPen(opt.palette.color(QPalette::ButtonText));
        if (opt.state & QStyle::State_On) {
            int sx = style->pixelMetric(QStyle::PM_ButtonShiftHorizontal, option, widget);
            int sy = style->pixelMetric(QStyle::PM_ButtonShiftVertical, option, widget);
            opt.rect.adjust(sx, sy, sx, sy);
        }
    }
    ((const QCommonStyle *) style)->QCommonStyle::drawControl(QStyle::CE_ComboBoxLabel, &opt, painter, widget);
    if (buttonMode) {
        painter->restore();
    }
}


/*-----------------------------------------------------------------------*/

QRect subControlRectComboBox(const QStyleOptionComboBox *option, QStyle::SubControl subControl, const QWidget *widget, const QStyle *style)
{
    int fw = option->frame ? style->pixelMetric(QStyle::PM_ComboBoxFrameWidth, option, widget) : 0;
    int bw = style->pixelMetric(QStyle::PM_ScrollBarExtent, option, widget);
    if (option->editable) bw = qMax(bw, QSize(0,0).width());
    QRect rect;

    switch (subControl) {
        case QStyle::SC_ComboBoxArrow:
            rect = QRect(option->rect.right() - bw - fw + 1, option->rect.top() + fw, bw, option->rect.height() - 2 * fw);
            break;
        case QStyle::SC_ComboBoxEditField: {
            if (option->editable) {
                rect = option->rect.adjusted(fw, fw, -fw - bw, -fw);
            } else {
                rect = option->rect.adjusted(fw + 4, fw, -fw - bw - 4, -fw);
            }
            break;
        }
        case QStyle::SC_ComboBoxFrame:
        default: // avoid warning
            rect = option->rect;
            break;
    }
    return style->visualRect(option->direction, option->rect, rect);
}


QRect subElementRectComboBoxFocusRect(const QStyleOptionComboBox *option, const QWidget *widget, const QStyle *style)
{
    int fw = option->frame ? (option->editable ? style->pixelMetric(QStyle::PM_ComboBoxFrameWidth, option, widget) : 4) : 2;
    int bw = true || option->editable ? qMax(style->pixelMetric(QStyle::PM_ScrollBarExtent, option, widget), QSize(0,0).width()) : 0;

    return style->visualRect(option->direction, option->rect, option->rect.adjusted(fw, fw, -fw - bw, -fw));
}


