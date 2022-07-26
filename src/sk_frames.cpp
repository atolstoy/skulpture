/*
 * skulpture_frames.cpp
 *
 */

#include "skulpture_p.h"
#include <QtWidgets/QAbstractSpinBox>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QAbstractItemView>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtGui/QPainter>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLineEdit>
#include <cmath>


/*-----------------------------------------------------------------------*/

static void paintThinFrame(QPainter *painter, const QRect &rect, const QBrush &brush1, const QBrush &brush2)
{
	painter->fillRect(QRect(rect.left() + 1, rect.top(), rect.width() - 1, 1), brush2);
	painter->fillRect(QRect(rect.left(), rect.top(), 1, rect.height()), brush2);
	painter->fillRect(QRect(rect.left(), rect.bottom(), rect.width() - 1, 1), brush1);
	painter->fillRect(QRect(rect.right(), rect.top(), 1, rect.height()), brush1);
}


static const QBrush shaded_brush(const QPalette &palette, int shade, QPalette::ColorRole bgrole)
{
	return (shaded_color(palette.color(bgrole), shade));
}


/*-----------------------------------------------------------------------*/
/**
 * paintThinFrame - paint a single pixel wide frame
 *
 * Paints a frame _inside_ the specified rectangle, using
 * a single pixel wide pen. The frame is rendered by darkening
 * or brightening the pixels in that area; no specific color
 * can be selected.
 *
 * dark and light specify how dark or bright the frame should
 * be rendered. They are either negative (meaning darkening),
 * or positive (meaning brigthening).
 *
 * TODO:
 * dark and light are arbitrary values; they need adjustment.
 *
 */

void paintThinFrame(QPainter *painter, const QRect &rect, const QPalette &palette, int dark, int light, QPalette::ColorRole bgrole)
{
	paintThinFrame(painter, rect, shaded_brush(palette, dark, bgrole), shaded_brush(palette, light, bgrole));
}


void paintRecessedFrame(QPainter *painter, const QRect &rect, const QPalette &palette, enum RecessedFrame rf, QPalette::ColorRole bgrole)
{
	paintThinFrame(painter, rect, palette, 30, -20, bgrole);
	paintThinFrame(painter, rect.adjusted(1, 1, -1, -1), palette, -20, -70, bgrole);
	paintRecessedFrameShadow(painter, rect.adjusted(2, 2, -2, -2), rf);
}


/*-----------------------------------------------------------------------*/

void paintFrameGroupBox(QPainter *painter, const QStyleOptionFrame *option)
{
	QRect r = option->rect;
	r.setHeight(/*r.height() +*/ 2);
	paintThinFrame(painter, r, option->palette, 60, -20);
//	paintThinFrame(painter, r.adjusted(1, 1, -1, -1), option->palette, -20, 60);
}


static inline bool is_popup_menu(const QWidget *widget)
{
	if (widget) {
		Qt::WindowFlags flags = widget->windowFlags();
		Qt::WindowType type = Qt::WindowType(int(flags & Qt::WindowType_Mask));

		if ((type & Qt::Window) && (flags & Qt::FramelessWindowHint || type == Qt::Popup)) {
			return true;
		}
	}
	return false;
}


