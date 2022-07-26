/*
 * skulpture_scrollbar.cpp
 *
 */

#include "skulpture_p.h"
#include <QtGui/QPainter>
#include <QtWidgets/QAbstractScrollArea>
#include <QtWidgets/QApplication>
#include <climits>


/*-----------------------------------------------------------------------*/

extern void paintScrollArrow(QPainter *painter, const QStyleOption *option, Qt::ArrowType arrow, bool spin);

void paintScrollArea(QPainter *painter, const QStyleOption *option)
{
	QColor color = option->palette.color(QPalette::Disabled, QPalette::Window);
	if (option->state & QStyle::State_Enabled || option->type != QStyleOption::SO_Slider) {
#if 0
		if (option->state & QStyle::State_Sunken) {
			color = color.lighter(107);
		} else {
			color = color.darker(107);
		}
#elif 0
		color = option->palette.color(QPalette::Base);
#elif 1
		if (option->state & QStyle::State_Sunken) {
			color = color.darker(107);
		} else {
			// ###
			if (false && option->state & QStyle::State_MouseOver) {
				color = color.lighter(110);
			} else {
				color = color.lighter(107);
			}
		}
#endif
	}
	painter->fillRect(option->rect, color);
//	painter->fillRect(option->rect, Qt::red);
}


void paintScrollAreaCorner(QPainter *painter, const QStyleOption *option, const QWidget *widget, const QStyle */*style*/)
{
	QStyleOption opt;
	opt = *option;
	opt.type = QStyleOption::SO_Default;
	if (qobject_cast<const QAbstractScrollArea *>(widget)) {
            // ### work around bug in Qt 4.5
            if (option->rect.y() + option->rect.height() > widget->rect().height()
             || option->rect.x() + option->rect.width() > widget->rect().width()) {
                return;
            }
		opt.type = QStyleOption::SO_Slider;
		opt.state &= ~QStyle::State_Enabled;
		if (widget->isEnabled()) {
			opt.state |= QStyle::State_Enabled;
		}
	}
	paintScrollArea(painter, &opt);
}


extern void paintSliderGroove(QPainter *painter, QRect &rect, const QStyleOptionSlider *option);

void paintScrollBarPage(QPainter *painter, const QStyleOptionSlider *option)
{
    const bool paintGroove = true;

    paintScrollArea(painter, option);
    if (paintGroove) {
        QRect rect = option->rect.adjusted(1, 1, -1, -1);
        paintSliderGroove(painter, rect, option);
    }
}


void paintScrollBarAddLine(QPainter *painter, const QStyleOptionSlider *option)
{
	paintScrollArea(painter, option);
//	paintThinFrame(painter, option->rect, option->palette, -40, 120);
	if (option->minimum != option->maximum) {
		QStyleOptionSlider opt = *option;
		opt.fontMetrics = QApplication::fontMetrics();
                opt.palette.setColor(QPalette::ButtonText, opt.palette.color(QPalette::WindowText));
		paintScrollArrow(painter, &opt, option->orientation == Qt::Horizontal ? (option->direction == Qt::LeftToRight ? Qt::RightArrow : Qt::LeftArrow) : Qt::DownArrow, false);
	}
}


void paintScrollBarSubLine(QPainter *painter, const QStyleOptionSlider *option)
{
	paintScrollArea(painter, option);
//	paintThinFrame(painter, option->rect, option->palette, -40, 120);
	if (option->minimum != option->maximum) {
		QStyleOptionSlider opt = *option;
		opt.fontMetrics = QApplication::fontMetrics();
                opt.palette.setColor(QPalette::ButtonText, opt.palette.color(QPalette::WindowText));
                paintScrollArrow(painter, &opt, option->orientation == Qt::Horizontal ? (option->direction == Qt::LeftToRight ? Qt::LeftArrow : Qt::RightArrow) : Qt::UpArrow, false);
	}
}


void paintScrollBarFirst(QPainter *painter, const QStyleOptionSlider *option)
{
    paintScrollBarSubLine(painter, option);
    if (option->minimum != option->maximum) {
        painter->fillRect(option->rect.adjusted(2, 2, -2, -2), option->palette.color(QPalette::WindowText));
    }
}


void paintScrollBarLast(QPainter *painter, const QStyleOptionSlider *option)
{
    paintScrollBarAddLine(painter, option);
    if (option->minimum != option->maximum) {
        painter->fillRect(option->rect.adjusted(2, 2, -2, -2), option->palette.color(QPalette::WindowText));
    }
}


