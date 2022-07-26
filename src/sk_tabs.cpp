/*
 * skulpture_tabs.cpp
 *
 */

#include "skulpture_p.h"
#include <QtGui/QPainter>
#include <QtWidgets/QTabWidget>


/*-----------------------------------------------------------------------*/

enum Pos { North, South, West, East };

static inline Pos tabPos(QTabBar::Shape shape)
{
	return Pos(int(shape) & 3);
}

static inline bool isVertical(QTabBar::Shape shape)
{
	return (int(shape) & 2);
}

struct Affinity {
	int x1, y1, x2, y2;
};

static inline void tabAffinity(QTabBar::Shape shape, Affinity &affinity, int amount)
{
	affinity.x1 = affinity.y1 = affinity.x2 = affinity.y2 = 0;
	switch (tabPos(shape)) {
		case North:	affinity.y1 = amount;	break;
		case South:	affinity.y2 = -amount;	break;
		case West:	affinity.x1 = amount;	break;
		case East:		affinity.x2 = -amount;	break;
	}
}


/*-----------------------------------------------------------------------*/
#if 0
enum TabState
{
	TS_New,
	TS_Removed,
	TS_Inactive,
	TS_HoverAnim,
	TS_Hover,
	TS_ActiveAnim,
	TS_Active,
	TS_Moved,
	TS_LabelChanged	// text, icon or color changed
	TS_StateChanged	// disabled changed
};


class TabAnim
{
	qreal pos;
	qreal speed;
	int color;
};


class Tab
{
	public:
		TabState state;
		TabAnim anim;
		QStyleOptionTab oldOption;
};


/*-----------------------------------------------------------------------*/

class TabBarState
{
	public:
		TabBarState() : active_tab(0), hover_tab(0) { }

	public:
		QList<Tab> tabs;
		Tab *active_tab;
		Tab *hover_tab;
		int hover_x;
		int hover_y;
		int hover_counter;
};


const TabBarState *SkulpturePrivate::tabBarState(const QWidget *widget)
{
	if (qobject_cast<const QTabBar *>(widget)) {
		if ((int i = tabBarStates.indexOf(widget))) {
			return tabBarStates.at(i);
		}
		// add state if not found
		TabBarState *state = new TabBarState;
		if (state) {

		}
	}
	return 0;
}
#endif

/*-----------------------------------------------------------------------*/

static void paintTabBase(QPainter *painter, const QRect &r, const QStyleOption *option, QTabBar::Shape shape)
{
    QRect rect = r;
    if (!isVertical(shape) && option->version >= QStyleOptionTabBarBase::Version) {
        if (((const QStyleOptionTabBarBase *) option)->documentMode) {
            rect.adjust(-10, 0, 10, 0);
        }
    }
	if (true /*option->state & QStyle::State_Enabled*/) {
		QLinearGradient tabGradient(rect.topLeft(), isVertical(shape) ? rect.topRight() : rect.bottomLeft());
		tabGradient.setColorAt(0.0, option->palette.color(QPalette::Window).darker(118));
		tabGradient.setColorAt(1.0, option->palette.color(QPalette::Window).darker(105));
		painter->fillRect(rect.adjusted(1, 1, -1, -1), tabGradient);
	} else {
		painter->fillRect(rect.adjusted(1, 1, -1, -1), option->palette.color(QPalette::Window).darker(106));
	}
	paintThinFrame(painter, rect.adjusted(1, 1, -1, -1), option->palette, -20, -40);
	paintRecessedFrameShadow(painter, rect.adjusted(2, 2, -2, -2), RF_Small);
}