void paintStyledFrame(QPainter *painter, const QStyleOptionFrame *option, const QWidget *widget, const QStyle */*style*/)
{
	QPalette::ColorRole bgrole = widget ? widget->backgroundRole() : QPalette::Window;

	if (option->state & QStyle::State_Sunken) {
		if (qobject_cast<const QFrame *>(widget) && widget->parentWidget() && widget->parentWidget()->inherits("KFontRequester")) {
			paintThinFrame(painter, option->rect, option->palette, 60, -20);
			paintThinFrame(painter, option->rect.adjusted(1, 1, -1, -1), option->palette, -20, 60);
			QLinearGradient panelGradient(option->rect.topLeft(), option->rect.bottomLeft());
			panelGradient.setColorAt(0.6, QColor(255, 255, 255, 0));
			panelGradient.setColorAt(1.0, shaded_color(option->palette.color(QPalette::Window), 70));
			painter->fillRect(option->rect.adjusted(2, 2, -2, -2), panelGradient);
		} else {
		/*	if (option->palette.color(QPalette::Base) == QColor(220, 230, 210)) {
				painter->fillRect(option->rect.adjusted(2, 2, -2, -2), option->palette.color(QPalette::Base));
				paintRecessedFrame(painter, option->rect, option->palette, RF_Small);
			} else*/ {
				RecessedFrame rf = RF_Large;
				if (!(option->state & QStyle::State_Enabled)
				 || (widget && (!widget->isEnabled() || qobject_cast<const QLCDNumber *>(widget)))) {
					rf = RF_Small;
				}
				if (qobject_cast<const QAbstractItemView *>(widget) || (widget && widget->inherits("Q3ScrollView"))) {
					const QList<QObject *> children = widget->children();
                    Q_FOREACH (QObject *child, children) {
						if (qobject_cast<FrameShadow *>(child)) {
							rf = RF_None;
							break;
						}
					}
				}
				paintRecessedFrame(painter, option->rect, option->palette, rf);
			}
		}
	} else if (option->state & QStyle::State_Raised) {
		QRect r = option->rect;
		if (option->lineWidth == 0) {
			paintThinFrame(painter, r, option->palette, -20, 60);
		} else {
			paintThinFrame(painter, r, option->palette, -10, -20);
			paintThinFrame(painter, r.adjusted(1, 1, -1, -1), option->palette, -40, 80, bgrole);
		//	painter->fillRect(option->rect, Qt::red);
		}
	} else {
		// Plain
		if (qobject_cast<const QFrame *>(widget) && widget->parentWidget() && widget->parentWidget()->inherits("KTitleWidget")) {
			QRect r = option->rect;
			bgrole = QPalette::Window;
//			bgrole = QPalette::Base;
#if 1
			QColor bgcolor = option->palette.color(bgrole);
#else
			QColor bgcolor = QColor(230, 230, 230);
#endif
		//	painter->fillRect(r.adjusted(1, 1, -1, -1), bgcolor);
			paintThinFrame(painter, r, option->palette, -10, -20);
		//	painter->fillRect(r.adjusted(1, 1, -1, -1), QColor(200, 190, 160));
		//	painter->fillRect(r.adjusted(1, 1, -1, -1), QColor(240, 240, 240));
			paintThinFrame(painter, r.adjusted(1, 1, -1, -1), option->palette, -30, 80, bgrole);
			QLinearGradient gradient(r.topLeft(), r.bottomLeft());
			gradient.setColorAt(0.0, shaded_color(bgcolor, 90));
			gradient.setColorAt(0.2, shaded_color(bgcolor, 60));
			gradient.setColorAt(0.5, shaded_color(bgcolor, 0));
			gradient.setColorAt(0.51, shaded_color(bgcolor, -10));
			gradient.setColorAt(1.0, shaded_color(bgcolor, -20));
			painter->fillRect(r.adjusted(1, 1, -1, -1), gradient);
#if 0
			QRadialGradient dialogGradient2(r.left() + r.width() / 2, r.top(), r.height());
			dialogGradient2.setColorAt(0.0, QColor(255, 255, 255, 50));
			dialogGradient2.setColorAt(1.0, QColor(0, 0, 0, 0));
			painter->save();
			painter->translate(r.center());
			painter->scale(r.width() / 2.0 / r.height(), 1);
			painter->translate(-r.center());
			painter->fillRect(r.adjusted(1, 1, -1, -1), dialogGradient2);
			painter->restore();
#endif
		} else if (is_popup_menu(widget)) {
			QRect r = option->rect;
			paintThinFrame(painter, r, option->palette, -60, 160);
			paintThinFrame(painter, r.adjusted(1, 1, -1, -1), option->palette, -20, 60, bgrole);
		} else {
			QRect r = option->rect;
			paintThinFrame(painter, r, option->palette, 60, -20);
			paintThinFrame(painter, r.adjusted(1, 1, -1, -1), option->palette, -20, 60, bgrole);
		}
	}
}


void paintFrameLineEdit(QPainter *painter, const QStyleOptionFrame *option)
{
	paintRecessedFrame(painter, option->rect, option->palette, RF_Small);
}


class LineEditHack : public QLineEdit
{
    public:
        QRect cursorRectHack() const {
            return cursorRect();
        }
};