/*-----------------------------------------------------------------------*/

class ComplexControlLayout
{
    public:
        struct SubControlItem {
            QStyle::SubControl subControl;
            QStyle::ControlElement controlElement;
            char layoutSpec;
        };

    protected:
        ComplexControlLayout(const SubControlItem *controlItem, uint controlCount,
            const QStyleOptionComplex *opt, const QWidget *w = 0, const QStyle *s = 0)
            : subControlItem(controlItem), subControlCount(controlCount),
            option(opt), widget(w), style(s), layoutCount(0)
        {
            /* */
        }
        ~ComplexControlLayout() { }

    public:
        QStyle::SubControl hitTestComplexControl(const QPoint &position) const;
        QRect subControlRect(QStyle::SubControl subControl) const;
        void paintComplexControl(QPainter *painter) const;

    protected:
        struct LayoutItem {
            QStyle::SubControl subControl;
            QRect rect;
        };

    protected:
        void addLayoutItem(QStyle::SubControl subControl, const QRect &rect);

    protected:
        static const uint maxLayoutCount = 16;

    protected:
        const SubControlItem * const subControlItem;
        const uint subControlCount;
        const QStyleOptionComplex * const option;
        const QWidget * const widget;
        const QStyle * const style;
        uint layoutCount;
        LayoutItem layoutItem[maxLayoutCount];
};


/*-----------------------------------------------------------------------*/

void ComplexControlLayout::addLayoutItem(QStyle::SubControl subControl, const QRect &rect)
{
    if (layoutCount < maxLayoutCount) {
        layoutItem[layoutCount].subControl = subControl;
        layoutItem[layoutCount].rect = style->visualRect(option->direction, option->rect, rect);
        ++layoutCount;
    }
}


QStyle::SubControl ComplexControlLayout::hitTestComplexControl(const QPoint &position) const
{
    for (uint i = 0; i < subControlCount; ++i) {
        for (uint j = 0; j < layoutCount; ++j) {
            if (layoutItem[j].subControl == subControlItem[i].subControl) {
                if (layoutItem[j].rect.contains(position)) {
                    return layoutItem[j].subControl;
                }
            }
        }
    }
    return QStyle::SC_None;
}


QRect ComplexControlLayout::subControlRect(QStyle::SubControl subControl) const
{
    QRect rect;
    for (uint i = 0; i < layoutCount; ++i) {
        if (layoutItem[i].subControl == subControl) {
            rect |= layoutItem[i].rect;
        }
    }
    return rect;
}


void ComplexControlLayout::paintComplexControl(QPainter *painter) const
{
    for (int i = subControlCount; --i >= 0; ) {
        if (subControlItem[i].controlElement != QStyle::CE_CustomBase
        && option->subControls & subControlItem[i].subControl) {
            for (uint j = 0; j < layoutCount; ++j) {
                if (layoutItem[j].subControl == subControlItem[i].subControl) {
                    QStyleOptionSlider opt = *static_cast<const QStyleOptionSlider *>(ComplexControlLayout::option);
                    opt.rect = layoutItem[j].rect;
                    if (!(option->activeSubControls & subControlItem[i].subControl)) {
                        opt.state &= ~(QStyle::State_Sunken | QStyle::State_MouseOver);
                    }
                    style->drawControl(subControlItem[i].controlElement, &opt, painter, widget);
                }
            }
        }
    }
}


/*-----------------------------------------------------------------------*/

static const ComplexControlLayout::SubControlItem scrollBarSubControlItem[] = {
    /* hitTest in forward order, paint in reverse order */
    { QStyle::SC_ScrollBarSlider, QStyle::CE_ScrollBarSlider, '*' },
    { QStyle::SC_ScrollBarAddLine, QStyle::CE_ScrollBarAddLine, '>' },
    { QStyle::SC_ScrollBarSubLine, QStyle::CE_ScrollBarSubLine, '<' },
    { QStyle::SC_ScrollBarFirst, QStyle::CE_ScrollBarFirst, '[' },
    { QStyle::SC_ScrollBarLast, QStyle::CE_ScrollBarLast, ']' },
    { QStyle::SC_ScrollBarAddPage, QStyle::CE_ScrollBarAddPage, ')' },
    { QStyle::SC_ScrollBarSubPage, QStyle::CE_ScrollBarSubPage, '(' },
    { QStyle::SC_ScrollBarGroove, QStyle::CE_CustomBase, '#' }
};


