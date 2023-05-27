/*
 * skulpture_shadows.cpp
 *
 */

#include "skulpture_p.h"
#include <QtWidgets/QAbstractScrollArea>
#include <QtWidgets/QMdiArea>
#include <QtWidgets/QApplication>
#include <QtGui/QMouseEvent>
#include <QtCore/QEvent>
#include <QtGui/QPainter>


/*-----------------------------------------------------------------------*/
/**
 * FrameShadow - overlay a shadow inside a frame
 *
 * This class is used to give the impression of sunken
 * frames that cast shadows over the contents inside.
 *
 * To create the shadow, call installFrameShadow()
 * on the sunken QFrame that you want the shadow
 * to be added to.
 *
 * Side-effects:
 *
 * Adds transparent widgets to the frame, which may
 * affect performance.
 *
 */

FrameShadow::FrameShadow(QWidget *parent)
    : QWidget(parent)
{
    init();
}


FrameShadow::FrameShadow(ShadowArea area, QWidget *parent)
    : QWidget(parent)
{
    init();
    area_ = area;
}


FrameShadow::~FrameShadow()
{
    /* */
}


/*-----------------------------------------------------------------------*/

void SkulptureStyle::Private::installFrameShadow(QWidget *widget)
{
//	printf("adding shadow to %s\n", widget->metaObject()->className());
    widget->installEventFilter(this);
    removeFrameShadow(widget);
    for (int i = 0; i < 4; ++i) {
        FrameShadow *shadow = new FrameShadow(FrameShadow::ShadowArea(i));
        shadow->hide();
        shadow->setParent(widget);
        shadow->updateGeometry();
        shadow->show();
    }
}


void SkulptureStyle::Private::removeFrameShadow(QWidget *widget)
{
    const QList<QObject *> shadows = widget->children();
    Q_FOREACH (QObject *child, shadows) {
        FrameShadow *shadow;
        if ((shadow = qobject_cast<FrameShadow *>(child))) {
            shadow->hide();
            shadow->setParent(0);
            shadow->deleteLater();
        }
    }
}


void SkulptureStyle::Private::updateFrameShadow(QWidget *widget)
{
    const QList<QObject *> shadows = widget->children();
    Q_FOREACH (QObject *child, shadows) {
        FrameShadow *shadow;
        if ((shadow = qobject_cast<FrameShadow *>(child))) {
            if (shadow->isVisible()) {
                shadow->updateGeometry();
            }
        }
    }
}


/*-----------------------------------------------------------------------*/

#define SHADOW_SIZE_TOP		4
#define SHADOW_SIZE_BOTTOM	2
#define SHADOW_SIZE_LEFT		4
#define SHADOW_SIZE_RIGHT		4

void FrameShadow::updateGeometry()
{
    QWidget *widget = parentWidget();
    QRect cr = widget->contentsRect();

//	printf("cr-top: %d in class %s\n", cr.top(), widget->metaObject()->className());
    switch (shadowArea()) {
        case FrameShadow::Top:
            cr.setHeight(SHADOW_SIZE_TOP);
            break;
        case FrameShadow::Left:
            cr.setWidth(SHADOW_SIZE_LEFT);
            cr.adjust(0, SHADOW_SIZE_TOP, 0, -SHADOW_SIZE_BOTTOM);
            break;
        case FrameShadow::Bottom:
            cr.setTop(cr.bottom() - SHADOW_SIZE_BOTTOM + 1);
            break;
        case FrameShadow::Right:
            cr.setLeft(cr.right() - SHADOW_SIZE_RIGHT + 1);
            cr.adjust(0, SHADOW_SIZE_TOP, 0, -SHADOW_SIZE_BOTTOM);
            break;
    }
    setGeometry(cr);
}


/*-----------------------------------------------------------------------*/

