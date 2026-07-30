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


#include <QAction>
#include <QList>
#include <QMenu>
#include <QSize>
#include <QSizePolicy>
#include <QStringList>

#include <Base/Console.h>
#include <Base/Exception.h>
#include <Gui/Action.h>
#include <Gui/Application.h>
#include <Gui/Command.h>

#include "RibbonButton.h"


using namespace Gui;
using namespace Gui::Ribbon;

namespace
{
constexpr int largeIconExtent = 24;
// Fusion draws the command icons alone and names them only on hover, so the
// button is a square around the icon rather than a column with a label.
constexpr int largeButtonHeight = 34;
constexpr int largeButtonMinimumWidth = 34;
constexpr int largeButtonMaximumWidth = 34;
constexpr int menuIndicatorWidth = 12;
constexpr int smallIconExtent = 16;
constexpr int smallButtonHeight = 24;
constexpr int smallButtonWidth = 26;

}  // namespace


RibbonButton::RibbonButton(QWidget* parent)
    : QToolButton(parent)
{
    setObjectName(QStringLiteral("RibbonButton"));
    setAutoRaise(true);
    setFocusPolicy(Qt::NoFocus);
    setPopupMode(QToolButton::DelayedPopup);
}

void RibbonButton::setPrimary(bool primary)
{
    setObjectName(
        primary ? QStringLiteral("RibbonPrimaryButton") : QStringLiteral("RibbonButton")
    );
}

QAction* RibbonButton::resolveAction(const QString& command)
{
    if (command.isEmpty() || !Application::Instance) {
        return nullptr;
    }

    CommandManager& manager = Application::Instance->commandManager();
    Command* cmd = manager.getCommandByName(command.toLatin1().constData());
    if (!cmd) {
        return nullptr;
    }

    cmd->initAction();
    Gui::Action* action = cmd->getAction();
    return action ? action->action() : nullptr;
}

bool RibbonButton::setCommand(
    const QString& command,
    const QString& label,
    const QStringList& subCommands,
    ButtonSize size,
    bool quiet
)
{
    if (!Application::Instance) {
        return false;
    }

    CommandManager& manager = Application::Instance->commandManager();
    Command* cmd = manager.getCommandByName(command.toLatin1().constData());
    if (!cmd) {
        if (quiet) {
            Base::Console().log(
                "Ribbon: '%s' is not available, its module is not loaded\n",
                command.toUtf8().constData()
            );
        }
        else {
            Base::Console().warning(
                "Ribbon: skipping unknown command '%s'\n",
                command.toUtf8().constData()
            );
        }
        return false;
    }

    cmd->initAction();
    Gui::Action* guiAction = cmd->getAction();
    if (!guiAction || !guiAction->action()) {
        Base::Console().warning(
            "Ribbon: command '%s' provides no action\n",
            command.toUtf8().constData()
        );
        return false;
    }

    setDefaultAction(guiAction->action());
    applySize(size);

    if (size == ButtonSize::Large) {
        // The name is not drawn, so it has to lead the tooltip: hovering is the only
        // way to find out what an icon does.
        const QString caption = label.isEmpty() ? Action::commandMenuText(cmd) : label;
        setText(caption);

        const QString description = guiAction->action()->toolTip();
        if (description.isEmpty()) {
            setToolTip(caption);
        }
        else if (description.contains(caption)) {
            // FreeCAD's tooltip already opens with the command's own name.
            setToolTip(description);
        }
        else {
            // Only differs when the ribbon renames the command, and the separator
            // has to match the format Qt infers for the rest of the tooltip.
            const bool rich = description.trimmed().startsWith(QLatin1Char('<'));
            setToolTip(
                caption + (rich ? QLatin1String("<br/>") : QLatin1String("\n")) + description
            );
        }
    }

    QList<QAction*> children;
    for (const QString& subCommand : subCommands) {
        if (QAction* action = resolveAction(subCommand)) {
            children.append(action);
        }
        else if (quiet) {
            Base::Console().log(
                "Ribbon: sub-command '%s' of '%s' is not available\n",
                subCommand.toUtf8().constData(),
                command.toUtf8().constData()
            );
        }
        else {
            Base::Console().warning(
                "Ribbon: skipping unknown sub-command '%s' of '%s'\n",
                subCommand.toUtf8().constData(),
                command.toUtf8().constData()
            );
        }
    }

    if (children.isEmpty()) {
        if (auto* group = qobject_cast<Gui::ActionGroup*>(guiAction)) {
            children = group->actions();
        }
    }

    if (!children.isEmpty()) {
        auto* menu = new QMenu(this);
        menu->addActions(children);
        setMenu(menu);
        setPopupMode(QToolButton::MenuButtonPopup);

        // The drop-down arrow is carved out of the button, so the icon keeps its
        // size only if the button grows by that much.
        setMinimumWidth(minimumWidth() + menuIndicatorWidth);
        setMaximumWidth(maximumWidth() + menuIndicatorWidth);
    }

    return true;
}

void RibbonButton::applySize(ButtonSize size)
{
    switch (size) {
        case ButtonSize::Large:
            setToolButtonStyle(Qt::ToolButtonIconOnly);
            setIconSize(QSize(largeIconExtent, largeIconExtent));
            setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            setMinimumSize(QSize(largeButtonMinimumWidth, largeButtonHeight));
            setMaximumSize(QSize(largeButtonMaximumWidth, largeButtonHeight));
            break;
        case ButtonSize::Small:
            setToolButtonStyle(Qt::ToolButtonIconOnly);
            setIconSize(QSize(smallIconExtent, smallIconExtent));
            setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            setMinimumSize(QSize(smallButtonWidth, smallButtonHeight));
            setMaximumSize(QSize(smallButtonWidth, smallButtonHeight));
            break;
        default:
            throw Base::RuntimeError("RibbonButton: unhandled button size");
    }
}

#include "moc_RibbonButton.cpp"
