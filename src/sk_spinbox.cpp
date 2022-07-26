/*
 * skulpture_spinbox.cpp
 *
 */

#include "skulpture_p.h"
#include <QtWidgets/QAbstractSpinBox>
#include <QtWidgets/QApplication>
#include <QtGui/QPainter>


/*-----------------------------------------------------------------------*/

extern void paintScrollArrow(QPainter *painter, const QStyleOption *option, Qt::ArrowType arrow, bool spin);

void paintIndicatorSpinDown(QPainter *painter, const QStyleOption *option)
{
	paintScrollArrow(painter, option, Qt::DownArrow, true);
}


void paintIndicatorSpinUp(QPainter *painter, const QStyleOption *option)
{
	paintScrollArrow(painter, option, Qt::UpArrow, true);
}


void paintIndicatorSpinMinus(QPainter *painter, const QStyleOption *option)
{
    paintScrollArrow(painter, option, Qt::LeftArrow, true);
}


void paintIndicatorSpinPlus(QPainter *painter, const QStyleOption *option)
{
    paintScrollArrow(painter, option, Qt::RightArrow, true);
}


/*-----------------------------------------------------------------------*/

QRect subControlRectSpinBox(const QStyleOptionSpinBox *option, QStyle::SubControl subControl, const QWidget *widget, const QStyle *style)
{
    int fw = option->frame ? style->pixelMetric(QStyle::PM_SpinBoxFrameWidth, option, widget) : 0;
    int bw;
    if (option->buttonSymbols == QAbstractSpinBox::NoButtons) {
        bw = 0;
    } else
    {
        bw = qMax(style->pixelMetric(QStyle::PM_ScrollBarExtent, option, widget), QSize(0,0).width());
    }
    bool strutMode = QSize(0,0).height() > (option->rect.height() >> 1);
    QRect rect;

    switch (subControl) {
        case QStyle::SC_SpinBoxUp:
        case QStyle::SC_SpinBoxDown: {
            int h = option->rect.height() - 2 * fw;
            int t = option->rect.top() + fw;
            int l = option->rect.right() - bw - fw + 1;
            if (strutMode) {
                if (subControl == QStyle::SC_SpinBoxUp) {
                    l -= bw;
                }
            } else {
                if (subControl == QStyle::SC_SpinBoxDown) {
                    t += (h >> 1);
                }
                h = (h + 1) >> 1;
            }
            rect = QRect(l, t, bw, h);
            break;
        }
        case QStyle::SC_SpinBoxEditField: {
            if (strutMode) {
                bw *= 2;
            }
            rect = option->rect.adjusted(fw, fw, -fw - bw, -fw);
            break;
        }
        case QStyle::SC_SpinBoxFrame:
        default: // avoid warning
            rect = option->rect;
            break;
    }
    return style->visualRect(option->direction, option->rect, rect);
}


/*-----------------------------------------------------------------------*/

void paintComplexControlArea(QPainter *painter, const QStyleOption *option)
{
    // configuration
    const bool paintBackground = true;
    const bool paintSeparator = true;

    // background
    QColor color;
    if (paintBackground && option->state & QStyle::State_Enabled) {
        color = option->palette.color(QPalette::Window);
        // ### should arrow areas have hover highlight?
        if (false && option->state & QStyle::State_MouseOver) {
            color = color.lighter(110);
        } else {
            color = color.lighter(107);
        }
    } else {
        color = option->palette.color(QPalette::Base);
    }
    painter->fillRect(option->rect, color);

    // separator
    if (paintSeparator) {
        QRect rect = option->rect;
        if (option->direction == Qt::LeftToRight) {
            rect.setWidth(1);
        } else {
            rect.setLeft(rect.left() + rect.width() - 1);
        }
        painter->fillRect(rect, shaded_color(option->palette.color(QPalette::Window), -5));
    }
}


static void paintSpinBoxUpDown(QPainter *painter, const QStyleOptionSpinBox *option, QStyle::SubControl subControl, const QWidget *widget, const QStyle *style)
{
    QStyleOption opt;
    opt.QStyleOption::operator=(*option);
    opt.rect = style->subControlRect(QStyle::CC_SpinBox, option, subControl, widget);
    if (!(option->activeSubControls & subControl)) {
        opt.state &= ~(QStyle::State_Sunken | QStyle::State_On | QStyle::State_MouseOver);
    }

    // button background
    paintComplexControlArea(painter, &opt);

    // button symbol
    QStyle::PrimitiveElement pe;
    if (!(option->stepEnabled & (subControl == QStyle::SC_SpinBoxUp ? QAbstractSpinBox::StepUpEnabled : QAbstractSpinBox::StepDownEnabled))) {
        opt.state &= ~(QStyle::State_Enabled | QStyle::State_MouseOver);
        opt.palette.setCurrentColorGroup(QPalette::Disabled);
    }
    // micro adjustments
    if (subControl == QStyle::SC_SpinBoxUp) {
        opt.rect.translate(0, 1);
    } else if (opt.rect.height() & 1) {
        opt.rect.translate(0, -1);
    }
    switch (option->buttonSymbols) {
        case QAbstractSpinBox::PlusMinus:
            pe = subControl == QStyle::SC_SpinBoxUp ? QStyle::PE_IndicatorSpinPlus : QStyle::PE_IndicatorSpinMinus;
            break;
        default:
            pe = subControl == QStyle::SC_SpinBoxUp ? QStyle::PE_IndicatorSpinUp : QStyle::PE_IndicatorSpinDown;
            break;
    }
    style->drawPrimitive(pe, &opt, painter, widget);
}


void paintSpinBox(QPainter *painter, const QStyleOptionSpinBox *option, const QWidget *widget, const QStyle *style)
{
    // up/down controls
    if (option->buttonSymbols != QAbstractSpinBox::NoButtons)
    {
        for (uint sc = QStyle::SC_SpinBoxUp; sc != QStyle::SC_SpinBoxFrame; sc <<= 1) {
            if (option->subControls & sc) {
                paintSpinBoxUpDown(painter, option, (QStyle::SubControl) sc, widget, style);
            }
        }
    }

    // frame
    if (option->frame && (option->subControls & QStyle::SC_SpinBoxFrame)) {
        QStyleOptionFrame frameOpt;
        frameOpt.QStyleOption::operator=(*option);
        frameOpt.rect = style->subControlRect(QStyle::CC_SpinBox, option, QStyle::SC_SpinBoxFrame, widget);
        frameOpt.state |= QStyle::State_Sunken;
        frameOpt.lineWidth = style->pixelMetric(QStyle::PM_SpinBoxFrameWidth, &frameOpt, widget);
        frameOpt.midLineWidth = 0;
        style->drawPrimitive(QStyle::PE_FrameLineEdit, &frameOpt, painter, widget);
    }
}


