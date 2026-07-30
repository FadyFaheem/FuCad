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
#include <QStringList>
#include <QToolButton>

#include <FCGlobal.h>

class QAction;

namespace Gui
{
namespace Ribbon
{

enum class ButtonSize
{
    Large,
    Small,
};

/**
 * A single ribbon entry: a large icon with its label underneath, driven by the
 * QAction that the command framework owns for the command.
 *
 * When the entry provides sub-commands, or when the command itself is a group
 * command, the button turns into a split button whose drop-down lists them.
 * @author FuCad contributors
 */
class GuiExport RibbonButton: public QToolButton
{
    Q_OBJECT

public:
    explicit RibbonButton(QWidget* parent = nullptr);
    ~RibbonButton() override = default;

    /**
     * Binds the button to \a command and, when given, to the \a subCommands of
     * its drop-down. A non-empty \a label replaces the menu text the command
     * registered, which lets the ribbon speak Fusion vocabulary while the
     * command keeps the name it is known and translated under everywhere else.
     * Returns false when the command is not registered, in which case the
     * button is left unusable and should be discarded by the caller.
     */
    bool setCommand(
        const QString& command,
        const QString& label,
        const QStringList& subCommands,
        ButtonSize size
    );

    /**
     * Marks the button as the call to action of its page, which the stylesheet
     * picks up to give it the accent fill Fusion gives "Finish Sketch".
     */
    void setPrimary(bool primary);

    /**
     * Resolves a command name to the QAction the command framework owns for it,
     * or nullptr when the command is not registered.
     */
    static QAction* resolveAction(const QString& command);

private:
    void applySize(ButtonSize size);

    Q_DISABLE_COPY(RibbonButton)
};

}  // namespace Ribbon
}  // namespace Gui
