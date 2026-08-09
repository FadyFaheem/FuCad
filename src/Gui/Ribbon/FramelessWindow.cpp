/***************************************************************************
 *   Copyright (c) 2026 FuCad contributors                                 *
 *                                                                         *
 *   This file is part of FreeCAD.                                         *
 *                                                                         *
 *   FreeCAD is free software: you can redistribute it and/or modify it    *
 *   under the terms of the GNU Lesser General Public License as           *
 *   published by the Free Software Foundation, either version 2.1 of the  *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   FreeCAD is distributed in the hope that it will be useful, but        *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU      *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with FreeCAD. If not, see                               *
 *   <https://www.gnu.org/licenses/>.                                      *
 *                                                                         *
 ***************************************************************************/


#include <array>

#include <QEvent>
#include <QMainWindow>
#include <QMouseEvent>
#include <QRect>
#include <QWidget>
#include <QWindow>

#include <App/Application.h>
#include <App/SafeMode.h>
#include <Base/Parameter.h>

#include "FramelessWindow.h"


using namespace Gui;
using namespace Gui::Ribbon;

namespace
{
constexpr int resizeBorder = 5;
/// The corners are wider than the edges so that a diagonal drag is easy to hit.
constexpr int resizeCorner = 14;
const char* const mainWindowParameters = "User parameter:BaseApp/Preferences/MainWindow";

/// The cursor that says which way a drag from \a edges will resize.
Qt::CursorShape cursorFor(Qt::Edges edges)
{
    if ((edges == (Qt::TopEdge | Qt::LeftEdge)) || (edges == (Qt::BottomEdge | Qt::RightEdge))) {
        return Qt::SizeFDiagCursor;
    }
    if ((edges == (Qt::TopEdge | Qt::RightEdge)) || (edges == (Qt::BottomEdge | Qt::LeftEdge))) {
        return Qt::SizeBDiagCursor;
    }
    if ((edges & Qt::LeftEdge) || (edges & Qt::RightEdge)) {
        return Qt::SizeHorCursor;
    }

    return Qt::SizeVerCursor;
}

/**
 * One of the strips along the window edge that a resize is dragged from.
 *
 * A widget rather than a margin of the window, because a stylesheet decides the
 * margins of everything it has a rule for and would take any margin back; a
 * child that is always on top is the one thing nothing else can claim.
 */
class ResizeGrip: public QWidget
{
public:
    ResizeGrip(QWidget* parent, Qt::Edges edges)
        : QWidget(parent)
        , edges(edges)
    {
        setObjectName(QStringLiteral("FramelessResizeGrip"));
        setCursor(cursorFor(edges));
    }

protected:
    void mousePressEvent(QMouseEvent* event) override
    {
        QWindow* handle = window() ? window()->windowHandle() : nullptr;
        if (event->button() != Qt::LeftButton || !handle) {
            QWidget::mousePressEvent(event);
            return;
        }

        // Handed to the window system rather than emulated, so the resize
        // behaves like a resize of any other window.
        handle->startSystemResize(edges);
        event->accept();
    }

private:
    Qt::Edges edges;
};
}  // namespace


bool FramelessWindow::isEnabled()
{
    // Safe mode is the way back for anyone the frameless window does not suit,
    // and the way out if it ever misbehaves on a platform.
    if (SafeMode::SafeModeEnabled()) {
        return false;
    }

    return App::GetApplication()
        .GetParameterGroupByPath(mainWindowParameters)
        ->GetBool("FramelessWindow", true);
}

int FramelessWindow::borderWidth()
{
    return resizeBorder;
}

void FramelessWindow::install(QMainWindow* window)
{
    if (!window) {
        return;
    }

    window->setWindowFlag(Qt::FramelessWindowHint, true);

    auto* frameless = new FramelessWindow(window);
    window->installEventFilter(frameless);
    frameless->layOutGrips();
}

FramelessWindow::FramelessWindow(QMainWindow* window)
    : QObject(window)
    , window(window)
{
    // Clockwise from the top, then the corners, which is the order layOutGrips
    // places them in.
    const std::array<Qt::Edges, 8> sides = {
        Qt::TopEdge,
        Qt::RightEdge,
        Qt::BottomEdge,
        Qt::LeftEdge,
        Qt::TopEdge | Qt::LeftEdge,
        Qt::TopEdge | Qt::RightEdge,
        Qt::BottomEdge | Qt::LeftEdge,
        Qt::BottomEdge | Qt::RightEdge,
    };

    for (std::size_t i = 0; i < sides.size(); ++i) {
        grips.at(i) = new ResizeGrip(window, sides.at(i));
    }
}

void FramelessWindow::layOutGrips()
{
    // A maximized or full screen window has no edge to drag.
    const bool resizable = !window->isMaximized() && !window->isFullScreen();

    const int width = window->width();
    const int height = window->height();
    const int corner = resizeCorner;
    const int border = resizeBorder;

    const std::array<QRect, 8> places = {
        QRect(corner, 0, width - 2 * corner, border),
        QRect(width - border, corner, border, height - 2 * corner),
        QRect(corner, height - border, width - 2 * corner, border),
        QRect(0, corner, border, height - 2 * corner),
        QRect(0, 0, corner, corner),
        QRect(width - corner, 0, corner, corner),
        QRect(0, height - corner, corner, corner),
        QRect(width - corner, height - corner, corner, corner),
    };

    for (std::size_t i = 0; i < grips.size(); ++i) {
        QWidget* grip = grips.at(i);
        if (!grip) {
            continue;
        }

        grip->setGeometry(places.at(i));
        grip->setVisible(resizable);
        if (resizable) {
            // Docks and views are added and restacked long after this, so the
            // grips have to climb back on top whenever the window changes.
            grip->raise();
        }
    }
}

bool FramelessWindow::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == window
        && (event->type() == QEvent::Resize || event->type() == QEvent::Show
            || event->type() == QEvent::WindowStateChange)) {
        layOutGrips();
    }

    return QObject::eventFilter(watched, event);
}

#include "moc_FramelessWindow.cpp"
