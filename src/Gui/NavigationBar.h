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

#include <QToolBar>

#include <FCGlobal.h>

namespace Gui
{

/**
 * The strip Fusion floats along the bottom of the canvas: fitting, the standard
 * views, the display style and the projection, within reach of the model rather
 * than up in the application chrome.
 *
 * It is a QToolBar so that a group command such as the standard views arrives
 * with the drop-down the command framework builds for a toolbar, but it is never
 * added to a toolbar area: it parents itself to the view and keeps itself
 * centred over the bottom edge.
 * @author FuCad contributors
 */
class GuiExport NavigationBar: public QToolBar
{
    Q_OBJECT

public:
    /// Whether 3D views float the bar over the canvas.
    static bool isEnabled();

    /// Floats a bar over \a view, which must be the widget it should follow.
    explicit NavigationBar(QWidget* view);
    ~NavigationBar() override = default;

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void populate();
    void reposition();

    Q_DISABLE_COPY(NavigationBar)
};

}  // namespace Gui