class ScrollBarLayout : public ComplexControlLayout
{
    public:
        ScrollBarLayout(const QStyleOptionSlider *opt, const QWidget *w = 0, const QStyle *s = 0)
            : ComplexControlLayout(scrollBarSubControlItem, array_elements(scrollBarSubControlItem), opt, w, s)
        {
            /* */
        }
        ~ScrollBarLayout() { }

    public:
        void initLayout(const char *layoutSpec);
        void initLayout(ArrowPlacementMode placementMode);

    protected:
        void addLayoutItem(const char layoutSpec, int pos, int size);
};


/*-----------------------------------------------------------------------*/

void ScrollBarLayout::addLayoutItem(char c, int pos, int size)
{
    const QStyleOptionSlider *option = static_cast<const QStyleOptionSlider *>(ComplexControlLayout::option);

    if (size > 0) {
        for (uint i = 0; i < subControlCount; ++i) {
            if (subControlItem[i].layoutSpec == c) {
                QRect rect;
                if (option->orientation == Qt::Horizontal) {
                    rect = QRect(option->rect.left() + pos, option->rect.top(), size, option->rect.height());
                } else {
                    rect = QRect(option->rect.left(), option->rect.top() + pos, option->rect.width(), size);
                }
                ComplexControlLayout::addLayoutItem(subControlItem[i].subControl, rect);
                return;
            }
        }
    }
}


void ScrollBarLayout::initLayout(ArrowPlacementMode placementMode)
{
    static const char *layoutSpec[] = {
        "(*)", // NoArrowsMode
        "(<*>)", // SkulptureMode
        "<(*)>", // WindowsMode
        "<(*)<>", // KDEMode
        "(*)<>", // PlatinumMode
        "<>(*)" // NextMode
    };
    initLayout(layoutSpec[uint(placementMode)]);
}


void ScrollBarLayout::initLayout(const char *layoutSpec)
{
    const QStyleOptionSlider *option = static_cast<const QStyleOptionSlider *>(ComplexControlLayout::option);
    uint range = option->maximum - option->minimum;
    int pos = option->orientation == Qt::Horizontal ? option->rect.left() : option->rect.top();
    int maxSize = option->orientation == Qt::Horizontal ? option->rect.width() : option->rect.height();
    int endPos = pos + maxSize;
    int groovePos = pos, grooveSize = maxSize;
    int pagePos = pos, pageSize = maxSize;
    int buttonSize = style->pixelMetric(QStyle::PM_ScrollBarExtent, option, widget);
    buttonSize = qMin(maxSize >> 1, buttonSize);

    if (qstrcmp(layoutSpec, "(*)")) {
        if (!qstrcmp(layoutSpec, "<(*)<>")) {
            if (maxSize < 4 * buttonSize) {
                layoutSpec = "<(*)>";
            }
        }
        if (maxSize < 3 * buttonSize) {
            layoutSpec = "(<*>)";
        }
    }

    if (layoutSpec && range != 0) {
        // layout items before slider
        const char *p = layoutSpec;
        char c;
        while ((c = *p)) {
            if (c == '*') {
                pagePos = pos;
                break;
            } else if (c == '(') {
                groovePos = pos;
            } else {
                addLayoutItem(c, pos, buttonSize);
                pos += buttonSize;
            }
            ++p;
        }

        // layout items after slider
        while (*p) {
            ++p;
        }
        --p;
        pos = endPos;
        while (p >= layoutSpec) {
            c = *p;
            if (c == '*') {
                pageSize = pos - pagePos;
                break;
            } else if (c == ')') {
                grooveSize = pos - groovePos;
            } else {
                pos -= buttonSize;
                addLayoutItem(c, pos, buttonSize);
            }
            --p;
        }
    }
    if (layoutCount > maxLayoutCount - 4) {
        layoutCount = maxLayoutCount - 4;
    }
    if (range != 0) {
        int sliderSize = (qint64(option->pageStep) * grooveSize) / (range + option->pageStep);
        int sliderMin = style->pixelMetric(QStyle::PM_ScrollBarSliderMin, option, widget);

        if (sliderMin > grooveSize >> 1) {
            sliderMin = grooveSize >> 1;
            if (sliderSize > sliderMin) {
                sliderSize = sliderMin;
            }
        }
        if (sliderSize < sliderMin || range > INT_MAX / 2) {
            sliderSize = sliderMin;
        }
        if (grooveSize != pageSize) {
            if (sliderSize > grooveSize - buttonSize) {
                sliderSize = grooveSize - buttonSize;
            }
        }
        int sliderPos = groovePos + style->sliderPositionFromValue(option->minimum, option->maximum, option->sliderPosition, grooveSize - sliderSize, option->upsideDown);

        addLayoutItem('(', pagePos, sliderPos - pagePos);
        addLayoutItem(')', sliderPos + sliderSize, (pagePos + pageSize) - (sliderPos + sliderSize));
        addLayoutItem('*', sliderPos, sliderSize);
    } else {
        addLayoutItem('*', groovePos, grooveSize);
    }
    addLayoutItem('#', groovePos, grooveSize);
}


