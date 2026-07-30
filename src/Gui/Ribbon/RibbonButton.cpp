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
#include <QChar>
#include <QFontMetrics>
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
constexpr int largeIconExtent = 32;
// Room for the icon plus two rows of label: Qt clips a tool button whose
// maximum height is below the size hint it derives from its multi-row text.
constexpr int largeButtonHeight = 76;
constexpr int largeButtonMinimumWidth = 58;
constexpr int largeButtonMaximumWidth = 90;
constexpr int largeLabelWidth = 80;
constexpr int largeLabelLines = 2;
constexpr int menuIndicatorWidth = 16;
constexpr int smallIconExtent = 16;
constexpr int smallButtonHeight = 24;
constexpr int smallButtonWidth = 26;

/**
 * Folds \a text over at most \a lines rows of at most \a width pixels, the way
 * Fusion stacks a long command name under its icon. Qt lays out the rows itself
 * once they are separated by newlines, but it never breaks a row on its own, so
 * the break has to be picked here.
 */
QString wrapLabel(const QString& text, const QFontMetrics& metrics, int width, int lines)
{
    const QStringList words = text.split(QLatin1Char(' '), Qt::SkipEmptyParts);
    if (words.isEmpty()) {
        return text;
    }

    QStringList rows;
    QString row = words.first();
    for (int i = 1; i < words.size(); ++i) {
        const QString candidate = row + QLatin1Char(' ') + words.at(i);
        if (rows.size() + 1 < lines && metrics.horizontalAdvance(candidate) > width) {
            rows.append(row);
            row = words.at(i);
            continue;
        }

        row = candidate;
    }
    rows.append(row);

    for (QString& line : rows) {
        line = metrics.elidedText(line, Qt::ElideRight, width);
    }

    return rows.join(QLatin1Char('\n'));
}
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
    ButtonSize size
)
{
    if (!Application::Instance) {
        return false;
    }

    CommandManager& manager = Application::Instance->commandManager();
    Command* cmd = manager.getCommandByName(command.toLatin1().constData());
    if (!cmd) {
        Base::Console().warning("Ribbon: skipping unknown command '%s'\n", command.toUtf8().constData());
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
        const QString caption = label.isEmpty() ? Action::commandMenuText(cmd) : label;
        setText(wrapLabel(caption, QFontMetrics(font()), largeLabelWidth, largeLabelLines));
    }

    QList<QAction*> children;
    for (const QString& subCommand : subCommands) {
        if (QAction* action = resolveAction(subCommand)) {
            children.append(action);
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

        // The drop-down arrow is carved out of the button, so the label keeps
        // the width it was wrapped for only if the button grows by that much.
        setMaximumWidth(maximumWidth() + menuIndicatorWidth);
    }

    return true;
}

void RibbonButton::applySize(ButtonSize size)
{
    switch (size) {
        case ButtonSize::Large:
            setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
            setIconSize(QSize(largeIconExtent, largeIconExtent));
            setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
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