void paintFrameTabBarBase(QPainter *painter, const QStyleOptionTabBarBase *option, const QWidget *widget)
{
    if (option->version >= QStyleOptionTabBarBase::Version) {
        if (((const QStyleOptionTabBarBase *) option)->documentMode) {
            QRect r = option->rect;
            if (tabPos(option->shape) == North && r.top() > 0) {
                r.setTop(0);
            } else if (tabPos(option->shape) == South && qobject_cast<const QTabBar *>(widget) && widget->rect().bottom() > r.bottom()) {
                r.setBottom(widget->rect().bottom());
            } else {
                return;
            }
            painter->save();
            painter->setClipRect(r);
            paintTabBase(painter, r, option, option->shape);
            paintThinFrame(painter, r.adjusted(-10, 0, 10, 0), option->palette, 60, -20);
            painter->restore();
            return;
        }
    }
	// ### remove clipping
	painter->save();
	QRect r = option->rect;
	r = r.united(option->tabBarRect);
	QRegion region(r);
	region -= option->tabBarRect;
	painter->setClipRegion(region);
	paintTabBase(painter, r, option, option->shape);
	QRect rect = r;
#if 0
	Affinity affinity;
	tabAffinity(option->shape, affinity, 1);
	rect.adjust(affinity.x2, affinity.y2, affinity.x1, affinity.y1);
#endif
	paintThinFrame(painter, rect, option->palette, 60, -20);
	painter->restore();
}


void paintTabWidgetFrame(QPainter *painter, const QStyleOptionTabWidgetFrame *option, const QWidget *widget)
{
	Q_UNUSED(widget);

	QRect base = option->rect;
	int s = (isVertical(option->shape) ? option->tabBarSize.width() : option->tabBarSize.height());
	if (s < 2) s = 2;
	if (isVertical(option->shape)) {
		base.setWidth(s);
	} else {
		base.setHeight(s);
	}
	const int overlap = 2;
	switch (tabPos(option->shape)) {
		case North:	base.translate(0, -(s - overlap));	break;
		case West:	base.translate(-(s - overlap), 0);	break;
		case South:	base.translate(0, option->rect.height() - overlap);	break;
		case East:		base.translate(option->rect.width() - overlap, 0);	break;
	}
	if (s != 2) {
		paintTabBase(painter, base, option, option->shape);
	}

	Affinity affinity;
	tabAffinity(option->shape, affinity, -(s - overlap));
//	painter->save();
//	painter->setClipRect(base);
	paintThinFrame(painter, option->rect.adjusted(affinity.x1, affinity.y1, affinity.x2, affinity.y2), option->palette, 60, -20);
	paintThinFrame(painter, option->rect.adjusted(1, 1, -1, -1), option->palette, -40, 160);
#if 1
        painter->save();
        painter->setPen(QPen(QColor(0, 0, 0, 20), 1));
        painter->drawLine(option->rect.x() + 1, option->rect.bottom(), option->rect.right() - 1, option->rect.bottom());
        painter->drawLine(option->rect.right(), option->rect.top() + 1, option->rect.right(), option->rect.bottom());
        painter->restore();
#endif
#if 0
	QRect r = option->rect.adjusted(2, 2, -2, -2);
	painter->fillRect(r, option->palette.color(QPalette::Window));
	QLinearGradient gradient(r.topLeft(), r.bottomLeft());
	gradient.setColorAt(0.0, QColor(255, 255, 255, 0));
	gradient.setColorAt(1.0, QColor(0, 0, 0, 20));
	painter->fillRect(r, gradient);
#endif
//	painter->restore();
}


/*-----------------------------------------------------------------------*/

#define TAB_SHIFT 1

