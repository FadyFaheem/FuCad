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

#include <FCGlobal.h>

namespace Gui
{

/**
 * The accelerator FuCad gives \a command instead of the one it registered with,
 * or nullptr when it keeps its own.
 *
 * Kept in one table rather than spread over the command sources so that the
 * whole keyboard can be read at once, and so that the sources stay as upstream
 * wrote them. It only moves the default: a shortcut the user has set for
 * themselves still wins, as it does for any command.
 */
GuiExport const char* defaultShortcutFor(const char* command);

/**
 * Puts the commands of the table ahead of anything else that answers to the same
 * key, which matters inside an assembly, where its joint commands hold most of
 * the alphabet and are live at the same time as the modelling commands.
 *
 * Only fills in a priority that has never been set, so a user who has ordered the
 * conflicting commands themselves keeps their order.
 */
GuiExport void applyDefaultShortcutPriorities();

}  // namespace Gui
