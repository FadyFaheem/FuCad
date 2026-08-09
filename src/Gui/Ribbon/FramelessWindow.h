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


#pragma once

#include <array>

#include <QObject>

#include <FCGlobal.h>

class QMainWindow;
class QWidget;

namespace Gui
{
namespace Ribbon
{

/**
 * Takes the system title bar off the main window so that the ribbon's app bar
 * can be the title bar, the way Fusion has one row where other applications have
 * two.
 *
 * Losing the frame means losing what the frame did, so this puts it back: a
 * border around the window that starts a resize, and a caption area that starts
 * a move. Both are handed to the window system rather than emulated, so dragging
 * to an edge still snaps and dragging a corner still resizes the way it does for
 * every other window.
 * @author FuCad contributors
 */
class GuiExport FramelessWindow: public QObject
{
    Q_OBJECT

public:
    /**
     * Whether the main window drops the system frame. Off in safe mode, which is
     * the way back for anyone the frameless window does not suit.
     */
    static bool isEnabled();

    /// Drops the frame of \a window and takes over moving and resizing it.
    static void install(QMainWindow* window);

    /// The height of the strip along each edge that starts a resize.
    static int borderWidth();

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    explicit FramelessWindow(QMainWindow* window);

    /**
     * Puts the grips back along the edges and on top of everything, and takes
     * them away while the window is maximized and has no edge to drag.
     */
    void layOutGrips();

    QMainWindow* window;
    /// Four edges then four corners, owned by the window they sit on.
    std::array<QWidget*, 8> grips {};

    Q_DISABLE_COPY(FramelessWindow)
};

}  // namespace Ribbon
}  // namespace Gui
