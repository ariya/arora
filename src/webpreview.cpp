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
const int preview_ofs = 10;

const qreal preview_opacity = 0.8;


WebPreview::WebPreview(QWidget *parent)
#ifdef WEBPREVIEW_OPENGL
    : QGLWidget(parent)
#else
    : QWidget(parent)
#endif
    , m_view(0)
    , m_scalingFactor(0.25)
{
    m_pixmap = QPixmap(preview_width, preview_height);

    m_showTimer.setInterval(500);
    m_showTimer.setSingleShot(true);
    connect(&m_showTimer, SIGNAL(timeout()),
            this, SLOT(show()));

    m_repaintTimer.setInterval(100);
    connect(&m_repaintTimer, SIGNAL(timeout()),
            this, SLOT(updatePreview()));

    setFixedSize(preview_width + 4, preview_height + preview_ofs + 4);
}

void WebPreview::track(QWebView *view)
{
    if (view == m_view)
        return;

    m_view = view;
    if (!m_view) {
        m_showTimer.stop();
        m_repaintTimer.stop();
        hide();
    } else {
        m_repaintTimer.start();

        qreal xf = qreal(m_pixmap.width()) / m_view->width();
        qreal yf = qreal(m_pixmap.height()) / m_view->height();
        m_scalingFactor = qMax(xf, yf);

        m_pixmap.fill(m_view->palette().color(QPalette::Background));

        if (!isVisible())
            m_showTimer.start();
    }
}

void WebPreview::updatePreview(const QRect& rect)
{
    if (!m_view)
        return;

    QRect r = rect.isEmpty() ? m_view->rect() : rect;

    QPainter p(&m_pixmap);

    // comment the next line for old & slow system
    p.setRenderHint(QPainter::SmoothPixmapTransform, true);

    p.scale(m_scalingFactor, m_scalingFactor);
    m_view->page()->mainFrame()->render(&p, r);
    p.end();

    update();
}

void WebPreview::paintEvent(QPaintEvent *event)
{
#ifdef WEBPREVIEW_OPENGL
    QGLWidget::paintEvent(event);
#else
    QWidget::paintEvent(event);
#endif

    QColor bgcolor = Qt::white;
    if (m_view)
        bgcolor = m_view->palette().color(QPalette::Background);

    QPolygon polygon;
    polygon << QPoint(0, preview_ofs);
    polygon << QPoint(preview_ofs * 2, preview_ofs);
    polygon << QPoint(preview_ofs * 2, 0);
    polygon << QPoint(preview_ofs * 3, preview_ofs);
    polygon << QPoint(preview_width + 2, preview_ofs);
    polygon << QPoint(preview_width + 2, preview_height + preview_ofs + 2);
    polygon << QPoint(0, preview_height + preview_ofs + 2);
    polygon << QPoint(0, preview_ofs);

    QPainter p(this);
    p.setOpacity(preview_opacity);

    p.setBrush(bgcolor);
    p.drawPolygon(polygon);
    p.drawPixmap(1, preview_ofs + 1, m_pixmap);

    p.setBrush(QBrush());
    p.setPen(Qt::gray);
    p.drawPolygon(polygon);
    p.end();
}