static QRect getCursorRect(const QWidget *widget)
{
    if (const QLineEdit *lineEdit = qobject_cast<const QLineEdit *>(widget)) {
        const LineEditHack *lineEditHack = reinterpret_cast<const LineEditHack *>(lineEdit);
        /*if (lineEdit->cursorPosition() != lineEdit->text().length())*/ {
            return lineEditHack->cursorRectHack();
        }
    }
    return QRect();
}


void lineEditMouseMoved(QLineEdit *lineEdit, QMouseEvent *event)
{
    if (!lineEdit->hasFocus()) {
        QAbstractSpinBox *spinBox = qobject_cast<QAbstractSpinBox *>(lineEdit->parentWidget());
        int oldCursorPosition = lineEdit->cursorPosition();
        int newCursorPosition = lineEdit->cursorPositionAt(event->pos());

        if (spinBox && lineEdit->text() == spinBox->specialValueText()) {
            // newCursorPosition = lineEdit->text().length();
        } else {
            if (QSpinBox *spinBox = qobject_cast<QSpinBox *>(lineEdit->parentWidget())) {
                newCursorPosition = qBound(spinBox->prefix().length(), newCursorPosition, lineEdit->text().length() - spinBox->suffix().length());
            } else if (QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox *>(lineEdit->parentWidget())) {
                newCursorPosition = qBound(spinBox->prefix().length(), newCursorPosition, lineEdit->text().length() - spinBox->suffix().length());
            }
        }

        if (oldCursorPosition != newCursorPosition) {
            lineEdit->update(getCursorRect(lineEdit).adjusted(-4, -16, 4, 16));
            lineEdit->setCursorPosition(newCursorPosition);
            lineEdit->update(getCursorRect(lineEdit).adjusted(-4, -16, 4, 16));
        }
    }
}