void paintTabBarTabShape(QPainter *painter, const QStyleOptionTab *option, const QWidget *widget, const QStyle *style)
{
	Q_UNUSED(style);
        const QColor tabBackgroundColor = option->palette.color(QPalette::Active, QPalette::Window);
        bool mouse = (option->state & QStyle::State_MouseOver) && !(option->state & QStyle::State_Selected) && (option->state & QStyle::State_Enabled);
	QRect c_rect = option->rect;
	bool konq = false;

	if (widget && widget->parentWidget()) {
		if (!qstrcmp(widget->metaObject()->className(), "KTabBar") && !qstrcmp(widget->parentWidget()->metaObject()->className(), "KonqFrameTabs")) {
			konq = true;
                        if (option->version >= QStyleOptionTab::Version) {
                            if (((const QStyleOptionTab *) option)->documentMode) {
                                konq = false;
                            }
                        }
		}
	}
	if (konq || (widget && !(qobject_cast<const QTabWidget *>(widget->parentWidget())))) {
		// ### remove clipping
		painter->save();
		painter->setClipRect(option->rect);
		QRect rect = widget->rect();
		if (konq) {
			rect.adjust(-10, 0, 10, 0);
		}
		paintTabBase(painter, rect, option, option->shape);
#if 0
		Affinity affinity;
		tabAffinity(option->shape, affinity, 1);
		rect.adjust(affinity.x2, affinity.y2, affinity.x1, affinity.y1);
#endif
		paintThinFrame(painter, rect, option->palette, 60, -20);
		painter->restore();
	}

	switch (tabPos(option->shape)) {
	case North:
		c_rect.adjust(1, 1, -2, 0);
		if (option->position != QStyleOptionTab::Beginning
		&& option->position != QStyleOptionTab::OnlyOneTab) {
			c_rect.adjust(-1, 0, 0, 0);
		}
		if (option->state & QStyle::State_Selected) {
			painter->fillRect(c_rect.adjusted(0, 0, -1, 0), tabBackgroundColor);
			if (option->state & QStyle::State_Enabled) {
				QLinearGradient gradient(c_rect.topLeft(), c_rect.bottomLeft());
#if 0
				QColor c = option->palette.color(QPalette::Highlight);
				gradient.setColorAt(0.0, c);
				gradient.setColorAt(0.2, QColor(255, 255, 255, 20));
				gradient.setColorAt(1.0, QColor(255, 255, 255, 0));
#else
				gradient.setColorAt(0.0, shaded_color(tabBackgroundColor, 20));
				gradient.setColorAt(1.0, shaded_color(tabBackgroundColor, 0));
#endif
				painter->fillRect(c_rect.adjusted(0, 0, -1, 0), gradient);
			}
			// ### flat tabs: 50->20, 180->80
		//	painter->fillRect(c_rect, QColor(255, 0, 0, 100));
			Affinity affinity;
			tabAffinity(option->shape, affinity, 1);
			paintThinFrame(painter, c_rect.adjusted(affinity.x2, affinity.y2, affinity.x1, affinity.y1), option->palette, -50, 180);
			// shadows
			painter->fillRect(QRect(c_rect.right() + 1, c_rect.top(), 1, c_rect.height()), QColor(0, 0, 0, 25));
			painter->fillRect(QRect(c_rect.right() + 2, c_rect.top(), 1, c_rect.height()), QColor(0, 0, 0, 10));
		} else {
			// ### install clip
			painter->save();
			Affinity affinity;
			tabAffinity(option->shape, affinity, -1);
			painter->setClipRect(option->rect.adjusted(affinity.x2, affinity.y2, affinity.x1, affinity.y1));

			painter->fillRect(c_rect.adjusted(1, mouse ? 1 : 2, -1, -1), mouse ? tabBackgroundColor.darker(104) : tabBackgroundColor.darker(108));
			paintThinFrame(painter, c_rect.adjusted(1, mouse ? 1 : 2, -1, 1), option->palette, -40, 90);
			// shadows
			painter->fillRect(QRect(c_rect.right(), c_rect.top() + 3, 1, c_rect.height() - 4), QColor(0, 0, 0, 15));
			painter->fillRect(QRect(c_rect.right() + 1, c_rect.top() + 3, 1, c_rect.height() - 4), QColor(0, 0, 0, 5));

			painter->restore();
			// shadow below base
			painter->fillRect(QRect(c_rect.left() + 1, c_rect.bottom() - 1, c_rect.width() - 2, 1), QColor(0, 0, 0, 10));
			painter->fillRect(QRect(c_rect.left() + 1, c_rect.bottom() - 2, c_rect.width() - 2, 1), QColor(0, 0, 0, 4));
		}
		break;
	case South:
		c_rect.adjust(1, 0, -2, -1);
		if (option->position != QStyleOptionTab::Beginning
		 && option->position != QStyleOptionTab::OnlyOneTab) {
			c_rect.adjust(-1, 0, 0, 0);
		}
		if (option->state & QStyle::State_Selected) {
			painter->fillRect(c_rect.adjusted(0, 0, -1, 0), tabBackgroundColor);
			if (option->state & QStyle::State_Enabled) {
				QLinearGradient gradient(c_rect.topLeft(), c_rect.bottomLeft());
				gradient.setColorAt(0.0, shaded_color(tabBackgroundColor, 0));
				gradient.setColorAt(1.0, shaded_color(tabBackgroundColor, -5));
				painter->fillRect(c_rect.adjusted(0, 0, -1, 0), gradient);
			}
			Affinity affinity;
			tabAffinity(option->shape, affinity, 1);
			paintThinFrame(painter, c_rect.adjusted(affinity.x2, affinity.y2, affinity.x1, affinity.y1), option->palette, -50, 180);
			//paintThinFrame(painter, c_rect.adjusted(0, -1, 0, 0), option->palette, -50, 180);
			// shadows
			painter->fillRect(QRect(c_rect.right() + 1, c_rect.top() + 1, 1, c_rect.height() - 1), QColor(0, 0, 0, 25));
			painter->fillRect(QRect(c_rect.right() + 2, c_rect.top() + 1, 1, c_rect.height() - 1), QColor(0, 0, 0, 10));
			painter->fillRect(QRect(c_rect.left() + 1, c_rect.bottom() + 1, c_rect.width(), 1), QColor(0, 0, 0, 20));
		} else {
			// ### install clip
			painter->save();
			Affinity affinity;
			tabAffinity(option->shape, affinity, -1);
			painter->setClipRect(option->rect.adjusted(affinity.x2, affinity.y2, affinity.x1, affinity.y1));

			painter->fillRect(c_rect.adjusted(1, 1, -1, mouse ? -1 : -2), mouse ? tabBackgroundColor.darker(104) : tabBackgroundColor.darker(108));
			paintThinFrame(painter, c_rect.adjusted(1, -1, -1, mouse ? -1 : -2), option->palette, -40, 90);
			// shadows
			painter->fillRect(QRect(c_rect.right(), c_rect.top() + 1, 1, c_rect.height() - 2), QColor(0, 0, 0, 15));
			painter->fillRect(QRect(c_rect.right() + 1, c_rect.top() + 1, 1, c_rect.height() - 2), QColor(0, 0, 0, 5));
			if (!mouse) {
				painter->fillRect(QRect(c_rect.left() + 2, c_rect.bottom() - 1, c_rect.width() - 3, 1), QColor(0, 0, 0, 10));
			}

			painter->restore();
			// shadow below base
			painter->fillRect(QRect(c_rect.left() + 1, c_rect.top() + 1, c_rect.width() - 2, 1), QColor(0, 0, 0, 30));
			painter->fillRect(QRect(c_rect.left() + 1, c_rect.top() + 2, c_rect.width() - 2, 1), QColor(0, 0, 0, 15));
			painter->fillRect(QRect(c_rect.left() + 1, c_rect.top() + 3, c_rect.width() - 2, 1), QColor(0, 0, 0, 8));
			painter->fillRect(QRect(c_rect.left() + 1, c_rect.top() + 4, c_rect.width() - 2, 1), QColor(0, 0, 0, 4));
		}
		break;
	case West:
		c_rect.adjust(1, 1, 0, -2);
		if (option->state & QStyle::State_Selected) {
			painter->fillRect(c_rect.adjusted(0, 0, 0, -1), tabBackgroundColor);
			if (option->state & QStyle::State_Enabled) {
				QLinearGradient gradient(c_rect.topLeft(), c_rect.topRight());
				gradient.setColorAt(0.0, shaded_color(tabBackgroundColor, 20));
				gradient.setColorAt(1.0, shaded_color(tabBackgroundColor, 0));
				painter->fillRect(c_rect.adjusted(0, 0, 0, -1), gradient);
			}
			Affinity affinity;
			tabAffinity(option->shape, affinity, 1);
			paintThinFrame(painter, c_rect.adjusted(affinity.x2, affinity.y2, affinity.x1, affinity.y1), option->palette, -50, 180);
			// shadows
			painter->fillRect(QRect(c_rect.left(), c_rect.bottom() + 1, c_rect.width(), 1), QColor(0, 0, 0, 25));
			painter->fillRect(QRect(c_rect.left(), c_rect.bottom() + 2, c_rect.width(), 1), QColor(0, 0, 0, 10));
		} else {
			// ### install clip
			painter->save();
			Affinity affinity;
			tabAffinity(option->shape, affinity, -1);
			painter->setClipRect(option->rect.adjusted(affinity.x2, affinity.y2, affinity.x1, affinity.y1));
			painter->fillRect(c_rect.adjusted(mouse ? 1 : 2, 1, 1, -1), mouse ? tabBackgroundColor.darker(104) : tabBackgroundColor.darker(108));
			paintThinFrame(painter, c_rect.adjusted(mouse ? 1 : 2, 1, 1, -1), option->palette, -40, 90);
			// shadows
			painter->fillRect(QRect(c_rect.left() + 2, c_rect.bottom() + 0, c_rect.width() - 3, 1), QColor(0, 0, 0, 15));
			painter->fillRect(QRect(c_rect.left() + 2, c_rect.bottom() + 1, c_rect.width() - 3, 1), QColor(0, 0, 0, 5));

			painter->restore();
			// shadow below base
			painter->fillRect(QRect(c_rect.right() - 1, c_rect.top() + 1, 1, c_rect.height() - 2), QColor(0, 0, 0, 10));
			painter->fillRect(QRect(c_rect.right() - 2, c_rect.top() + 1, 1, c_rect.height() - 2), QColor(0, 0, 0, 4));
		}
		break;
	case East:
		c_rect.adjust(0, 1, -1, -2);
		if (option->state & QStyle::State_Selected) {
			painter->fillRect(c_rect.adjusted(0, 0, 0, -1), tabBackgroundColor);
			if (option->state & QStyle::State_Enabled) {
				QLinearGradient gradient(c_rect.topLeft(), c_rect.topRight());
				gradient.setColorAt(0.0, shaded_color(tabBackgroundColor, 0));
				gradient.setColorAt(1.0, shaded_color(tabBackgroundColor, 10));
				painter->fillRect(c_rect.adjusted(0, 0, 0, -1), gradient);
			}
			Affinity affinity;
			tabAffinity(option->shape, affinity, 1);
			paintThinFrame(painter, c_rect.adjusted(affinity.x2, affinity.y2, affinity.x1, affinity.y1), option->palette, -50, 180);
			// shadows
			painter->fillRect(QRect(c_rect.left(), c_rect.bottom() + 1, c_rect.width(), 1), QColor(0, 0, 0, 25));
			painter->fillRect(QRect(c_rect.left(), c_rect.bottom() + 2, c_rect.width(), 1), QColor(0, 0, 0, 10));
			painter->fillRect(QRect(c_rect.right() + 1, c_rect.top() + 1, 1, c_rect.height()), QColor(0, 0, 0, 20));
		} else {
			// ### install clip
			painter->save();
			Affinity affinity;
			tabAffinity(option->shape, affinity, -1);
			painter->setClipRect(option->rect.adjusted(affinity.x2, affinity.y2, affinity.x1, affinity.y1));
			painter->fillRect(c_rect.adjusted(-2, 1, mouse ? -1 : -2, -1), mouse ? tabBackgroundColor.darker(104) : tabBackgroundColor.darker(108));
			paintThinFrame(painter, c_rect.adjusted(-2, 1, mouse ? -1 : -2, -1), option->palette, -40, 90);
			// shadows
			painter->fillRect(QRect(c_rect.left() + 1, c_rect.bottom() + 0, c_rect.width() - 3, 1), QColor(0, 0, 0, 15));
			painter->fillRect(QRect(c_rect.left() + 1, c_rect.bottom() + 1, c_rect.width() - 3, 1), QColor(0, 0, 0, 5));

			painter->restore();
			// shadow below base
			painter->fillRect(QRect(c_rect.left() + 1, c_rect.top() + 1, 1, c_rect.height() - 2), QColor(0, 0, 0, 20));
			painter->fillRect(QRect(c_rect.left() + 2, c_rect.top() + 1, 1, c_rect.height() - 2), QColor(0, 0, 0, 10));
			painter->fillRect(QRect(c_rect.left() + 3, c_rect.top() + 1, 1, c_rect.height() - 2), QColor(0, 0, 0, 5));
		//	painter->fillRect(QRect(c_rect.left() + 4, c_rect.top() + 1, 1, c_rect.height() - 2), QColor(0, 0, 0, 4));
		}
		break;
	}
}


