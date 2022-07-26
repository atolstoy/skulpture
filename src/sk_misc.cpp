/*
 * skulpture_misc.cpp
 *
 */

#include "skulpture_p.h"
#include <QtGui/QPainter>
#include <QtWidgets/QTableView>


/*-----------------------------------------------------------------------*/

extern void paintPanelButtonTool(QPainter *painter, const QStyleOption *option, const QWidget *widget, const QStyle *style);

#if 0
void paintPanelPlacesViewItem(QPainter *painter, const QStyleOptionViewItem *option, const QWidget *widget, const QStyle *style)
{
    QStyleOption opt = *option;
    if (opt.state & QStyle::State_Selected) {
        opt.state |= QStyle::State_On;
    } else if (!(opt.state & QStyle::State_MouseOver)) {
        // draw nothing
        return;
    }
    paintPanelButtonTool(painter, &opt, 0, style);
}
#endif

void paintPanelItemViewItem(QPainter *painter, const QStyleOptionViewItem *option, const QWidget *widget, const QStyle *style)
{
	Q_UNUSED(style);
#if 0
        if (widget && widget->inherits("KFilePlacesView")) {
            paintPanelPlacesViewItem(painter, option, widget, style);
            return;
        }
#endif
	QColor color = option->palette.color(QPalette::Highlight);
	const bool mouse = option->state & QStyle::State_MouseOver && option->state & QStyle::State_Enabled;
        const QTableView *table = qobject_cast<const QTableView *>(widget);
        const bool largePanel = option->rect.height() > 7 + option->fontMetrics.height() && !table;
        QRect panelRect;

        if (largePanel) {
            if (option->version >= 4) {
                switch (option->viewItemPosition) {
                    case QStyleOptionViewItem::Beginning:
                        panelRect = option->rect.adjusted(1, 1, 0, 0);
                        break;
                    case QStyleOptionViewItem::End:
                        panelRect = option->rect.adjusted(0, 1, -1, 0);
                        break;
                    case QStyleOptionViewItem::Middle:
                        panelRect = option->rect.adjusted(0, 1, 0, 0);
                        break;
                    case QStyleOptionViewItem::Invalid:
                    case QStyleOptionViewItem::OnlyOne:
                        panelRect = option->rect.adjusted(1, 1, -1, 0);
                        break;
                }
            } else {
                panelRect = option->rect.adjusted(1, 1, -1, 0);
            }
        } else {
            panelRect = option->rect;
        }

        if (option->version >= 2 && option->features & QStyleOptionViewItem::Alternate) {
		painter->fillRect(panelRect, option->palette.color(QPalette::AlternateBase));
	} else {
		painter->fillRect(panelRect, option->backgroundBrush);
	}
	if (option->state & QStyle::State_Selected) {
		if (mouse) {
			color = color.lighter(110);
		}
	} else if (mouse) {
		color.setAlpha(40);
	} else {
		return;
	}
	painter->save();
	// ### work around KDE widgets that turn on antialiasing
	painter->setRenderHint(QPainter::Antialiasing, false);
	painter->setBrush(Qt::NoBrush);
//	QColor shine(255, 255, 255, panelRect.height() > 20 ? 25 : 10);
	QColor shadow(0, 0, 0, largePanel ? 50 : 20);
	painter->setPen(shadow);
//	painter->setPen(QPen(color.darker(panelRect.height() > 20 ? 150 : 120), 1));
	painter->fillRect(panelRect, color);
	if (table && table->showGrid()) {
		painter->restore();
		return;
	}
	if (option->version >= 4) {
		switch (option->viewItemPosition) {
			case QStyleOptionViewItem::Beginning:
				painter->drawLine(panelRect.topLeft() + QPoint(0, 1), panelRect.bottomLeft() - QPoint(0, 1));
				painter->drawLine(panelRect.topLeft(), panelRect.topRight());
				painter->drawLine(panelRect.bottomLeft(), panelRect.bottomRight());
				break;
			case QStyleOptionViewItem::End:
				painter->drawLine(panelRect.topRight() + QPoint(0, 1), panelRect.bottomRight() - QPoint(0, 1));
				painter->drawLine(panelRect.topLeft(), panelRect.topRight());
				painter->drawLine(panelRect.bottomLeft(), panelRect.bottomRight());
				break;
			case QStyleOptionViewItem::Middle:
				painter->drawLine(panelRect.topLeft(), panelRect.topRight());
				painter->drawLine(panelRect.bottomLeft(), panelRect.bottomRight());
				break;
			case QStyleOptionViewItem::Invalid:
			case QStyleOptionViewItem::OnlyOne:
				painter->drawRect(panelRect.adjusted(0, 0, -1, -1));
				break;
		}
	} else {
		painter->drawRect(panelRect.adjusted(0, 0, -1, -1));
	}
	painter->restore();
}


