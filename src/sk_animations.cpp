/*
 * skulpture_animations.cpp
 *
 */

#include "skulpture_p.h"
#include <QtWidgets/QProgressBar>
#include <QtCore/QTimerEvent>


/*-----------------------------------------------------------------------*/

void SkulptureStyle::Private::setAnimated(QWidget *widget, bool animated)
{
	if (!widget) {
		return;
	}

	animations.removeAll(widget);
	if (animated && animateProgressBars) {
		animations.prepend(widget);
		if (!timer) {
			timer = startTimer(60);
		}
	} else {
		if (animations.isEmpty()) {
			if (timer) {
				killTimer(timer);
				timer = 0;
			}
		}
	}
}


bool SkulptureStyle::Private::isAnimated(QWidget *widget)
{
	if (!widget || !timer) {
		return false;
	}

	return animations.contains(widget);
}


void SkulptureStyle::Private::timerEvent(QTimerEvent *event)
{
	if (event->timerId() == timer) {
		Q_FOREACH (QWidget *widget, animations) {
			// FIXME: move this logic to progressbar
			QProgressBar *bar = qobject_cast<QProgressBar *>(widget);
			if (bar) {
				if (bar->minimum() >= bar->maximum()
				 || bar->value() < bar->maximum()) {
					bar->update();
				}
			} else {
				widget->update();
			}
		}
	}
	event->ignore();
}


