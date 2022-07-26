/*
 * skulpture_progressbar.cpp
 *
 */

#include "skulpture_p.h"
#include <QtGui/QPainter>
#include <QtWidgets/QProgressBar>
#include <QtCore/QTime>
#include <cmath>

#if 0
#define FG_ROLE_PROGRESS QPalette::WindowText
#define BG_ROLE_PROGRESS QPalette::Window
#elif 1
#define FG_ROLE_PROGRESS QPalette::Text
#define BG_ROLE_PROGRESS QPalette::Base
#else
#define FG_ROLE_PROGRESS QPalette::QPalette::Highlight
#define BG_ROLE_PROGRESS QPalette::Base
#endif
#define FG_ROLE_CHUNK QPalette::HighlightedText
#define BG_ROLE_CHUNK QPalette::Highlight

/*-----------------------------------------------------------------------*/

static bool isPasswordStrengthIndicator(const QWidget *widget)
{
    return widget && widget->parentWidget() && widget->parentWidget()->parentWidget()
     && widget->parentWidget()->parentWidget()->inherits("KNewPasswordDialog");
}


static bool isDiskSpaceIndicator(const QWidget *widget)
{
    return false && widget && widget->inherits("StatusBarSpaceInfo");
}


/*-----------------------------------------------------------------------*/

static bool progressBarContentsCentered(const QStyleOptionProgressBar *option, const QWidget *widget)
{
    const bool vertical = option->version >= 2 && option->state == Qt::Vertical;
    if (vertical) {
        return false;
    }
    if (isPasswordStrengthIndicator(widget) || isDiskSpaceIndicator(widget)) {
        return false;
    }
    return true;
}


static QRect progressBarContentsRect(const QStyleOptionProgressBar *option, bool contentsCentered)
{
    // configuration options
    const int border = 2;

    QRect contentsRect = option->rect.adjusted(border, border, -border, -border);
    if (option->minimum < option->maximum) {
        if (option->progress > option->minimum) {
            if (option->progress < option->maximum) {
                // progress
                qreal progress = qreal(option->progress - option->minimum) / (option->maximum - option->minimum);
                if (option->version >= 2 && option->state == Qt::Vertical) {
                    if (contentsCentered) {
                        int adjustment = int((contentsRect.height() / 2) * (1 - progress));
                        contentsRect.adjust(0, adjustment, 0, -adjustment);
                    } else {
                        int contentsHeight = qMax(1, int(contentsRect.height() * progress + 0.5));
                        if (option->version >= 2 && option->invertedAppearance) {
                            contentsRect.setHeight(contentsHeight);
                        } else {
                            contentsRect.setTop(contentsRect.top() + contentsRect.height() - contentsHeight);
                        }
                    }
                } else {
                    if (contentsCentered) {
                        int adjustment = int((contentsRect.width() / 2) * (1 - progress));
                        contentsRect.adjust(adjustment, 0, -adjustment, 0);
                    } else {
                        int contentsWidth = qMax(1, int(contentsRect.width() * progress + 0.5));
                        if ((option->version >= 2 && option->invertedAppearance) ^ (option->direction != Qt::LeftToRight)) {
                            contentsRect.setLeft(contentsRect.left() + contentsRect.width() - contentsWidth);
                        } else {
                            contentsRect.setWidth(contentsWidth);
                        }
                    }
                }
            } else {
                // finished
            }
        } else {
            // starting
            contentsRect = QRect();
        }
    } else if (option->minimum == option->maximum) {
        // busy indicator
    } else {
        // invalid values
        contentsRect = QRect();
    }
    return contentsRect;
}


/*-----------------------------------------------------------------------*/

void paintProgressBarGroove(QPainter *painter, const QStyleOptionProgressBar *option)
{
    painter->fillRect(option->rect.adjusted(2, 2, -2, -2), option->palette.brush(BG_ROLE_PROGRESS));
}