/*-----------------------------------------------------------------------*/

extern void paintCachedIndicatorBranchChildren(QPainter *painter, const QStyleOption *option);

void paintIndicatorBranch(QPainter *painter, const QStyleOption *option)
{
    QPoint center = option->rect.center() + (option->direction == Qt::LeftToRight ? QPoint(2, 0) : QPoint(-1, 0));

    if (option->state & (QStyle::State_Item | QStyle::State_Sibling)) {
        QColor lineColor = option->palette.color(QPalette::Text);
        lineColor.setAlpha(50);
        painter->fillRect(QRect(center.x(), option->rect.y(), 1, center.y() - option->rect.y()), lineColor);
        if (option->state & QStyle::State_Sibling) {
            painter->fillRect(QRect(center.x(), center.y(), 1, option->rect.bottom() - center.y() + 1), lineColor);
        }
        if (option->state & QStyle::State_Item) {
            if (option->direction == Qt::LeftToRight) {
                painter->fillRect(QRect(center.x() + 1, center.y(), option->rect.right() - center.x(), 1), lineColor);
            } else {
                painter->fillRect(QRect(option->rect.left(), center.y(), center.x() - option->rect.left(), 1), lineColor);
            }
            if (!(option->state & QStyle::State_Sibling)) {
                lineColor.setAlpha(25);
                painter->fillRect(QRect(center.x(), center.y(), 1, 1), lineColor);
            }
        }
    }
    if (option->state & QStyle::State_Children && !(option->state & QStyle::State_Open)) {
        QStyleOption opt = *option;
        static const int d = 9;
        opt.rect = QRect(center.x() - d / 2, center.y() - d / 2, d, d);
        paintCachedIndicatorBranchChildren(painter, &opt);
    }
}


/*-----------------------------------------------------------------------*/
#ifdef QT3_SUPPORT
void paintQ3ListView(QPainter *painter, const QStyleOptionQ3ListView *option, const QWidget *widget,
                     const QStyle *style)
{
    Q_UNUSED(widget); Q_UNUSED(style);

    if (option->subControls & QStyle::SC_Q3ListView) {
        painter->fillRect(option->rect, option->viewportPalette.brush(option->viewportBGRole));
    }
    if (option->subControls & QStyle::SC_Q3ListViewBranch) {
        QStyleOption opt = *((QStyleOption *) option);
        int y = option->rect.y();
        for (int i = 1; i < option->items.size(); ++i) {
            QStyleOptionQ3ListViewItem item = option->items.at(i);
            if (y + item.totalHeight > 0 && y < option->rect.height()) {
                opt.state = QStyle::State_Item | (option->state & QStyle::State_Enabled);
                if (i + 1 < option->items.size()) {
                    opt.state |= QStyle::State_Sibling;
                }
                if (item.features & QStyleOptionQ3ListViewItem::Expandable
                    || (item.childCount > 0 && item.height > 0)) {
                    opt.state |= QStyle::State_Children | (item.state & QStyle::State_Open);
                }
                opt.rect = QRect(option->rect.left(), y, option->rect.width(), item.height);
                paintIndicatorBranch(painter, &opt);

                if (opt.state & QStyle::State_Sibling && item.height < item.totalHeight) {
                    opt.state = QStyle::State_Sibling | (option->state & QStyle::State_Enabled);
                    opt.rect = QRect(option->rect.left(), y + item.height,
                                     option->rect.width(), item.totalHeight - item.height);
                    paintIndicatorBranch(painter, &opt);
                }
            }
            y += item.totalHeight;
        }
    }
}
#endif