void paintPanelLineEdit(QPainter *painter, const QStyleOptionFrame *option, const QWidget *widget, const QStyle *style)
{
	QPalette::ColorRole bgrole = widget ? widget->backgroundRole() : QPalette::Window;

	bool focus = (option->state & QStyle::State_HasFocus) && !(option->state & QStyle::State_ReadOnly);
        int fw = option->lineWidth;
        if (option->palette.brush(bgrole).style() == Qt::SolidPattern) {
            QRect cursorRect;
                QColor color = option->palette.color(bgrole);
//		printf("style=%d, bgrole=%d, panel color: r=%d, g=%d, b=%d, a=%d\n", option->palette.brush(bgrole).style(), bgrole, color.red(), color.green(), color.blue(), color.alpha());
		if (focus && color.alpha() > 0) {
			color = blend_color(color, option->palette.color(QPalette::Highlight), 0.15);
		} else {
                    focus = false;
#if 1
                    if (option->state & QStyle::State_MouseOver && option->state & QStyle::State_Enabled && !(option->state & QStyle::State_ReadOnly)) {
                        color = color.lighter(103);
                        cursorRect = getCursorRect(widget);
                    } else if (widget) {
                        QWidget *box = widget->parentWidget();
                        if (qobject_cast<QComboBox *>(box) || qobject_cast<QAbstractSpinBox *>(box)) {
                            if (box->underMouse() && option->state & QStyle::State_Enabled) {
                                QAbstractSpinBox *spinBox = qobject_cast<QAbstractSpinBox *>(box);
                                if (!spinBox || !spinBox->isReadOnly()) {
                                    color = color.lighter(103);
                                }
                            }
                        }
                    }
#endif
                }
                painter->fillRect(option->rect.adjusted(fw, fw, -fw, -fw), color);
		if (false && option->state & QStyle::State_Enabled && option->rect.height() <= 64) {
			QLinearGradient panelGradient(option->rect.topLeft(), option->rect.bottomLeft());
			if (color.valueF() > 0.9) {
				panelGradient.setColorAt(0.0, shaded_color(color, -20));
			}
			panelGradient.setColorAt(0.6, shaded_color(color, 0));
			panelGradient.setColorAt(1.0, shaded_color(color, 10));
			painter->fillRect(option->rect.adjusted(fw, fw, -fw, -fw), panelGradient);
		}
                if (!cursorRect.isEmpty()) {
                    QRect cursor = style->subElementRect(QStyle::SE_LineEditContents, option, widget).adjusted(0, 2, 0, -3);
                    if (cursor.height() != option->fontMetrics.height() - 1) {
                        cursor.adjust(0, 1, 0, 0);
                    }
                    int cursorWidth = style->pixelMetric(QStyle::PM_TextCursorWidth, option, widget);
                    cursor.setLeft(cursorRect.center().x() + 1 - (cursorWidth >> 1));
                    cursor.setWidth(cursorWidth);
                    cursor.setTop(cursor.top() + ((cursor.height() - option->fontMetrics.height() + 2) >> 1));
                    cursor.setHeight(cursorRect.height() - 2);
                    QColor color = option->palette.color(QPalette::Text);
                    color.setAlpha(60);
                    painter->fillRect(cursor, color);
                    painter->fillRect(QRect(cursor.left() - cursorWidth, cursor.top() - 1, cursorWidth, 1), color);
                    painter->fillRect(QRect(cursor.left() + cursorWidth, cursor.top() - 1, cursorWidth, 1), color);
                    painter->fillRect(QRect(cursor.left() - cursorWidth, cursor.bottom() + 1, cursorWidth, 1), color);
                    painter->fillRect(QRect(cursor.left() + cursorWidth, cursor.bottom() + 1, cursorWidth, 1), color);
                }
        }
	if (focus && option->state & QStyle::State_KeyboardFocusChange) {
            QColor color = option->palette.color(QPalette::Highlight).darker(120);
		color.setAlpha(120);
                QRect r = option->rect.adjusted(fw + 2, fw + 2, -fw - 2, -fw - 2);
                r.setTop(r.top() + r.height() - 1);
		painter->fillRect(r, color);
	}
	if (fw) {
		if (option->state & QStyle::State_ReadOnly && !(option->state & QStyle::State_Enabled)) {
			paintThinFrame(painter, option->rect, option->palette, 60, -20);
			paintThinFrame(painter, option->rect.adjusted(1, 1, -1, -1), option->palette, -20, 60);
		} else {
			paintRecessedFrame(painter, option->rect, option->palette, option->rect.height() <= 64 ? RF_Small : RF_Small);
		}
	} else if (widget && widget->parent() && (qobject_cast<QAbstractSpinBox *>(widget->parent()) || qobject_cast<QComboBox *>(widget->parent()))) {
		if (option->palette.brush(bgrole).style() != Qt::SolidPattern) {
			/* Fix Qt stylesheet demo */
			return;
		}
                // we need to adjust shadows to the real frame, not that of the line edit
                int left = widget->geometry().left(), right = widget->geometry().right();
		QComboBox *combo = qobject_cast<QComboBox *>(widget->parent());
                int parentFrameWidth;
                if (combo) {
                    if (!combo->hasFrame()) {
                        return;
                    }
                    parentFrameWidth = style->pixelMetric(QStyle::PM_ComboBoxFrameWidth, option, widget);
		} else {
                    QAbstractSpinBox *spin = qobject_cast<QAbstractSpinBox *>(widget->parent());
                    if (spin && !spin->hasFrame()) {
                        return;
                    }
                    parentFrameWidth = style->pixelMetric(QStyle::PM_SpinBoxFrameWidth, option, widget);
                }
                paintRecessedFrameShadow(painter, option->rect.adjusted(parentFrameWidth - left, 0, widget->parentWidget()->geometry().width() - right - parentFrameWidth - 1, 0), option->rect.height() <= 64 ? RF_Small : RF_Small);
        }
}


void paintFrameFocusRect(QPainter *painter, const QStyleOptionFocusRect *option, const QWidget *widget)
{
	if (!(option->state & QStyle::State_KeyboardFocusChange)) {
		return;
	}
        if (option->state & QStyle::State_Item) {
            if (widget && widget->window() && !widget->window()->testAttribute(Qt::WA_KeyboardFocusChange)) {
                return;
            }
        }
	QColor color = option->palette.color(QPalette::Highlight);
	color.setAlpha(20);
	painter->fillRect(option->rect, color);
//	painter->fillRect(option->rect.adjusted(1, 1, -1, -1), color);
	painter->fillRect(option->rect.adjusted(2, 2, -2, -2), color);
        color = color.darker(120);
	color.setAlpha(230);
	painter->fillRect(option->rect.adjusted(0, option->rect.height() - 1, 0, 0), color);
}


