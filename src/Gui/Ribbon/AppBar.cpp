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
#include <QHBoxLayout>
#include <QList>
#include <QMenu>
#include <QMenuBar>
#include <QResizeEvent>
#include <QSize>
#include <QSizePolicy>
#include <QStringList>
#include <QStyle>
#include <QToolButton>
#include <QWidget>

#include <Base/Console.h>
#include <Gui/Action.h>
#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Gui/WorkbenchSelector.h>

#include "AppBar.h"
#include "RibbonButton.h"


using namespace Gui;
using namespace Gui::Ribbon;

namespace
{
constexpr int appBarHeight = 30;
constexpr int quickAccessIconExtent = 16;
constexpr int separatorGap = 3;
constexpr int separatorWidth = 1;
constexpr int workspaceSelectorWidth = 170;
}  // namespace


AppBar::AppBar(QWidget* parent)
    : QWidget(parent)
    , barLayout(nullptr)
    , appMenuBar(nullptr)
    , menuButton(nullptr)
    , menuButtonMenu(nullptr)
{
    setObjectName(QStringLiteral("RibbonAppBar"));
    setAttribute(Qt::WA_StyledBackground, true);
    setFixedHeight(appBarHeight);

    barLayout = new QHBoxLayout(this);
    barLayout->setContentsMargins(4, 2, 4, 2);
    barLayout->setSpacing(2);

    createMenuButton();
    createQuickAccess();
    barLayout->addStretch(1);
    createWorkspaceSelector();
}

QMenuBar* AppBar::menuBar() const
{
    return appMenuBar;
}

void AppBar::createMenuButton()
{
    appMenuBar = new QMenuBar(this);
    appMenuBar->setObjectName(QStringLiteral("menuBar"));

    // Qt only dispatches the shortcut of a menu action when the menu bar owning
    // the menu is visible, so the bar has to stay shown even though the ribbon
    // renders the menus through the button below. Clamping it to zero height
    // and keeping it out of the layout makes it paint nothing.
    appMenuBar->setFixedHeight(0);
    appMenuBar->setGeometry(0, 0, width(), 0);
    appMenuBar->lower();
    appMenuBar->show();

    menuButtonMenu = new QMenu(this);

    menuButton = new QToolButton(this);
    menuButton->setObjectName(QStringLiteral("RibbonMenuButton"));
    menuButton->setAutoRaise(true);
    menuButton->setFocusPolicy(Qt::NoFocus);
    menuButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    menuButton->setIcon(style()->standardIcon(QStyle::SP_TitleBarMenuButton));
    menuButton->setIconSize(QSize(quickAccessIconExtent, quickAccessIconExtent));
    menuButton->setText(tr("Menu"));
    menuButton->setToolTip(tr("Show the application menus"));
    menuButton->setPopupMode(QToolButton::InstantPopup);
    menuButton->setMenu(menuButtonMenu);

    connect(menuButtonMenu, &QMenu::aboutToShow, this, &AppBar::refreshMenu);

    barLayout->addWidget(menuButton);
}

void AppBar::createQuickAccess()
{
    const QStringList quickCommands = {
        QStringLiteral("Std_New"),
        QStringLiteral("Std_Open"),
        QStringLiteral("Std_Save"),
        QString(),
        QStringLiteral("Std_Undo"),
        QStringLiteral("Std_Redo"),
    };

    for (const QString& command : quickCommands) {
        if (command.isEmpty()) {
            auto* rule = new QWidget(this);
            rule->setObjectName(QStringLiteral("RibbonAppBarSeparator"));
            rule->setAttribute(Qt::WA_StyledBackground, true);
            rule->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
            rule->setFixedWidth(separatorWidth);
            barLayout->addSpacing(separatorGap);
            barLayout->addWidget(rule);
            barLayout->addSpacing(separatorGap);
            continue;
        }

        QAction* action = RibbonButton::resolveAction(command);
        if (!action) {
            Base::Console().warning(
                "Ribbon: quick access command '%s' is not registered\n",
                command.toUtf8().constData()
            );
            continue;
        }

        auto* button = new QToolButton(this);
        button->setObjectName(QStringLiteral("RibbonQuickAccessButton"));
        button->setAutoRaise(true);
        button->setFocusPolicy(Qt::NoFocus);
        button->setToolButtonStyle(Qt::ToolButtonIconOnly);
        button->setIconSize(QSize(quickAccessIconExtent, quickAccessIconExtent));
        button->setDefaultAction(action);
        barLayout->addWidget(button);
    }
}

void AppBar::createWorkspaceSelector()
{
    if (!Application::Instance) {
        return;
    }

    CommandManager& manager = Application::Instance->commandManager();
    Command* command = manager.getCommandByName("Std_Workbench");
    if (!command) {
        Base::Console().warning(
            "Ribbon: 'Std_Workbench' is not registered, the workspace selector is unavailable\n"
        );
        return;
    }

    command->initAction();
    auto* group = qobject_cast<WorkbenchGroup*>(command->getAction());
    if (!group) {
        Base::Console().warning("Ribbon: 'Std_Workbench' provides no workbench group\n");
        return;
    }

    auto* selector = new WorkbenchComboBox(group, this);
    selector->setObjectName(QStringLiteral("RibbonWorkspaceSelector"));
    selector->setMinimumWidth(workspaceSelectorWidth);
    barLayout->addWidget(selector);
}

void AppBar::refreshMenu()
{
    // MenuManager rebuilds the menu bar from scratch on every workbench switch,
    // so the popup is mirrored from the live bar each time it is opened. The
    // actions stay owned by their menus, hence QMenu::clear() does not free them.
    menuButtonMenu->clear();

    const QList<QAction*> actions = appMenuBar ? appMenuBar->actions() : QList<QAction*> {};
    for (QAction* action : actions) {
        menuButtonMenu->addAction(action);
    }

    if (menuButtonMenu->isEmpty()) {
        QAction* placeholder = menuButtonMenu->addAction(tr("No menu available"));
        placeholder->setEnabled(false);
    }
}

void AppBar::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    // A resize can reach this before the constructor has built the menu bar.
    if (appMenuBar) {
        appMenuBar->setGeometry(0, 0, width(), 0);
    }
}

#include "moc_AppBar.cpp"
