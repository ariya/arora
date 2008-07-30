/*
 * Copyright 2008 Ariya Hidayat <ariya.hidayat@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

#include "webpreview.h"

#include <qwebpage.h>
#include <qwebview.h>
#include <qwebframe.h>

#include <QtGui>

const int preview_width = 200;
const int preview_height = 150;
const int preview_margin = 10;


WebPreview::WebPreview(QWidget *parent)
    : QFrame(parent)
    , m_view(0)
{
    m_repaintTimer.setInterval(100);
    connect(&m_repaintTimer, SIGNAL(timeout()),
            this, SLOT(updatePreview()));
    setWindowFlags(windowFlags() | Qt::ToolTip);

    setFrameStyle(QFrame::Box | QFrame::Raised);
    setLineWidth(1);

    int w = preview_width + 2 * preview_margin;
    int h = preview_height + 2 * preview_margin;
    setFixedSize(w, h);
}

void WebPreview::track(QWebView *view)
{
    if (view == m_view)
        return;

    m_view = view;
    if (!m_view) {
        m_repaintTimer.stop();
        hide();
    } else {
        m_repaintTimer.start();

        if (m_pixmap.size() != m_view->size()) {
            int w = m_view->width();
            int h = m_view->height();
            QString key = QString("arora_webpreview_%1_%2").arg(w).arg(h);
            QPixmap pm;
            if (!QPixmapCache::find(key, pm)) {
                pm = QPixmap(m_view->size());
                QPixmapCache::insert(key, pm);
            }
            m_pixmap = pm;
        }

        qreal xf = m_pixmap.width() / preview_width;
        qreal yf = m_pixmap.height() / preview_height;
        qreal factor = qMin(xf, yf);
        m_thumbnail = QPixmap(m_pixmap.size() / factor);

        if (!isVisible())
            QTimer::singleShot(500, this, SLOT(show()));
    }
}

void WebPreview::updatePreview(const QRect& rect)
{
    if (!m_view)
        return;

    QRect r = rect.isEmpty() ? m_view->rect() : rect;

    QPainter p(&m_pixmap);
    m_view->page()->mainFrame()->render(&p, r);
    p.end();

    m_thumbnail = m_pixmap.scaled(m_thumbnail.size(), Qt::IgnoreAspectRatio,
                                  Qt::SmoothTransformation);

    update();
}

void WebPreview::paintEvent(QPaintEvent *event)
{
    QFrame::paintEvent(event);


    QPoint topLeft(preview_margin, preview_margin);
    QRect sourceRect(0, 0, preview_width, preview_height);

    QPainter p(this);
    p.drawPixmap(topLeft, m_thumbnail, sourceRect);
    p.end();
}

