/*
 * skulpture_text.cpp
 *
 */

#include "skulpture_p.h"
#include <QtGui/QPainter>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QTextBrowser>
#include <QtGui/QTextFrame>
#include <QtGui/QTextCursor>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QApplication>


/*-----------------------------------------------------------------------*/

void SkulptureStyle::Private::removeCursorLine(QAbstractScrollArea *edit)
{
    Q_UNUSED (edit);

    if (oldEdit) {
        oldEdit->viewport()->update(QRect(0, oldCursorTop, oldCursorWidth, oldCursorHeight));
        oldEdit = 0;
    }
}


void SkulptureStyle::Private::updateCursorLine(QAbstractScrollArea *edit, const QRect &cursorRect)
{
    const int highlightMargin = qMin(2, widgetSize);
    QRect cursorLine = cursorRect;
    cursorLine.setLeft(0);
    cursorLine.setWidth(edit->viewport()->width());
    cursorLine.adjust(0, -highlightMargin, 0, highlightMargin);
    if (edit != oldEdit || cursorLine.top() != oldCursorTop
     || cursorLine.width() != oldCursorWidth
     || cursorLine.height() != oldCursorHeight
     || edit->viewport()->height() != oldHeight) {
        removeCursorLine(edit);
        oldEdit = edit;
        oldCursorTop = cursorLine.top();
        oldCursorWidth = cursorLine.width();
        oldCursorHeight = cursorLine.height();
        oldHeight = edit->viewport()->height();
        edit->viewport()->update(cursorLine);
    }
}


void SkulptureStyle::Private::paintCursorLine(QAbstractScrollArea *edit)
{
    if (edit == oldEdit) {
        QRect cursorLine = QRect(0, oldCursorTop, oldCursorWidth, oldCursorHeight);
        QPainter painter(edit->viewport());
        QPalette palette = edit->palette();
        QColor color = palette.color(QPalette::Highlight);
        color.setAlpha(40);
        painter.fillRect(cursorLine, color);
        if (edit->window()->testAttribute(Qt::WA_KeyboardFocusChange)) {
            color = palette.color(QPalette::Highlight).darker(120);
            color.setAlpha(120);
            painter.fillRect(cursorLine.adjusted(0, cursorLine.height() - 3, 0, -2), color);
        }
    }
}


void SkulptureStyle::Private::handleCursor(QPlainTextEdit *edit)
{
    if (edit->hasFocus() && !edit->isReadOnly()) {
        QStyleOption option;
        option.initFrom(edit);
        int cursorWidth = q->SkulptureStyle::pixelMetric(PM_TextCursorWidth, &option, edit);
        if (edit->cursorWidth() != cursorWidth) {
            edit->setCursorWidth(cursorWidth);
        }
        updateCursorLine(edit, edit->cursorRect());
    } else {
        if (edit == oldEdit) {
            removeCursorLine(edit);
        }
    }
}


void SkulptureStyle::Private::handleCursor(QTextEdit *edit)
{
    if (edit->hasFocus() && !edit->isReadOnly()) {
        QStyleOption option;
        option.initFrom(edit);
        int cursorWidth = q->SkulptureStyle::pixelMetric(PM_TextCursorWidth, &option, edit);
        if (edit->cursorWidth() != cursorWidth) {
            edit->setCursorWidth(cursorWidth);
        }
        updateCursorLine(edit, edit->cursorRect());
    } else {
        if (edit == oldEdit) {
            removeCursorLine(edit);
        }
    }
}


/*-----------------------------------------------------------------------*/

void SkulptureStyle::Private::updateTextEditMargins(QTextEdit *edit)
{
	int margin = 1 + edit->fontMetrics().height() / 5;
	if (margin > 4) margin = 4;
	if (qobject_cast<QTextBrowser *>(edit)) {
		margin = edit->fontMetrics().height();
		if (margin < 4 || edit->height() < 4 * edit->fontMetrics().height()) {
			margin = 4;
		}
	}
	if (margin < 2 || edit->height() < 2 * edit->fontMetrics().height()) {
		margin = 2;
	}
	QTextDocument *doc = edit->document();
//	printf("doc: %p\n", doc);
	if (!doc) return;
	if (doc->isEmpty()) {
		// create valid root frame
		QTextCursor cursor(doc);
	}

	QTextFrame *root = doc->rootFrame();
//	printf("root: %p\n", root);
	if (!root) return;

	QTextFrameFormat format = root->frameFormat();
	if (!format.isValid()) return;

	if (format.margin() == 2.0 && margin != 2) {
	//	printf("set margin %d\n", margin);
		// ### crash on setText(), disable signals
		disconnect(edit, SIGNAL(textChanged()), &mapper, SLOT(map()));
		doc->blockSignals(true);
		format.setMargin(margin);
                if (margin < 12) {
                    format.setTopMargin(widgetSize - ((textShift + 1) >> 1));
                    format.setBottomMargin(widgetSize + ((textShift + 1) >> 1));
                }
                root->setFrameFormat(format);
	//	edit->insertPlainText(QLatin1String(""));
	//	edit->update();
		doc->blockSignals(false);
		connect(edit, SIGNAL(textChanged()), &mapper, SLOT(map()));
		// clear undo buffer
		bool undo = edit->isUndoRedoEnabled();
		edit->setUndoRedoEnabled(false);
		doc->setModified(false);
	//	emit doc->contentsChanged();
		edit->setUndoRedoEnabled(undo);
		// force relayout
		edit->resize(edit->size() + QSize(-1, 0));
		edit->resize(edit->size() + QSize(1, 0));
	}
}


void SkulptureStyle::Private::textEditSourceChanged(QWidget *widget)
{
	if (QTextEdit *edit = qobject_cast<QTextEdit *>(widget)) {
		updateTextEditMargins(edit);
	}
}


/*-----------------------------------------------------------------------*/

QRect SkulptureStyle::itemTextRect(const QFontMetrics &metrics, const QRect &rectangle, int alignment, bool enabled, const QString &text) const
{
	Q_UNUSED(enabled);
	return ParentStyle::itemTextRect(metrics, rectangle, alignment, true, text);
}


void SkulptureStyle::drawItemText(QPainter * painter, const QRect &rectangle, int alignment, const QPalette &palette, bool enabled, const QString &text, QPalette::ColorRole textRole) const
{
    int textShift = 0;

    if (!(alignment & (Qt::AlignTop | Qt::AlignBottom))) {
        textShift = d->verticalTextShift(painter->fontMetrics());
            if (textShift & 1 && ((painter->fontMetrics().height() ^ rectangle.height()) & 1)) {
                textShift -= 1;
            }
    }
    ParentStyle::drawItemText(painter, textShift == 0 || textShift == -1 ? rectangle : rectangle.adjusted(0, (-textShift) >> 1, 0, (-textShift) >> 1), alignment, palette, enabled, text, textRole);
}