void paintProgressBarLabel(QPainter *painter, const QStyleOptionProgressBar *option, const QWidget *widget, const QStyle *style)
{
    if (!option->textVisible || option->text.isEmpty()) {
        return;
    }
    const bool vertical = option->version >= 2 && option->state == Qt::Vertical;
    Qt::Alignment alignment = option->textAlignment;
    if (vertical) {
        if (!(alignment & (Qt::AlignTop | Qt::AlignBottom | Qt::AlignVCenter))) {
            alignment |= Qt::AlignVCenter;
        }
        alignment &= ~(Qt::AlignLeft | Qt::AlignRight);
        alignment |= Qt::AlignHCenter;
    } else {
        if (!(alignment & (Qt::AlignLeft | Qt::AlignRight | Qt::AlignHCenter))) {
            alignment |= Qt::AlignHCenter;
        }
        alignment &= ~(Qt::AlignTop | Qt::AlignBottom);
        alignment |= Qt::AlignVCenter;
    }
    // FIXME currently forces centering
    if (true) {
        alignment &= ~(Qt::AlignLeft | Qt::AlignRight);
        alignment &= ~(Qt::AlignTop | Qt::AlignBottom);
        alignment |= Qt::AlignCenter;
    }
    const bool contentsCentered = progressBarContentsCentered(option, widget);
    const QRect contentsRect = progressBarContentsRect(option, contentsCentered);
    QTransform mat;
    if (vertical) {
        QPointF c = QRectF(option->rect).center();
        mat.translate(c.x(), c.y());
        mat.rotate(option->bottomToTop ? -90 : 90);
        mat.translate(-c.x(), -c.y());
    }
    QRect r = mat.mapRect(option->rect).adjusted(6, 2, -6, -2);
    painter->save();
    painter->setClipRegion(contentsRect);
    painter->setTransform(mat, true);
    style->drawItemText(painter, r, alignment, option->palette, true, option->text, FG_ROLE_CHUNK);
    painter->restore();
    painter->save();
    QRegion region = option->rect;
    region -= contentsRect;
    painter->setClipRegion(region);
    painter->setTransform(mat, true);
    style->drawItemText(painter, r, alignment, option->palette, option->state & QStyle::State_Enabled, option->text, FG_ROLE_PROGRESS);
    painter->restore();
}


/*-----------------------------------------------------------------------*/

enum AnimationMode {
    NoAnimation,
    FloatAnimation,
    LiquidAnimation
};


static QColor progressBarFillColor(const QStyleOptionProgressBar *option, const QWidget *widget)
{
    QColor fillColor = option->palette.color(BG_ROLE_CHUNK);
    if (isPasswordStrengthIndicator(widget)) {
        int p = option->minimum < option->maximum ? 100 * (option->progress - option->minimum) / (option->maximum - option->minimum) : 0;
        fillColor.setHsv(p * 85 / 100, 200, 240 - p);
    } else if (isDiskSpaceIndicator(widget)) {
        int p = option->minimum < option->maximum ? 100 * (option->progress - option->minimum) / (option->maximum - option->minimum) : 0;
        if (p < 75) p = 100; else p = (100 - p) * 4;
        fillColor.setHsv(p * 85 / 100, 200, 240 - p);
    }
    return fillColor;
}


