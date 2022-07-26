/*
 * skulpture_complex.cpp
 *
 */

#include "skulpture_p.h"


/*-----------------------------------------------------------------------*/

extern QRect subControlRectSpinBox(const QStyleOptionSpinBox *option, QStyle::SubControl subControl, const QWidget *widget, const QStyle *style);
extern QRect subControlRectScrollBar(const QStyleOptionSlider *option, QStyle::SubControl subControl, const QWidget *widget, const QStyle *style, ArrowPlacementMode horizontalArrowMode, ArrowPlacementMode verticalArrowMode);
extern QRect subControlRectSlider(const QStyleOptionSlider *option, QStyle::SubControl subControl, const QWidget *widget, const QStyle *style);
extern QRect subControlRectToolButton(const QStyleOptionToolButton *option, QStyle::SubControl subControl, const QWidget *widget, const QStyle *style);
extern QRect subControlRectComboBox(const QStyleOptionComboBox *option, QStyle::SubControl subControl, const QWidget *widget, const QStyle *style);
extern QRect subControlRectGroupBox(const QStyleOptionGroupBox *option, QStyle::SubControl subControl, const QWidget *widget, const QStyle *style);
extern QRect subControlRectTitleBar(const QStyleOptionTitleBar *option, QStyle::SubControl subControl, const QWidget *widget, const QStyle *style);


/*-----------------------------------------------------------------------*/

#define SC_CASE(cc, so) \
    case CC_## cc: \
        if (option->type == QStyleOption::SO_## so) { \
            return subControlRect ## cc((const QStyleOption ## so *) option, subControl, widget, this); \
        } \
        break


QRect SkulptureStyle::subControlRect(ComplexControl control, const QStyleOptionComplex *option, SubControl subControl, const QWidget *widget) const
{
    switch (control) {
        SC_CASE(SpinBox, SpinBox);
        SC_CASE(ComboBox, ComboBox);
        case CC_ScrollBar:
            if (option && option && option->type == QStyleOption::SO_Slider) {
                return subControlRectScrollBar((const QStyleOptionSlider *) option, subControl, widget, this, d->horizontalArrowMode, d->verticalArrowMode);
            }
            break;
        SC_CASE(Slider, Slider);
        SC_CASE(TitleBar, TitleBar);
#ifdef QT3_SUPPORT
        case CC_Q3ListView: break;
#endif
        SC_CASE(ToolButton, ToolButton);
        case CC_Dial: break;
        SC_CASE(GroupBox, GroupBox);
        case CC_MdiControls:
            break;
        case CC_CustomBase: // avoid warning
            break;
    }
    return ParentStyle::subControlRect(control, option, subControl, widget);
}


/*-----------------------------------------------------------------------*/

extern QStyle::SubControl hitTestComplexControlScrollBar(const QStyleOptionSlider *option, const QPoint &position, const QWidget *widget, const QStyle *style, ArrowPlacementMode horizontalArrowMode, ArrowPlacementMode verticalArrowMode);


/*-----------------------------------------------------------------------*/

#define HIT_CASE(cc, so) \
    case CC_## cc: \
        if (option->type == QStyleOption::SO_## so) { \
            return hitTestComplexControl ## cc((const QStyleOption ## so *) option, position, widget, this); \
        } \
        break


QStyle::SubControl SkulptureStyle::hitTestComplexControl(ComplexControl control, const QStyleOptionComplex *option, const QPoint &position, const QWidget *widget) const
{
    switch (control) {
        case CC_ScrollBar:
            if (option->type == QStyleOption::SO_Slider) {
                return hitTestComplexControlScrollBar((const QStyleOptionSlider *) option, position, widget, this, d->horizontalArrowMode, d->verticalArrowMode);
            }
            break;
        default:
            break;
    }
    return ParentStyle::hitTestComplexControl(control, option, position, widget);
}


/*-----------------------------------------------------------------------*/

extern void paintSpinBox(QPainter *painter, const QStyleOptionSpinBox *option, const QWidget *widget, const QStyle *style);
extern void paintComboBox(QPainter *painter, const QStyleOptionComboBox *option, const QWidget *widget, const QStyle *style);
extern void paintScrollBar(QPainter *painter, const QStyleOptionSlider *option, const QWidget *widget, const QStyle *style, ArrowPlacementMode horizontalArrowMode, ArrowPlacementMode verticalArrowMode);
extern void paintSlider(QPainter *painter, const QStyleOptionSlider *option, const QWidget *widget, const QStyle *style);
extern void paintToolButton(QPainter *painter, const QStyleOptionToolButton *option, const QWidget *widget, const QStyle *style);
extern void paintTitleBar(QPainter *painter, const QStyleOptionTitleBar *option, const QWidget *widget, const QStyle *style);
#ifdef QT3_SUPPORT
extern void paintQ3ListView(QPainter *painter, const QStyleOptionQ3ListView *option, const QWidget *widget, const QStyle *style);
#endif
extern void paintDial(QPainter *painter, const QStyleOptionSlider *option, const QWidget *widget, const QStyle *style);
//extern void paintGroupBox(QPainter *painter, const QStyleOptionGroupBox *option, const QWidget *widget, const QStyle *style);
//extern void paintMdiControls(QPainter *painter, const QStyleOptionComplex *option, const QWidget *widget, const QStyle *style);


/*-----------------------------------------------------------------------*/

#define CC_CASE(cc, so) \
    case CC_## cc: \
        if (option->type == QStyleOption::SO_## so) { \
            paint ## cc(painter, (const QStyleOption ## so *) option, widget, this); \
            return; \
        } \
        break


void SkulptureStyle::drawComplexControl(ComplexControl control, const QStyleOptionComplex *option,
                                        QPainter *painter, const QWidget *widget) const
{
    switch (control) {
        CC_CASE(SpinBox, SpinBox);
        CC_CASE(ComboBox, ComboBox);
        case CC_ScrollBar:
            if (option->type == QStyleOption::SO_Slider) {
                paintScrollBar(painter, (const QStyleOptionSlider *) option, widget, this, d->horizontalArrowMode, d->verticalArrowMode);
                return;
            }
            break;
        CC_CASE(Slider, Slider);
        CC_CASE(ToolButton, ToolButton);
        CC_CASE(TitleBar, TitleBar);
#ifdef QT3_SUPPORT
        CC_CASE(Q3ListView, Q3ListView);
#endif
        CC_CASE(Dial, Slider);
        case CC_GroupBox:
            break;
        case CC_MdiControls:
            break;
        case CC_CustomBase: // avoid warning
            break;
    }
    ParentStyle::drawComplexControl(control, option, painter, widget);
}


