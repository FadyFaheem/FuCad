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


#include <array>

#include <QEvent>
#include <QSize>
#include <QString>
#include <QToolButton>

#include <App/Application.h>
#include <Base/Console.h>
#include <Base/Parameter.h>

#include "Application.h"
#include "Command.h"
#include "NavigationBar.h"


using namespace Gui;

namespace
{
constexpr int iconExtent = 18;
/// Distance kept between the bar and the bottom edge of the view.
constexpr int bottomInset = 14;

const char* const viewParameters = "User parameter:BaseApp/Preferences/View";

/// A "" entry is a separator. Ordered the way Fusion groups them: getting to the
/// model, then looking at it from somewhere, then how it is drawn.
constexpr std::array<const char*, 11> barCommands = {
    "Std_ViewFitAll",
    "Std_ViewFitSelection",
    "Std_AlignToSelection",
    "",
    "Std_ViewGroup",
    "Std_ViewHome",
    "",
    "Std_DrawStyle",
    "Std_OrthographicCamera",
    "Std_PerspectiveCamera",
    "Std_AxisCross",
};
}  // namespace


bool NavigationBar::isEnabled()
{
    return App::GetApplication()
        .GetParameterGroupByPath(viewParameters)
        ->GetBool("UseNavigationBar", true);
}

NavigationBar::NavigationBar(QWidget* view)
    : QToolBar(view)
{
    setObjectName(QStringLiteral("NavigationBar"));
    setAttribute(Qt::WA_StyledBackground, true);
    setMovable(false);
    setFloatable(false);
    setFocusPolicy(Qt::NoFocus);
    setIconSize(QSize(iconExtent, iconExtent));
    setToolButtonStyle(Qt::ToolButtonIconOnly);

    populate();

    view->installEventFilter(this);
    reposition();
    raise();
    show();
}

void NavigationBar::populate()
{
    if (!Application::Instance) {
        return;
    }

    CommandManager& manager = Application::Instance->commandManager();
    for (const char* name : barCommands) {
        if (!*name) {
            addSeparator();
            continue;
        }

        Command* command = manager.getCommandByName(name);
        if (!command) {
            Base::Console().warning("NavigationBar: '%s' is not registered\n", name);
            continue;
        }

        // Through the command rather than through its action, so that a group
        // command brings the drop-down the framework builds for a toolbar.
        command->addTo(this);
    }

    for (QToolButton* button : findChildren<QToolButton*>()) {
        button->setFocusPolicy(Qt::NoFocus);
    }
}

void NavigationBar::reposition()
{
    QWidget* view = parentWidget();
    if (!view) {
        return;
    }

    // sizeHint rather than the current size: the bar has no layout owner to give
    // it one, so it has to take the width its buttons ask for.
    const QSize wanted = sizeHint();
    resize(wanted);
    move((view->width() - wanted.width()) / 2, view->height() - wanted.height() - bottomInset);
}

bool NavigationBar::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == parentWidget()
        && (event->type() == QEvent::Resize || event->type() == QEvent::Show)) {
        reposition();
        raise();
    }

    return QToolBar::eventFilter(watched, event);
}

#include "moc_NavigationBar.cpp"