/*-----------------------------------------------------------------------*/

extern void paintCachedGrip(QPainter *painter, const QStyleOption *option, QPalette::ColorRole bgrole = QPalette::Window);
extern void paintSliderHandle(QPainter *painter, const QRect &rect, const QStyleOptionSlider *option);


void paintScrollBarSlider(QPainter *painter, const QStyleOptionSlider *option)
{
    if (option->minimum == option->maximum) {
        paintScrollArea(painter, option);
    } else {
        paintSliderHandle(painter, option->rect, option);
    }
}


/*-----------------------------------------------------------------------*/

void paintScrollBar(QPainter *painter, const QStyleOptionSlider *option, const QWidget *widget, const QStyle *style, ArrowPlacementMode horizontalArrowMode, ArrowPlacementMode verticalArrowMode)
{
    // paint scrollbar
    ScrollBarLayout layout(option, widget, style);
    layout.initLayout(option->orientation == Qt::Horizontal ? horizontalArrowMode : verticalArrowMode);
    layout.paintComplexControl(painter);

    // get frame this scrollbar is in
    QFrame *frame = 0;
    if (widget && widget->parentWidget()) {
        QWidget *container = widget->parentWidget();
        if (container->inherits("Q3ListView")) {
            if (option->orientation == Qt::Vertical) {
                frame = qobject_cast<QFrame *>(container);
            }
        } else if (container->inherits("Q3Table")) {
            frame = qobject_cast<QFrame *>(container);
        } else if (container->parentWidget()) {
            frame = qobject_cast<QAbstractScrollArea *>(container->parentWidget());
        }
    }

    // paint shadow
    if (frame && frame->frameStyle() == (QFrame::StyledPanel | QFrame::Sunken)) {
        QRect rect = option->rect;
        if (option->orientation == Qt::Vertical) {
            if (option->direction == Qt::LeftToRight) {
                rect.adjust(-3, 0, 1, 0);
            } else {
                rect.adjust(-1, 0, 2, 0);
            }
            // ### temporary hack to detect corner widget
            if (widget->height() == frame->height() - 4) {
                rect.adjust(0, -1, 0, 1);
            } else {
                rect.adjust(0, -1, 0, 4);
            }
        } else {
            rect.adjust(0, -3, 0, 1);
            // ### temporary hack to detect corner widget
            if (widget->width() == frame->width() - 4) {
                rect.adjust(-1, 0, 1, 0);
            } else {
                if (option->direction == Qt::LeftToRight) {
                    rect.adjust(-1, 0, 4, 0);
                } else {
                    rect.adjust(-4, 0, 1, 0);
                }
            }
        }
        paintRecessedFrameShadow(painter, rect.adjusted(1, 1, -1, -1), RF_Small);
    }
}


QRect subControlRectScrollBar(const QStyleOptionSlider *option, QStyle::SubControl subControl, const QWidget *widget, const QStyle *style, ArrowPlacementMode horizontalArrowMode, ArrowPlacementMode verticalArrowMode)
{
    ScrollBarLayout layout(option, widget, style);
    layout.initLayout(option->orientation == Qt::Horizontal ? horizontalArrowMode : verticalArrowMode);
    return layout.subControlRect(subControl);
}


QStyle::SubControl hitTestComplexControlScrollBar(const QStyleOptionSlider *option, const QPoint &position, const QWidget *widget, const QStyle *style, ArrowPlacementMode horizontalArrowMode, ArrowPlacementMode verticalArrowMode)
{
    ScrollBarLayout layout(option, widget, style);
    layout.initLayout(option->orientation == Qt::Horizontal ? horizontalArrowMode : verticalArrowMode);
    return layout.hitTestComplexControl(position);
}


