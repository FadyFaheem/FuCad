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

#include <QString>
#include <QWidget>

#include <FCGlobal.h>

class QHBoxLayout;

namespace Gui
{
namespace Ribbon
{

/**
 * A captioned group of ribbon buttons: a horizontal row of buttons with the
 * caption centred underneath and a vertical rule along the right edge that
 * separates the panel from its neighbour.
 * @author FuCad contributors
 */
class GuiExport RibbonPanel: public QWidget
{
    Q_OBJECT

public:
    explicit RibbonPanel(const QString& caption, QWidget* parent = nullptr);
    ~RibbonPanel() override = default;

    void addButton(QWidget* button);
    bool isEmpty() const;

    /// The right-hand rule is dropped on the last panel of a page.
    void setSeparatorVisible(bool visible);

private:
    QHBoxLayout* buttonLayout;
    QWidget* separator;
    int buttonCount {0};

    Q_DISABLE_COPY(RibbonPanel)
};

}  // namespace Ribbon
}  // namespace Gui