/*-----------------------------------------------------------------------*/

void paintSizeGrip(QPainter *painter, const QStyleOption *option)
{
    Qt::Corner corner = Qt::BottomRightCorner;
    if (option->type == QStyleOption::SO_SizeGrip) {
        const QStyleOptionSizeGrip *sizegrip = static_cast<const QStyleOptionSizeGrip *>(option);
        corner = sizegrip->corner;
    }
	QRect r;

	switch (corner) {
		case Qt::TopLeftCorner:		r = option->rect.adjusted(0, 0, 2, 2);	break;
		case Qt::TopRightCorner:		r = option->rect.adjusted(-2, 0, 0, 2);	break;
		case Qt::BottomLeftCorner:	r = option->rect.adjusted(0, -2, 2, 0);	break;
		case Qt::BottomRightCorner:	r = option->rect.adjusted(-2, -2, 0, 0);	break;
	}
	paintThinFrame(painter, r, option->palette, 60, -20);
	paintThinFrame(painter, r.adjusted(1, 1, -1, -1), option->palette, -20, 60);
	switch (corner) {
		case Qt::TopRightCorner:	// for Kickoff 4.1
			painter->save();
			painter->setPen(QPen(shaded_color(option->palette.color(QPalette::Window), 60), 1.0));
			painter->drawLine(r.topLeft(), r.bottomRight());
			painter->setPen(QPen(shaded_color(option->palette.color(QPalette::Window), -20), 1.0));
			painter->drawLine(r.topLeft() + QPoint(1, -1), r.bottomRight() + QPoint(1, -1));
			painter->restore();
			break;
		case Qt::BottomRightCorner:
			painter->save();
			painter->setPen(QPen(shaded_color(option->palette.color(QPalette::Window), -20), 1.0));
			painter->drawLine(r.topRight(), r.bottomLeft());
			painter->setPen(QPen(shaded_color(option->palette.color(QPalette::Window), 60), 1.0));
			painter->drawLine(r.topRight() + QPoint(1, 1), r.bottomLeft() + QPoint(1, 1));
			painter->restore();
			break;
		case Qt::TopLeftCorner:
			// TODO
			break;
		case Qt::BottomLeftCorner:
			// TODO
			break;
	}
}


/*-----------------------------------------------------------------------*/

extern void paintCommandButtonPanel(QPainter *painter, const QStyleOptionButton *option, const QWidget *widget);