bool FrameShadow::event(QEvent *e)
{
    if (e->type() == QEvent::Paint) {
        return QWidget::event(e);
    }
    QWidget *viewport = 0;
    if (parentWidget()) {
        if (QAbstractScrollArea *widget = qobject_cast<QAbstractScrollArea *>(parentWidget())) {
            viewport = widget->viewport();
        } else if (parentWidget()->inherits("Q3ScrollView")) {
            // FIXME: get viewport? needs Qt3Support linkage!
            viewport = 0;
        } else {
            viewport = 0;
        }
    }
    if (!viewport) {
        return false;
    }

#if 1
    switch (e->type()) {
        case QEvent::DragEnter:
        case QEvent::DragMove:
        case QEvent::DragLeave:
            case QEvent::Drop: {
                setAcceptDrops(viewport->acceptDrops());
                QObject *o = viewport;
                return o->event(e);
            }
            break;
        case QEvent::Enter:
            setCursor(viewport->cursor());
            setAcceptDrops(viewport->acceptDrops());
            break;
            case QEvent::ContextMenu: {
                QContextMenuEvent *me = reinterpret_cast<QContextMenuEvent *>(e);
                QContextMenuEvent *ne = new QContextMenuEvent(me->reason(), parentWidget()->mapFromGlobal(me->globalPos()), me->globalPos());
                QApplication::sendEvent(viewport, ne);
                e->accept();
                return true;
            }
            break;
        case QEvent::MouseButtonPress:
            releaseMouse();
        case QEvent::MouseMove:
            case QEvent::MouseButtonRelease: {
                QMouseEvent *me = reinterpret_cast<QMouseEvent *>(e);
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
                QPoint globalPos = me->globalPos();
#else
                QPoint globalPos = me->globalPosition().toPoint();
#endif
                QMouseEvent *ne = new QMouseEvent(e->type(), parentWidget()->mapFromGlobal(globalPos), globalPos, me->button(), me->buttons(), me->modifiers());
                QApplication::sendEvent(viewport, ne);
                e->accept();
                return true;
            }
            break;
        case QEvent::Paint:
            return QWidget::event(e);
        default:
            break;
    }
    e->ignore();
    return false;
#else
    return QWidget::event(e);
#endif
}


/*-----------------------------------------------------------------------*/

void FrameShadow::paintEvent(QPaintEvent *)
{
    // this fixes shadows in frames that change frameStyle() after polish()
    if (QFrame *frame = qobject_cast<QFrame *>(parentWidget())) {
        if (frame->frameStyle() != (QFrame::StyledPanel | QFrame::Sunken)) {
            return;
        }
    }
    QPainter painter(this);
    QWidget *parent = parentWidget();
    QRect r = parent->contentsRect();
    r.translate(mapFromParent(QPoint(0, 0)));
//	painter.fillRect(QRect(-100, -100, 1000, 1000), Qt::red);
    paintRecessedFrameShadow(&painter, r, RF_Large);
}


/*-----------------------------------------------------------------------*/

void FrameShadow::init()
{
//	setAttribute(Qt::WA_NoSystemBackground, true);
//	setAttribute(Qt::WA_NoBackground, true);
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    setFocusPolicy(Qt::NoFocus);
    // TODO: check if this is private
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    setContextMenuPolicy(Qt::NoContextMenu);

    QWidget *viewport;
    if (parentWidget()) {
        if (QAbstractScrollArea *widget = qobject_cast<QAbstractScrollArea *>(parentWidget())) {
            setAcceptDrops(true);
            viewport = widget->viewport();
        } else if (parentWidget()->inherits("Q3ScrollView")) {
            // FIXME: get viewport? needs Qt3Support linkage!
            viewport = parentWidget();
        } else {
            viewport = 0;
        }
        if (viewport) {
            setCursor(viewport->cursor());
        }
    }
}


/*-----------------------------------------------------------------------*/

void paintRecessedFrameShadow(QPainter *painter, const QRect &rect, enum RecessedFrame rf)
{
    if (rf == RF_None) return;
#if 1
    int c1 = (rf == RF_Small) ? 10 : 10;
    int c2 = (rf == RF_Small) ? 24 : 36;
#else
    int c1 = 0;
    int c2 = 0;
#endif
#if 0
    c1 += c1 >> 1;
    c2 += c2 >> 1;
    int intensityTop = c2;
    int intensityLeft = c2;
    int intensityBottom = c1;
    int intensityRight = c1;
#endif
    QRect r = rect;
    while (c1 > 3 || c2 > 3) {
        QBrush brush1(QColor(0, 0, 0, c1));
        QBrush brush2(QColor(0, 0, 0, c2));

        painter->fillRect(QRect(rect.left(), r.top(), rect.width(), 1), brush2);
        painter->fillRect(QRect(r.left(), rect.top(), 1, rect.height()), brush2);
        painter->fillRect(QRect(rect.left(), r.bottom(), rect.width(), 1), brush1);
        painter->fillRect(QRect(r.right(), rect.top(), 1, rect.height()), brush1);
        c1 >>= 1; c2 >>= 1;
//		c1 = int(c1 * 0.7); c2 = int(c2 * 0.7);
        r.adjust(1, 1, -1, -1);
    }
}


