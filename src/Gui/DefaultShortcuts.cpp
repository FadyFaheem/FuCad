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


#include <string>
#include <string_view>
#include <unordered_map>

#include "DefaultShortcuts.h"
#include "ShortcutManager.h"


using namespace Gui;

namespace
{
/**
 * Fusion reaches its tools with a bare letter, where FreeCAD reserves those for
 * the sketch constraints and gives the tools a G or K prefix. The letters go to
 * the tools here, and the constraints that held them move into the K prefix
 * their siblings already use.
 *
 * Modelling and sketching can share a letter because only one of the two is ever
 * enabled: a sketch constraint needs a sketch open, and a modelling command needs
 * one closed. The shortcut framework hands the key to whichever is live.
 */
const std::unordered_map<std::string_view, const char*> shortcuts = {
    // Modelling, live while no sketch is open.
    {"PartDesign_PressPull", "Q"},
    {"PartDesign_Extrude", "E"},
    {"PartDesign_Fillet", "F"},
    {"PartDesign_Hole", "H"},
    {"Std_Placement", "M"},
    {"Std_Measure", "I"},
    {"Std_SetAppearance", "A"},

    // Sketching, live only inside a sketch.
    {"Sketcher_CreateLine", "L"},
    {"Sketcher_CompCreateRectangles", "R"},
    {"Sketcher_CreateCircle", "C"},
    {"Sketcher_Dimension", "D"},
    {"Sketcher_Trimming", "T"},
    {"Sketcher_Offset", "O"},
    {"Sketcher_CompExternal", "P"},
    {"Sketcher_ToggleConstruction", "X"},

    // The constraints the sketch letters displaced. Each keeps a letter that
    // says what it does, now behind the K that the other dimensions use.
    {"Sketcher_ConstrainDistanceX", "K, H"},
    {"Sketcher_CompConstrainRadDia", "K, C"},
    {"Sketcher_ConstrainTangent", "K, T"},
    {"Sketcher_ConstrainPointOnObject", "K, N"},
    {"Sketcher_ConstrainParallel", "K, P"},
};

/// High enough to beat the commands that never asked for an order of their own,
/// low enough to leave room for a user who wants something else on top.
constexpr int fuCadPriority = 100;
}  // namespace


const char* Gui::defaultShortcutFor(const char* command)
{
    if (!command || !*command) {
        return nullptr;
    }

    const auto found = shortcuts.find(std::string_view(command));
    return found == shortcuts.end() ? nullptr : found->second;
}

void Gui::applyDefaultShortcutPriorities()
{
    ShortcutManager* manager = ShortcutManager::instance();
    for (const auto& [command, accel] : shortcuts) {
        const std::string name(command);
        if (manager->getPriority(name.c_str()) == 0) {
            manager->setPriority(name.c_str(), fuCadPriority);
        }
    }
}
