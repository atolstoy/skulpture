/*
 * kwin_skulpture.h - Skulpture window decoration for KDE
 *
 * Copyright (c) 2008 Christoph Feck <christoph@maxiom.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef KWIN_SKULPTURE_H
#define KWIN_SKULPTURE_H 1


/*-----------------------------------------------------------------------*/

//#include <kcommondecoration.h>
class QStyleOptionButton;

class QtMdiDecorationButton : public KCommonDecorationButton
{
	Q_OBJECT

	public:
		QtMdiDecorationButton(ButtonType type, KCommonDecoration *parent);
		virtual ~QtMdiDecorationButton();
		virtual void reset(unsigned long changed);

	protected:
		virtual void paintEvent(QPaintEvent *event);

	protected:
		void initStyleOption(QStyleOptionButton &opt);

	private:
		void init();
};


/*-----------------------------------------------------------------------*/

class QStyleOption;

class QtMdiDecoration : public KCommonDecoration
{
	Q_OBJECT

	public:
		QtMdiDecoration(KDecorationBridge *bridge, KDecorationFactory *factory, QStyle *style = 0);
		virtual ~QtMdiDecoration();

	public:
		virtual QString visibleName() const;
		virtual bool decorationBehaviour(DecorationBehaviour behaviour) const;
		virtual int layoutMetric(LayoutMetric lm, bool respectWindowState = true, const KCommonDecorationButton *button = 0) const;
		virtual void updateWindowShape();
		virtual void updateCaption();

		virtual void init();
		virtual KCommonDecorationButton *createButton(ButtonType type);

		virtual void paintEvent(QPaintEvent *event);

                bool useIcon() const { return showIcon; }
                int onScreen() const { return paintOnScreen; }
                int iconSize;
                qreal symbolSize;

	protected:
		void initStyleOption(QStyleOption &opt);
		QPixmap createShadowPixmap() const;

	private:
		/* cached style hints and pixel metrics */
		bool coloredFrame : 1;
		bool contrastFrame : 1;
		bool renderShadows : 1;
		bool noBorder : 1;
		bool autoRaise : 1;
                bool showIcon: 1;
                bool onlyActiveFrame : 1;
                bool centerFullWidth : 1;
                int paintOnScreen;
                int textShift;
                int textShiftMaximized;
                Qt::Alignment titleAlignment;
		int titleHeight;
		int borderWidth;
                int borderHeight;
                QStyle *style;
		QPixmap shadowPixmap;
};


/*-----------------------------------------------------------------------*/

//#include <kdecorationfactory.h>

class SkulptureDecorationFactory : public KDecorationFactory
{
	public:
		SkulptureDecorationFactory();
		virtual ~SkulptureDecorationFactory();

	public:
		virtual KDecoration *createDecoration(KDecorationBridge *bridge);
		virtual bool reset(unsigned long changed);
		virtual bool supports(Ability ability) const;

        private:
                QStyle *style;
};


/*-----------------------------------------------------------------------*/

#endif