/*-----------------------------------------------------------------------*/
/**
 * WidgetShadow - add a drop shadow to a widget
 *
 * This class renders a shadow below a widget, such
 * as a QMdiSubWindow.
 *
 */

WidgetShadow::WidgetShadow(QWidget *parent)
    : QWidget(parent)
{
    init();
}


/*-----------------------------------------------------------------------*/

void WidgetShadow::init()
{
    setObjectName(QLatin1String("WidgetShadow"));
//	setAttribute(Qt::WA_NoSystemBackground, true);
//	setAttribute(Qt::WA_NoBackground, true);
    setAttribute(Qt::WA_OpaquePaintEvent, false);
//	setAutoFillBackground(false);
    setFocusPolicy(Qt::NoFocus);
    // TODO: check if this is private
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
//	setContextMenuPolicy(Qt::NoContextMenu);
    widget_ = 0;
}


/*-----------------------------------------------------------------------*/

bool WidgetShadow::event(QEvent *e)
{
    switch (e->type())
    {
        case QEvent::Paint: if (widget_) {
            QRect r(- 10, - 5, widget_->frameGeometry().width() + 20, widget_->frameGeometry().height() + 15);
            r.translate(qMin(widget_->x(), 10), qMin(widget_->y(), 5));
            QPainter p(this);
            QRegion region(r);
            region -= QRect(r.adjusted(10, 5, -10, -10));
            p.setClipRegion(region);
#if 0
            for (int i = 0; i < 10; ++i) {
            int k = 9 - i;
            p.fillRect(r.adjusted(k, i, -k, -i), QColor(0, 0, 0, i));
            p.fillRect(r.adjusted(i, k, -i, -k), QColor(0, 0, 0, i));
        }
#else
            p.setRenderHints(QPainter::Antialiasing, true);
            for (int i = 0; i < 10; ++i) {
                p.setBrush(QColor(0, 0, 0, 2 + i));
                p.setPen(Qt::NoPen);
                p.drawRoundedRect(r, 7, 7);
                r.adjust(1, 1, -1, -1);
            }
#endif
            e->ignore();
            return (true);
        }
        default:
            break;
    }
    return QWidget::event(e);
}


void WidgetShadow::updateGeometry()
{
    if (widget_) {
        if (widget_->isHidden()) {
            hide();
        } else {
            QWidget *parent = parentWidget();
                        if (parent && !qobject_cast<QMdiArea *>(parent) && qobject_cast<QMdiArea *>(parent->parentWidget())) {
                parent = parent->parentWidget();
            }
            if (parent) {
                if (qobject_cast<QAbstractScrollArea *>(parent)) {
                    parent = qobject_cast<QAbstractScrollArea *>(parent)->viewport();
                }
                QRect geo(widget_->x() - 10, widget_->y() - 5, widget_->frameGeometry().width() + 20, widget_->frameGeometry().height() + 15);
                setGeometry(geo & parent->rect());
            }
            show();
        }
    }
}


void WidgetShadow::updateZOrder()
{
    if (widget_) {
        if (widget_->isHidden()) {
            hide();
        } else {
            stackUnder(widget_);
            QWidget *parent = parentWidget();
            if (parent && !qobject_cast<QMdiArea *>(parent) && qobject_cast<QMdiArea *>(parent->parentWidget())) {
                parent = parent->parentWidget();
            }
            if (parent) {
                if (qobject_cast<QAbstractScrollArea *>(parent)) {
                    parent = qobject_cast<QAbstractScrollArea *>(parent)->viewport();
                }
                QRect geo(widget_->x() - 10, widget_->y() - 5, widget_->frameGeometry().width() + 20, widget_->frameGeometry().height() + 15);
                setGeometry(geo & parent->rect());
            }
            show();
        }
    }
}