void paintIndicatorTabClose(QPainter *painter, const QStyleOption *option, const QWidget *widget, const QStyle *style)
{
    int offset = 0;
    QTabBar::Shape shape = QTabBar::RoundedNorth;

    if (widget) {
        if (QTabBar *tabbar = qobject_cast<QTabBar *>(widget->parentWidget())) {
            offset = TAB_SHIFT;
            shape = tabbar->shape();
            for (int i = 0; i < tabbar->count(); ++i) {
                if (tabbar->tabRect(i).contains(widget->mapToParent(QPoint(1, 1)))) {
                    if (i == tabbar->currentIndex()) {
                        offset = 0;
                    } else if (tabbar->tabRect(i).contains(tabbar->mapFromGlobal(QCursor::pos()))) {
                        offset = 0;
                    }
                    break;
                }
            }
            if (false /*tabPos(shape) == East || tabPos(shape) == South*/) {
                offset += 1;
            }
        }
    }
    QIcon::Mode iconMode = QIcon::Normal;
    painter->save();
    if (option->state & QStyle::State_Enabled) {
        if (option->state & QStyle::State_MouseOver || option->state & QStyle::State_Sunken) {
            // paintThinFrame(painter, option->rect, option->palette, 90, -30);
            iconMode = QIcon::Active;
            //painter->fillRect(option->rect.adjusted(1, 1, -1, -1), QColor(220, 0, 0));
            if (!(option->state & QStyle::State_Sunken)) {
                // paintThinFrame(painter, option->rect.adjusted(1, 1, -1, -1), option->palette, -60, 180);
            } else {
                //iconMode = QIcon::Selected;
            }
        } else {
            painter->setOpacity(0.7);
        }
    } else {
        //iconMode = QIcon::Disabled;
        painter->setOpacity(0.7);
    }
    QRect r = QRect(option->rect.center() - QPoint(option->state & QStyle::State_Sunken ? 3 : 4, option->state & QStyle::State_Sunken ? 3 : 4), QSize(10, 10));
    if (offset) {
        Affinity affinity;
        tabAffinity(shape, affinity, offset);
        r.translate(affinity.x1 + affinity.x2, affinity.y1 + affinity.y2);
    }
    painter->drawPixmap(r, style->standardIcon(QStyle::SP_TitleBarCloseButton, option, widget).pixmap(10, 10, iconMode));
    painter->restore();
}


