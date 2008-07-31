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

#ifndef WEBPREVIEW_H
#define WEBPREVIEW_H

#include <QGLWidget>
#include <QPixmap>
#include <QPointer>
#include <QTimer>
#include <QWidget>

// uncomment to enable OpenGL-based preview widget
// should be faster on system with hardware accelerated graphics driver
// #define WEBPREVIEW_OPENGL

class QWebView;

#ifdef WEBPREVIEW_OPENGL
class WebPreview : public QGLWidget
#else
class WebPreview : public QWidget
#endif
{
    Q_OBJECT

public:
    WebPreview(QWidget *parent = 0);

    void track(QWebView *view);

protected:
    void paintEvent(QPaintEvent *event);

private:
    QPointer<QWebView> m_view;
    QPixmap m_pixmap;
    qreal m_scalingFactor;
    QTimer m_showTimer;
    QTimer m_repaintTimer;

private slots:
    void updatePreview(const QRect &rect = QRect());
};

#endif // WEBPREVIEW_H