void paintToolBoxTabShape(QPainter *painter, const QStyleOptionToolBox *option)
{
	QRect r = option->rect;
	if (option->state & QStyle::State_Selected) {
		QColor color = option->palette.color(QPalette::Window);
		paintThinFrame(painter, r, option->palette, 40, -20);
		paintThinFrame(painter, r.adjusted(1, 1, -1, -1), option->palette, -20, 80);
		QLinearGradient gradient(r.topLeft(), r.bottomLeft());
		gradient.setColorAt(0.0, shaded_color(color, 50));
		gradient.setColorAt(0.2, shaded_color(color, 30));
		gradient.setColorAt(0.5, shaded_color(color, 0));
		gradient.setColorAt(0.51, shaded_color(color, -10));
		gradient.setColorAt(1.0, shaded_color(color, -20));
		painter->fillRect(r.adjusted(1, 1, -1, -1), gradient);
	} else if (option->state & (QStyle::State_Sunken | QStyle::State_MouseOver)) {
		QStyleOptionButton button;
		button.QStyleOption::operator=(*option);
		button.features = QStyleOptionButton::None;
		button.rect.adjust(-1, -1, 1, 1);
		// ### needs QPalette::Window ?
		paintCommandButtonPanel(painter, &button, 0);
        } else if (option->version >= 2 && option->selectedPosition == QStyleOptionToolBox::PreviousIsSelected) {
		r.setHeight(2);
		paintThinFrame(painter, r, option->palette, 60, -20);
	} else {
	//	r.setHeight(2);
	//	painter->fillRect(r, option->palette.color(QPalette::Window));
	}
	QStyleOption indicator;
	indicator = *option;
	indicator.rect.setSize(QSize(11, 11));
	indicator.rect.translate(0, (option->rect.height() - 11) >> 1);
	indicator.state = QStyle::State_Children | (option->state & QStyle::State_Enabled);
	if (option->state & QStyle::State_Selected) {
		indicator.state |= QStyle::State_Open;
	}
	paintIndicatorBranch(painter, &indicator);
}


void paintToolBoxTabLabel(QPainter *painter, const QStyleOptionToolBox *option, const QWidget *widget, const QStyle *style)
{
    QStyleOptionToolBox opt;

    if (option->version >= 2) {
        opt = *((const QStyleOptionToolBox *) option);
    } else {
        opt = *option;
    }
    if ((option->state & QStyle::State_Selected) || !(option->state & (QStyle::State_Sunken | QStyle::State_MouseOver))) {
        opt.palette.setColor(QPalette::ButtonText, opt.palette.color(QPalette::WindowText));
    }
    ((QCommonStyle *) style)->QCommonStyle::drawControl(QStyle::CE_ToolBoxTabLabel, &opt, painter, widget);
}


/*-----------------------------------------------------------------------*/

extern void paintCachedGrip(QPainter *painter, const QStyleOption *option, QPalette::ColorRole bgrole);

void paintSplitter(QPainter *painter, const QStyleOption *option)
{
	if (option->state & QStyle::State_Enabled && option->state & QStyle::State_MouseOver) {
		painter->fillRect(option->rect, QColor(255, 255, 255, 60));
	}
	int d = 5;
	QRect rect(QRect(option->rect).center() - QPoint(d / 2, d / 2), QSize(d, d));
	QStyleOption iOption = *option;
	iOption.rect = rect;
	iOption.palette.setCurrentColorGroup(QPalette::Disabled);
//	iOption.state &= ~QStyle::State_Enabled;
	iOption.palette.setColor(QPalette::Button, iOption.palette.color(QPalette::Window));
	paintCachedGrip(painter, &iOption, QPalette::Window);
}


/*-----------------------------------------------------------------------*/

void paintRubberBand(QPainter *painter, const QStyleOptionRubberBand *option)
{
	painter->save();
	if (true || option->shape == QRubberBand::Rectangle) {
		QColor color = option->palette.color(QPalette::Highlight);
		color.setAlphaF(0.2 * color.alphaF());
		painter->fillRect(option->rect, color);
		color = option->palette.color(QPalette::Highlight);
		color.setAlphaF(0.8 * color.alphaF());
		painter->setPen(QPen(color /*, 1.0, Qt::DotLine*/));
		painter->drawRect(option->rect.adjusted(0, 0, -1, -1));
	} else {
	//	painter->fillRect(option->rect, Qt::green);
	}
	painter->restore();
}


int getRubberBandMask(QStyleHintReturnMask *mask, const QStyleOption *option, const QWidget *widget)
{
	static const int rubber_width = 4;
	int r = rubber_width;

	Q_UNUSED(widget);
	mask->region = option->rect;
	if (option->rect.width() > 2 * r && option->rect.height() > 2 * r) {
		mask->region -= option->rect.adjusted(r, r, -r, -r);
	}
	return 1;
}


