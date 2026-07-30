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

#include <QWidget>

#include <FCGlobal.h>

class QHBoxLayout;
class QMenu;
class QMenuBar;
class QResizeEvent;
class QToolButton;

namespace Gui
{
namespace Ribbon
{

/**
 * The strip above the ribbon tabs: the application menu button, the
 * quick-access buttons and the workspace selector.
 *
 * The app bar also hosts the application's one and only QMenuBar. The bar is
 * never installed into the QMainWindow menu slot, because the ribbon container
 * occupies it; MainWindow::menuBar() hands out the bar living here so that
 * MenuManager keeps driving the menus unchanged.
 * @author FuCad contributors
 */
class GuiExport AppBar: public QWidget
{
    Q_OBJECT

public:
    explicit AppBar(QWidget* parent = nullptr);
    ~AppBar() override = default;

    QMenuBar* menuBar() const;

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void createMenuButton();
    void createQuickAccess();
    void createWorkspaceSelector();
    void refreshMenu();

    QHBoxLayout* barLayout;
    QMenuBar* appMenuBar;
    QToolButton* menuButton;
    QMenu* menuButtonMenu;

    Q_DISABLE_COPY(AppBar)
};

}  // namespace Ribbon
}  // namespace Gui
