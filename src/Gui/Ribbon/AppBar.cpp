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
#include <QEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QList>
#include <QMenu>
#include <QMenuBar>
#include <QMouseEvent>
#include <QPoint>
#include <QResizeEvent>
#include <QSize>
#include <QSizePolicy>
#include <QStringList>
#include <QStyle>
#include <QToolButton>
#include <QWidget>
#include <QWindow>

#include <Base/Console.h>
#include <Gui/Action.h>
#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Gui/MainWindow.h>
#include <Gui/WorkbenchSelector.h>

#include "AppBar.h"
#include "FramelessWindow.h"
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
constexpr int windowButtonWidth = 40;
}  // namespace


AppBar::AppBar(QWidget* parent)
    : QWidget(parent)
    , barLayout(nullptr)
    , appMenuBar(nullptr)
    , menuButton(nullptr)
    , menuButtonMenu(nullptr)
    , titleLabel(nullptr)
    , maximizeButton(nullptr)
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
    createTitle();
    barLayout->addStretch(1);
    createWorkspaceSelector();
    createWindowControls();
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

void AppBar::createTitle()
{
    if (!FramelessWindow::isEnabled()) {
        return;
    }

    titleLabel = new QLabel(this);
    titleLabel->setObjectName(QStringLiteral("RibbonWindowTitle"));
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    barLayout->addWidget(titleLabel);

    if (MainWindow* window = getMainWindow()) {
        window->installEventFilter(this);
    }

    refreshTitle();
}

void AppBar::createWindowControls()
{
    if (!FramelessWindow::isEnabled()) {
        return;
    }

    MainWindow* window = getMainWindow();
    if (!window) {
        return;
    }

    struct Control
    {
        const char* name;
        QStyle::StandardPixmap icon;
        QString tip;
    };

    const Control controls[] = {
        {"RibbonMinimizeButton", QStyle::SP_TitleBarMinButton, tr("Minimize")},
        {"RibbonMaximizeButton", QStyle::SP_TitleBarMaxButton, tr("Maximize")},
        {"RibbonCloseButton", QStyle::SP_TitleBarCloseButton, tr("Close")},
    };

    for (const Control& control : controls) {
        auto* button = new QToolButton(this);
        button->setObjectName(QString::fromLatin1(control.name));
        button->setAutoRaise(true);
        button->setFocusPolicy(Qt::NoFocus);
        button->setIcon(style()->standardIcon(control.icon));
        button->setIconSize(QSize(quickAccessIconExtent, quickAccessIconExtent));
        button->setToolTip(control.tip);
        button->setFixedWidth(windowButtonWidth);
        barLayout->addWidget(button);

        if (button->objectName() == QLatin1String("RibbonMinimizeButton")) {
            connect(button, &QToolButton::clicked, window, &QWidget::showMinimized);
        }
        else if (button->objectName() == QLatin1String("RibbonCloseButton")) {
            connect(button, &QToolButton::clicked, window, &QWidget::close);
        }
        else {
            maximizeButton = button;
            connect(button, &QToolButton::clicked, this, [window]() {
                if (window->isMaximized()) {
                    window->showNormal();
                }
                else {
                    window->showMaximized();
                }
            });
        }
    }

    refreshWindowState();
}

void AppBar::refreshTitle()
{
    MainWindow* window = getMainWindow();
    if (!titleLabel || !window) {
        return;
    }

    titleLabel->setText(window->windowTitle());
}

void AppBar::refreshWindowState()
{
    MainWindow* window = getMainWindow();
    if (!maximizeButton || !window) {
        return;
    }

    const bool maximized = window->isMaximized();
    maximizeButton->setIcon(style()->standardIcon(
        maximized ? QStyle::SP_TitleBarNormalButton : QStyle::SP_TitleBarMaxButton
    ));
    maximizeButton->setToolTip(maximized ? tr("Restore") : tr("Maximize"));
}

bool AppBar::isBackgroundAt(const QPoint& pos) const
{
    // childAt() reports the deepest child, so anything it names is a control the
    // press belongs to rather than a place to grab the window by.
    const QWidget* child = childAt(pos);
    return !child || child == titleLabel;
}

void AppBar::mousePressEvent(QMouseEvent* event)
{
    MainWindow* window = getMainWindow();
    if (event->button() != Qt::LeftButton || !FramelessWindow::isEnabled() || !window) {
        QWidget::mousePressEvent(event);
        return;
    }

    if (!isBackgroundAt(event->position().toPoint())) {
        QWidget::mousePressEvent(event);
        return;
    }

    // Handed to the window system, which is what makes dragging to an edge snap.
    if (QWindow* handle = window->windowHandle()) {
        handle->startSystemMove();
        event->accept();
        return;
    }

    QWidget::mousePressEvent(event);
}

void AppBar::mouseDoubleClickEvent(QMouseEvent* event)
{
    MainWindow* window = getMainWindow();
    if (event->button() != Qt::LeftButton || !FramelessWindow::isEnabled() || !window
        || !isBackgroundAt(event->position().toPoint())) {
        QWidget::mouseDoubleClickEvent(event);
        return;
    }

    if (window->isMaximized()) {
        window->showNormal();
    }
    else {
        window->showMaximized();
    }

    event->accept();
}

bool AppBar::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == getMainWindow()) {
        if (event->type() == QEvent::WindowTitleChange) {
            refreshTitle();
        }
        else if (event->type() == QEvent::WindowStateChange) {
            refreshWindowState();
        }
    }

    return QWidget::eventFilter(watched, event);
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