void paintProgressBarContents(QPainter *painter, const QStyleOptionProgressBar *option, const QWidget *widget, const QStyle *style)
{
    // configuration
    const bool busyIndicator = option->minimum == option->maximum;
    const bool vertical = option->version >= 2 && option->state == Qt::Vertical;
    const AnimationMode animationMode = busyIndicator ? FloatAnimation : vertical ? LiquidAnimation : FloatAnimation;
    const int chunkWidth = 4 * qMax(1, style->pixelMetric(QStyle::PM_ProgressBarChunkWidth, option, widget));
    const bool reverseAnimation = busyIndicator;
    const int animationSpeed = (busyIndicator ? 1000 : 1000) * (reverseAnimation ? -1 : 1);
    const int floatAlpha = busyIndicator ? 100 : 255;
    const bool contentsCentered = progressBarContentsCentered(option, widget);
    const QRect contentsRect = progressBarContentsRect(option, contentsCentered);
    const int cornerSmoothing = 0;
    const int centerSmoothing = busyIndicator ? 100 : 0;
    const int borderSmoothing = 0;
    const bool contentsFrame = false;
    const int gradientSharpness = 0;
    const int gradientRatio = 50;
    const int gradientAngle = 0;

    // contents
    if (!contentsRect.isEmpty()) {
        QColor fillColor = progressBarFillColor(option, widget);
        qreal rounding = -1;
        if (!busyIndicator && option->progress < option->maximum) {
            rounding = contentsCentered ? 0.6 : 0.5;
        }
        switch (animationMode) {
            case NoAnimation: {
                painter->fillRect(contentsRect, fillColor);
                break;
            }
            case FloatAnimation: {
                QColor floatColor;
                if (option->palette.color(FG_ROLE_CHUNK).value() > fillColor.value()) {
                    floatColor = fillColor.lighter(105);
                } else {
                    floatColor = fillColor.darker(105);
                }
                floatColor.setAlpha(floatAlpha);
                int m = QTime(0, 0).msecsTo(QTime::currentTime()) / (animationSpeed / chunkWidth);
                QPoint startPoint = contentsCentered ? contentsRect.topLeft() : contentsRect.center();
                startPoint += vertical ? QPoint(0, chunkWidth - 1 - m % chunkWidth) : QPoint(m % chunkWidth, 0);
                QLinearGradient fillGradient(startPoint, startPoint + (vertical ? QPoint(0, chunkWidth) : QPoint(chunkWidth, qRound(chunkWidth * sin(gradientAngle * M_PI / 180)))));
                fillGradient.setSpread(QGradient::RepeatSpread);
                const qreal delta = gradientRatio * gradientSharpness * 0.000049999;
                fillGradient.setColorAt(0.0, fillColor);
                fillGradient.setColorAt(0.0 + delta, fillColor);
                fillGradient.setColorAt(gradientRatio / 100.0 - delta, floatColor);
                fillGradient.setColorAt(gradientRatio / 100.0 + delta, floatColor);
                fillGradient.setColorAt(gradientRatio / 50.0 - delta, fillColor);
                fillGradient.setColorAt(1.0, fillColor);
                if (contentsCentered) {
                    painter->save();
                    if (vertical) {
                        painter->setClipRect(QRect(contentsRect.x(), contentsRect.y() + (contentsRect.height() >> 1), contentsRect.width(), contentsRect.height() - (contentsRect.height() >> 1)));
                    } else {
                        painter->setClipRect(QRect(contentsRect.x(), contentsRect.y(), contentsRect.width() >> 1, contentsRect.height()));
                    }
                    painter->translate(QRectF(contentsRect).center());
                    painter->scale(vertical ? 1 : -1, vertical ? -1 : 1);
                    painter->translate(-QRectF(contentsRect).center());
                    painter->fillRect(contentsRect, fillGradient);
                    painter->restore();
                    painter->save();
                    if (vertical) {
                        painter->setClipRect(QRect(contentsRect.x(), contentsRect.y(), contentsRect.width(), contentsRect.height() >> 1));
                    } else {
                        painter->setClipRect(QRect(contentsRect.x() + (contentsRect.width() >> 1), contentsRect.y(), contentsRect.width() - (contentsRect.width() >> 1), contentsRect.height()));
                    }
                    painter->fillRect(contentsRect, fillGradient);
                    painter->restore();
                } else {
                    painter->fillRect(contentsRect, fillGradient);
                }
                if (contentsCentered && centerSmoothing > 0) {
                    QColor centerColor = fillColor;
                    const int contentsSize = vertical ? contentsRect.height() : contentsRect.width();
                    const int centerSize = qMin(3 * chunkWidth,  contentsSize >> 1);
                    const int delta = (contentsSize - centerSize) >> 1;
                    const QRect centerRect = vertical ? contentsRect.adjusted(0, delta, 0, -delta) : contentsRect.adjusted(delta, 0, -delta, 0);
                    QLinearGradient centerGradient(centerRect.topLeft(), vertical ? centerRect.bottomLeft() : centerRect.topRight());
                    // ### work around Qt 4.5 bug
                    QColor transparentCenterColor = centerColor;
                    transparentCenterColor.setAlpha(0);
                    centerGradient.setColorAt(0.0, transparentCenterColor);
                    centerGradient.setColorAt(0.4, centerColor);
                    centerGradient.setColorAt(0.6, centerColor);
                    centerGradient.setColorAt(1.0, transparentCenterColor);
                    painter->fillRect(centerRect, centerGradient);
                }
                break;
            }
            case LiquidAnimation: {
                painter->fillRect(contentsRect, fillColor);
                if (rounding >= 0) {
                    int m = QTime(0, 0).msecsTo(QTime::currentTime());
                    rounding = (sin(m / 100.0) + 1) / 2;
                }
            }
        }
        if (rounding >= 0) {
            QLinearGradient lineGradient(contentsRect.topLeft(), vertical ? contentsRect.topRight() : contentsRect.bottomLeft());
            QColor innerColor = blend_color(option->palette.color(BG_ROLE_PROGRESS), fillColor, rounding);
            QColor outerColor = blend_color(fillColor, option->palette.color(BG_ROLE_PROGRESS), rounding);
            lineGradient.setColorAt(0.0, outerColor);
            lineGradient.setColorAt(0.5, innerColor);
            lineGradient.setColorAt(1.0, outerColor);
            if (contentsCentered || (option->version >= 2 && option->invertedAppearance)) {
                if (vertical) {
                    painter->fillRect(QRect(contentsRect.x(), contentsRect.bottom(), contentsRect.width(), 1), lineGradient);
                } else {
                    painter->fillRect(QRect(contentsRect.left(), contentsRect.y(), 1, contentsRect.height()), lineGradient);
                }
            }
            if (contentsCentered || !(option->version >= 2 && option->invertedAppearance)) {
                if (vertical) {
                    painter->fillRect(QRect(contentsRect.x(), contentsRect.top(), contentsRect.width(), 1), lineGradient);
                } else {
                    painter->fillRect(QRect(contentsRect.right(), contentsRect.y(), 1, contentsRect.height()), lineGradient);
                }
            }
        }
        if (cornerSmoothing > 0) {
            painter->save();
            QColor color = option->palette.color(BG_ROLE_PROGRESS);
            color.setAlpha(cornerSmoothing);
            painter->setPen(color);
            painter->drawPoint(contentsRect.left(), contentsRect.top());
            painter->drawPoint(contentsRect.left(), contentsRect.bottom());
            painter->drawPoint(contentsRect.right(), contentsRect.top());
            painter->drawPoint(contentsRect.right(), contentsRect.bottom());
            painter->restore();
        }
        if (borderSmoothing > 0) {
            painter->save();
            QColor frameColor = option->palette.color(BG_ROLE_PROGRESS);
            frameColor.setAlpha(borderSmoothing);
            painter->setPen(frameColor);
            painter->drawRect(contentsRect.adjusted(0, 0, -1, -1));
            painter->restore();
        }
        if (contentsFrame) {
            paintThinFrame(painter, contentsRect, option->palette, -20, 60, BG_ROLE_CHUNK);
        }
    }

    // overlay gradient
    QLinearGradient glassyGradient(option->rect.topLeft(), vertical ? option->rect.topRight() : option->rect.bottomLeft());
    glassyGradient.setColorAt(0.0, QColor(255, 255, 255, 0));
    glassyGradient.setColorAt(0.47, QColor(0, 0, 0, 2));
    glassyGradient.setColorAt(0.475, QColor(0, 0, 0, 21));
    glassyGradient.setColorAt(1.0, QColor(255, 255, 255, 0));
    painter->fillRect(option->rect.adjusted(2, 2, -2, -2), glassyGradient);
    paintRecessedFrame(painter, option->rect, option->palette, RF_Small);
}