void paintTabBarTabLabel(QPainter *painter, const QStyleOptionTab *option, const QWidget *widget, const QStyle *style)
{
//	bool mouse = (option->state & QStyle::State_MouseOver) && !(option->state & QStyle::State_Selected) && (option->state & QStyle::State_Enabled);
//	painter->save();
#if 0
	if (!(option->state & QStyle::State_Selected)) {
		if (mouse) {
			painter->setOpacity(0.7);
		} else {
			painter->setOpacity(0.5);
		}
	} else {
		painter->setOpacity(0.8);
	}
//	QFont font(painter->font());
//	font.setBold(true);
//	painter->setFont(font);
#endif
    QStyleOptionTab opt;

	int offset = TAB_SHIFT;
	if (option->state & QStyle::State_Selected || (option->state & QStyle::State_MouseOver && option->state & QStyle::State_Enabled)) {
		offset = 0;
	}

	if (option->version > 1) {
		opt = *((QStyleOptionTab *) option);
	} else {
		opt = *option;
	}
	Affinity affinity;
	tabAffinity(option->shape, affinity, offset);
	opt.rect.translate(affinity.x1 + affinity.x2, affinity.y1 + affinity.y2);
	switch (tabPos(option->shape)) {
	case North:
		opt.rect.adjust(-2, 1, -1, 1);
		break;
	case South:
		opt.rect.adjust(-2, 0, -1, 0);
		break;
	case West:
	case East:
		painter->save();
        QTransform mat;
		if (tabPos(option->shape) == West) {
			opt.rect.adjust(3, 0, 3, 0);
		} else {
			opt.rect.adjust(-1, 0, -1, 0);
		}
		QPointF c = opt.rect.center();
		mat.translate(c.x(), c.y());
		mat.rotate(tabPos(option->shape) == West ? -90 : 90);
		mat.translate(-c.x(), -c.y());
		opt.rect = mat.mapRect(opt.rect);
        painter->setTransform(mat, true);
		opt.shape = (QTabBar::Shape) 0;
		break;
	}
	((QCommonStyle *) style)->QCommonStyle::drawControl(QStyle::CE_TabBarTabLabel, &opt, painter, widget);
	if (isVertical(option->shape)) {
		painter->restore();
	}
}


