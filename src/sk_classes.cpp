/*
 * skulpture_classes.cpp
 *
 */


/*-----------------------------------------------------------------------*/

class WidgetExtra
{
    public:
        virtual ~WidgetExtra();

    public:
        QPalette::ColorRole oldBackgroundRole;
        QPalette::ColorRole oldForegroundRole;
};


/*-----------------------------------------------------------------------*/

class LineEditExtra : public WidgetExtra
{
    QRect cursorRect;
    QCursor oldCursor;
};


class TextEditExtra : public WidgetExtra
{
    QRect cursorRect;
    QRect focusLine;
    QRect highlightLine;
    QCursor oldCursor;
    QCursor prevCursor;
};


class TabBarExtra : public WidgetExtra
{
    QList<Tabs> tabs;
    QList<Tabs> oldTabs;
};


/*-----------------------------------------------------------------------*/

class WidgetClass
{
        void (*polish)(QWidget *widget);
        void (*unpolish)(QWidget *widget);
        void (*animate)(QWidget *widget);
        WidgetExtra *(*createExtra)(QWidget *widget);
        bool hover : 1;
        bool nosuper : 1;
        bool setbgrole : 1;
        bool setfgrole : 1;
        bool setcursor : 1;
        bool smallfont : 1;
        bool animation : 1;
        bool extra : 1;
        uint reserved : 24;
        QPalette::ColorRole backgroundRole;
        QPalette::ColorRole oldBackgroundRole;
        QPalette::ColorRole foregroundRole;
        QPalette::ColorRole oldForegroundRole;
        Qt::CursorType cursor;
        Qt::CursorType oldCursor;
        QObject *eventFilter;
        OObject *postEventFilter;
};


/*-----------------------------------------------------------------------*/

QHash<QWidget *, WidgetExtra *> widgetExtraHash;
QHash<QMetaObject *, WidgetClass *> metaObjectHash;
QHash<const QByteArray &, WidgetClass *> classNameHash;


/*-----------------------------------------------------------------------*/

inline WidgetExtra *getWidgetExtra(QWidget *widget)
{
    return widgetExtraHash.value(widget);
}


/*-----------------------------------------------------------------------*/

inline WidgetClass *findWidgetClass(QMetaObject *metaObject)
{
    WidgetClass *widgetClass = metaObjectHash.value(metaObject);

    if (!widgetClass) {
        if (const char *name = metaObject->className()) {
            widgetClass = classNameHash.value(QByteArray::fromRawData(name, qstrlen(name)));
        }
    }
    return widgetClass;
}


/*-----------------------------------------------------------------------*/

void SkulptureStyle::polish(const QWidget *widget)
{
    QMetaObject *metaObject = widget->metaObject();

    while (metaObject != widgetMetaObject) {
        if (WidgetClass *widgetClass = findWidgetClass(metaObject)) {
            if (widgetClass->hover) {
                widget->setAttribute(Qt::WA_Hover, true);
            }
            if (widgetClass->setbgrole) {
                widget->setBackgroundRole(widgetClass->backgroundRole);
            }
            if (widgetClass->smallfont) {
                QFont font;
                font.setPointSizeF(font.pointSizeF / 1.19);
                widget->setFont(font);
            }
            if (widgetClass->polish) {
                (*widgetClass->polish)(widget);
            }
            if (widgetClass->eventFilter) {
                widget->installEventFilter(widgetClass->eventFilter);
            }
            if (widgetClass->postEventFilter) {
                widget->installEventFilter(postEventFilter);
            }
            if (widgetClass->nosuper) {
                return;
            }
        }
        metaObject = metaObject->superClass();
    }
}


void SkulptureStyle::unpolish(const QWidget *widget)
{
    QMetaObject *metaObject = widget->metaObject();

    while (metaObject != widgetMetaObject) {
        if (WidgetClass *widgetClass = findWidgetClass(metaObject)) {
            if (widgetClass->postEventFilter) {
                widget->removeEventFilter(postEventFilter);
            }
            if (widgetClass->eventFilter) {
                widget->removeEventFilter(widgetClass->eventFilter);
            }
            if (widgetClass->unpolish) {
                (*widgetClass->unpolish)(widget);
            }
            if (widgetClass->smallfont) {
                QFont font;
                widget->setFont(font);
            }
            if (widgetClass->setbgrole) {
                if (widget->backgroundRole == widgetClass->backgroundRole) {
                    widget->setBackgroundRole(widgetClass->oldBackgroundRole);
                }
            }
            if (widgetClass->hover) {
                widget->setAttribute(Qt::WA_Hover, false);
            }
            if (widgetClass->nosuper) {
                return;
            }
        }
        metaObject = metaObject->superClass();
    }
}


/*-----------------------------------------------------------------------*/

void registerWidgetClass(const char *className, WidgetClass *widgetClass);
void registerWidgetClass(const QMetaObject *metaObject, WidgetClass *widgetClass);


/*-----------------------------------------------------------------------*/

void SkulptureStyle::init()
{
    registerWidgetClass(&QFrame::staticMetaObject, frameClass);
    registerWidgetClass(&QAbstractScrollArea::staticMetaObject, abstractScrollAreaClass);
    registerWidgetClass(&QAbstractItemView::staticMetaObject, abstractItemViewClass);
#if KDE4_FOUND
    registerWidgetClass(&KTitleWidget::staticMetaObject, titleWidgetClass);
#else
    registerWidgetClass("KTitleWidget", titleWidgetClass);
#endif
}


