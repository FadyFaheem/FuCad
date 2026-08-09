// SPDX-License-Identifier: LGPL-2.1-or-later

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
 **************************************************************************/

#include <array>
#include <cstring>

#include <QAction>
#include <QCoreApplication>
#include <QList>
#include <QString>

#include "Action.h"
#include "Application.h"
#include "BitmapFactory.h"
#include "Command.h"
#include "MainWindow.h"
#include "Selection.h"


using namespace Gui;

namespace
{
/// The kinds of thing a pick in the 3D view can land on, as a bit per kind.
enum SelectableKind : int
{
    Vertices = 1 << 0,
    Edges = 1 << 1,
    Faces = 1 << 2,
    Solids = 1 << 3,
    EveryKind = Vertices | Edges | Faces | Solids,
};

/// Order of the entries in the drop-down, and of the bits they stand for.
constexpr std::array<SelectableKind, 4> kindOrder = {Vertices, Edges, Faces, Solids};

/**
 * Refuses picks that land on a kind of geometry the user has filtered out, which
 * is how Fusion's selection filters keep a click from finding the wrong thing in
 * a crowded model.
 */
class KindGate: public SelectionGate
{
public:
    explicit KindGate(int allowed)
        : allowed(allowed)
    {}

    bool allow(App::Document*, App::DocumentObject*, const char* sub) override
    {
        // No sub-element means the pick landed on the object as a whole.
        if (!sub || !*sub) {
            return (allowed & Solids) != 0;
        }

        if (std::strncmp(sub, "Vertex", 6) == 0) {
            return (allowed & Vertices) != 0;
        }
        if (std::strncmp(sub, "Edge", 4) == 0) {
            return (allowed & Edges) != 0;
        }
        if (std::strncmp(sub, "Face", 4) == 0) {
            return (allowed & Faces) != 0;
        }

        // Something the filters have no opinion about, such as a sketch axis.
        return true;
    }

private:
    int allowed;
};
}  // namespace


//===========================================================================
// Std_SelectionFilter
//===========================================================================

class StdCmdSelectionFilter: public Gui::Command
{
public:
    StdCmdSelectionFilter();
    ~StdCmdSelectionFilter() override = default;

    void languageChange() override;
    const char* className() const override
    {
        return "StdCmdSelectionFilter";
    }

protected:
    void activated(int iMsg) override;
    bool isActive() override;
    Gui::Action* createAction() override;

private:
    /// Installs or drops the gate so that it matches \a allowed.
    void applyFilter();
    /// Puts the ticks back in step with the gate that is actually installed.
    void syncActions();

    int allowed {EveryKind};
    /// Only ever compared, never dereferenced: the selection owns the gate and
    /// silently deletes it when anything else installs one of its own.
    const SelectionGate* installed {nullptr};
};

StdCmdSelectionFilter::StdCmdSelectionFilter()
    : Command("Std_SelectionFilter")
{
    sGroup = "View";
    sMenuText = QT_TR_NOOP("Selection Filters");
    sToolTipText = QT_TR_NOOP("Restricts what a click in the 3D view can select");
    sWhatsThis = "Std_SelectionFilter";
    sStatusTip = sToolTipText;
    sPixmap = "Std_SelectionFilter";
    eType = NoTransaction | AlterSelection;
}

Gui::Action* StdCmdSelectionFilter::createAction()
{
    auto* group = new Gui::ActionGroup(this, Gui::getMainWindow());
    group->setDropDownMenu(true);
    group->setExclusive(false);
    applyCommandData(this->className(), group);

    const std::array<const char*, 4> icons = {
        "vertex-selection",
        "edge-selection",
        "face-selection",
        "solid-selection",
    };

    for (const char* icon : icons) {
        QAction* entry = group->addAction(QString());
        entry->setCheckable(true);
        entry->setChecked(true);
        entry->setIcon(BitmapFactory().iconFromTheme(icon));
        entry->setWhatsThis(QString::fromLatin1(getWhatsThis()));
    }

    group->setIcon(BitmapFactory().iconFromTheme(getPixmap()));

    _pcAction = group;
    languageChange();
    return group;
}

void StdCmdSelectionFilter::languageChange()
{
    Command::languageChange();

    auto* group = qobject_cast<Gui::ActionGroup*>(_pcAction);
    if (!group) {
        return;
    }

    const QList<QAction*> entries = group->actions();
    if (entries.size() < 4) {
        return;
    }

    entries[0]->setText(QCoreApplication::translate("Std_SelectionFilter", "Vertices"));
    entries[1]->setText(QCoreApplication::translate("Std_SelectionFilter", "Edges"));
    entries[2]->setText(QCoreApplication::translate("Std_SelectionFilter", "Faces"));
    entries[3]->setText(QCoreApplication::translate("Std_SelectionFilter", "Bodies"));

    for (QAction* entry : entries) {
        entry->setToolTip(
            QCoreApplication::translate("Std_SelectionFilter", "Allow picking %1").arg(entry->text())
        );
    }
}

void StdCmdSelectionFilter::activated(int iMsg)
{
    auto* group = qobject_cast<Gui::ActionGroup*>(_pcAction);
    if (!group) {
        return;
    }

    const QList<QAction*> entries = group->actions();
    if (iMsg < 0 || iMsg >= entries.size()) {
        return;
    }

    // Turning the last one off would leave nothing selectable at all, so the
    // click that would do it turns it back on instead.
    const bool wanted = entries[iMsg]->isChecked();
    if (!wanted && (allowed & ~kindOrder.at(iMsg)) == 0) {
        entries[iMsg]->setChecked(true);
        return;
    }

    allowed = wanted ? (allowed | kindOrder.at(iMsg)) : (allowed & ~kindOrder.at(iMsg));
    applyFilter();
}

void StdCmdSelectionFilter::applyFilter()
{
    if (allowed == EveryKind) {
        // Nothing to keep out, so the selection is left without a gate rather
        // than with one that says yes to everything.
        if (installed && Selection().getSelectionGate(nullptr) == installed) {
            Selection().rmvSelectionGate();
        }

        installed = nullptr;
        return;
    }

    auto* gate = new KindGate(allowed);
    Selection().addSelectionGate(gate);
    installed = gate;
}

void StdCmdSelectionFilter::syncActions()
{
    if (!installed || Selection().getSelectionGate(nullptr) == installed) {
        return;
    }

    // Something else needed the gate for itself, which drops the filter. Say so
    // rather than leaving ticks that no longer describe anything.
    installed = nullptr;
    allowed = EveryKind;

    if (auto* group = qobject_cast<Gui::ActionGroup*>(_pcAction)) {
        for (QAction* entry : group->actions()) {
            entry->setChecked(true);
        }
    }
}

bool StdCmdSelectionFilter::isActive()
{
    syncActions();
    return true;
}

namespace Gui
{

void CreateSelectionCommands()
{
    CommandManager& rcCmdMgr = Application::Instance->commandManager();

    rcCmdMgr.addCommand(new StdCmdSelectionFilter());
}

}  // namespace Gui