QRect subControlRectGroupBox(const QStyleOptionGroupBox *option, QStyle::SubControl subControl, const QWidget *widget, const QStyle *style)
{
    switch (subControl) {
        case QStyle::SC_GroupBoxContents:
            return option->rect.adjusted(0, option->fontMetrics.height(), 0, 0);
        case QStyle::SC_GroupBoxCheckBox:
        case QStyle::SC_GroupBoxLabel: {
            if (!(option->features & QStyleOptionFrame::Flat)) {
                int x = option->direction == Qt::LeftToRight ? -8 : 8;
                int y = (subControl == QStyle::SC_GroupBoxCheckBox) ? 0 : 1;
                return ((const QCommonStyle *) style)->QCommonStyle::subControlRect(QStyle::CC_GroupBox, option, subControl, widget).adjusted(x, y, x, y);
            }
            break;
        }
        default:
            break;
    }
    return ((const QCommonStyle *) style)->QCommonStyle::subControlRect(QStyle::CC_GroupBox, option, subControl, widget);
}


/*-----------------------------------------------------------------------*/

QGradient path_edge_gradient(const QRectF &rect, const QStyleOption *option, const QPainterPath &path, const QColor &color2, const QColor &color1)
{
	QPointF c = rect.center();
	QColor color[8];
	if (/*true ||*/ option->direction == Qt::LeftToRight) {
		color[0] = blend_color(QColor(255, 255, 255, 255), color1, 0.5);
		color[1] = color1;
		color[2] = blend_color(color1, color2, 0.5);
		color[3] = color2;
	//	color[3] = color1;
		color[4] = blend_color(QColor(0, 0, 0, 255), color2, 0.5);
		color[5] = color2;
		color[6] = blend_color(color2, color1, 0.5);
		color[7] = color1;
	//	color[7] = color2;
	} else {
		color[2] = blend_color(QColor(255, 255, 255, 255), color1, 0.5);
		color[1] = color1;
		color[0] = blend_color(color1, color2, 0.5);
		color[7] = color2;
	//	color[7] = color1;
		color[6] = blend_color(QColor(0, 0, 0, 255), color2, 0.5);
		color[5] = color2;
		color[4] = blend_color(color2, color1, 0.5);
		color[3] = color1;
	//	color[3] = color2;
	}
	QConicalGradient gradient(c, 0);
	Q_UNUSED(path);
	qreal angle;
	qreal d = 1;
	QRectF r = rect.adjusted(1, 1, -1, -1);
	{
		QRectF rect = r;
		gradient.setColorAt(0.0, color[3]);

		angle = atan2(rect.top() + d - c.y(), rect.right() - c.x());
		if (angle < 0) angle += M_PI * 2.0;
		gradient.setColorAt(angle / 2.0 / M_PI, color[3]);

		angle = atan2(rect.top() - c.y(), rect.right() - d - c.x());
		if (angle < 0) angle += M_PI * 2.0;
		gradient.setColorAt(angle / 2.0 / M_PI, color[5]);

		angle = atan2(rect.top() - c.y(), rect.left() + d - c.x());
		if (angle < 0) angle += M_PI * 2.0;
		gradient.setColorAt(angle / 2.0 / M_PI, color[5]);

		angle = atan2(rect.top() + d - c.y(), rect.left() - c.x());
		if (angle < 0) angle += M_PI * 2.0;
		gradient.setColorAt(angle / 2.0 / M_PI, color[7]);

		angle = atan2(rect.bottom() - d - c.y(), rect.left() - c.x());
		if (angle < 0) angle += M_PI * 2.0;
		gradient.setColorAt(angle / 2.0 / M_PI, color[7]);

		angle = atan2(rect.bottom() - c.y(), rect.left() + d - c.x());
		if (angle < 0) angle += M_PI * 2.0;
		gradient.setColorAt(angle / 2.0 / M_PI, color[1]);

		angle = atan2(rect.bottom() - c.y(), rect.right() - d - c.x());
		if (angle < 0) angle += M_PI * 2.0;
		gradient.setColorAt(angle / 2.0 / M_PI, color[1]);

		angle = atan2(rect.bottom() - d - c.y(), rect.right() - c.x());
		if (angle < 0) angle += M_PI * 2.0;
		gradient.setColorAt(angle / 2.0 / M_PI, color[3]);

		gradient.setColorAt(1.0, color[3]);
	}
	return gradient;
}


