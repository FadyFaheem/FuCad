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


#include <cstddef>
#include <utility>

#include <QAction>
#include <QByteArray>
#include <QCoreApplication>
#include <QFile>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QList>
#include <QMenu>
#include <QWidget>

#include <App/Application.h>
#include <Base/Console.h>
#include <Base/Interpreter.h>
#include <Base/Parameter.h>
#include <Base/Tools.h>
#include <Gui/Action.h>
#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Gui/MainWindow.h>
#include <Gui/ToolBarManager.h>
#include <Gui/WorkbenchManager.h>

#include "RibbonBar.h"
#include "RibbonButton.h"
#include "RibbonManager.h"
#include "RibbonPanel.h"


using namespace Gui;
using namespace Gui::Ribbon;

namespace
{
const char* const workspaceResource = ":/ribbon/Workspaces/Design.json";
const char* const mainWindowPreferences = "User parameter:BaseApp/Preferences/MainWindow";

/// The command framework action of \a command, or nullptr when not registered.
Gui::Action* resolveGuiAction(const QString& command)
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
    return cmd->getAction();
}

void reportMissing(const QString& command, bool quiet)
{
    if (quiet) {
        Base::Console().log(
            "Ribbon: '%s' is not available, its module is not loaded\n",
            command.toUtf8().constData()
        );
        return;
    }

    Base::Console().warning(
        "Ribbon: skipping unknown command '%s'\n",
        command.toUtf8().constData()
    );
}

/**
 * A menu entry that carries \a label but triggers \a source, so that the ribbon
 * can name a command the way Fusion does without renaming the action the rest
 * of the application shares. The proxy follows the state of the original, which
 * the command framework keeps up to date.
 */
QAction* createLabelledAction(QAction* source, const QString& label, QMenu* parent)
{
    auto* proxy = new QAction(source->icon(), label, parent);
    proxy->setToolTip(source->toolTip());
    proxy->setStatusTip(source->statusTip());
    proxy->setWhatsThis(source->whatsThis());
    proxy->setCheckable(source->isCheckable());
    proxy->setChecked(source->isChecked());
    proxy->setEnabled(source->isEnabled());

    QObject::connect(proxy, &QAction::triggered, source, [source]() {
        source->trigger();
    });
    QObject::connect(source, &QAction::changed, proxy, [proxy, source]() {
        proxy->setIcon(source->icon());
        proxy->setEnabled(source->isEnabled());
        proxy->setChecked(source->isChecked());
    });

    return proxy;
}
}  // namespace


RibbonManager* RibbonManager::_instance = nullptr;

RibbonManager::RibbonManager() = default;

RibbonManager::~RibbonManager() = default;

RibbonManager* RibbonManager::instance()
{
    if (!_instance) {
        _instance = new RibbonManager();
    }

    return _instance;
}

void RibbonManager::destruct()
{
    delete _instance;
    _instance = nullptr;
}

bool RibbonManager::isEnabled()
{
    // Sampled once: the shell is installed while the main window is built, so a
    // later change cannot take effect and the toolbars must keep matching the
    // shell that was actually installed.
    static const bool enabled = App::GetApplication()
                                    .GetParameterGroupByPath(mainWindowPreferences)
                                    ->GetBool("UseRibbon", true);
    return enabled;
}

void RibbonManager::attach(RibbonBar* bar)
{
    ribbonBar = bar;
    if (!bar) {
        return;
    }

    connect(bar, &RibbonBar::tabActivated, this, &RibbonManager::onTabActivated);

    if (MainWindow* window = getMainWindow()) {
        connect(window, &MainWindow::workbenchActivated, this, &RibbonManager::onWorkbenchActivated);
    }

    const std::string active = WorkbenchManager::instance()->activeName();
    if (!active.empty()) {
        rebuildTabs(QString::fromStdString(active));
    }
}

void RibbonManager::rememberToolBars(const QString& workbench, const ToolBarItem* toolBars)
{
    if (workbench.isEmpty() || !toolBars) {
        return;
    }

    std::vector<PanelDefinition> panels;

    const QList<ToolBarItem*> bars = toolBars->getItems();
    for (const ToolBarItem* bar : bars) {
        if (!bar) {
            continue;
        }

        PanelDefinition panel;
        panel.caption = QCoreApplication::translate("Workbench", bar->command().c_str());

        const QList<ToolBarItem*> entries = bar->getItems();
        for (const ToolBarItem* entry : entries) {
            if (!entry) {
                continue;
            }

            const QString command = QString::fromStdString(entry->command());
            if (command.isEmpty() || command == QLatin1String("Separator")) {
                continue;
            }

            ItemDefinition item;
            item.command = command;
            panel.items.push_back(std::move(item));
        }

        if (!panel.items.empty()) {
            panels.push_back(std::move(panel));
        }
    }

    rememberedToolBars[workbench] = std::move(panels);
}

void RibbonManager::pushContextTab(const QString& id)
{
    if (ribbonBar.isNull()) {
        return;
    }

    loadWorkspace();

    const TabDefinition* definition = findContextTab(id);
    if (!definition) {
        Base::Console().warning(
            "Ribbon: the workspace definition describes no context tab '%s'\n",
            id.toUtf8().constData()
        );
        return;
    }

    for (const ContextTabState& state : activeContextTabs) {
        if (state.tab == definition) {
            const int shown = indexOfTab(definition);
            if (shown >= 0) {
                Base::StateLocker lock(updating);
                ribbonBar->setCurrentIndex(shown);
            }

            buildPage(shown);
            return;
        }
    }

    ContextTabState state;
    state.tab = definition;

    const int current = ribbonBar->currentIndex();
    if (current >= 0 && current < static_cast<int>(visibleTabs.size())) {
        state.restore = visibleTabs[current]->id;
    }

    activeContextTabs.push_back(state);

    int index = -1;
    {
        Base::StateLocker lock(updating);

        visibleTabs.push_back(definition);
        pageBuilt.push_back(false);

        index = ribbonBar->addTab(definition->id);
        ribbonBar->setContextTabPresent(true);
        ribbonBar->setCurrentIndex(index);
    }

    buildPage(index);
}

void RibbonManager::popContextTab(const QString& id)
{
    if (ribbonBar.isNull()) {
        return;
    }

    auto state = activeContextTabs.end();
    for (auto it = activeContextTabs.begin(); it != activeContextTabs.end(); ++it) {
        if (it->tab->id == id) {
            state = it;
            break;
        }
    }

    if (state == activeContextTabs.end()) {
        return;
    }

    const TabDefinition* definition = state->tab;
    const QString restore = state->restore;
    activeContextTabs.erase(state);

    int selected = -1;
    {
        Base::StateLocker lock(updating);

        const int index = indexOfTab(definition);
        if (index >= 0) {
            ribbonBar->removeTab(index);
            visibleTabs.erase(visibleTabs.begin() + index);
            if (index < static_cast<int>(pageBuilt.size())) {
                pageBuilt.erase(pageBuilt.begin() + index);
            }
        }

        ribbonBar->setContextTabPresent(!activeContextTabs.empty());

        selected = indexOfTab(restore);
        if (selected < 0) {
            selected = ribbonBar->currentIndex();
        }

        ribbonBar->setCurrentIndex(selected);
    }

    buildPage(selected);
}

const RibbonManager::TabDefinition* RibbonManager::findContextTab(const QString& id) const
{
    if (id.isEmpty()) {
        return nullptr;
    }

    for (const TabDefinition& tab : contextTabs) {
        if (tab.id == id) {
            return &tab;
        }
    }

    return nullptr;
}

int RibbonManager::indexOfTab(const TabDefinition* tab) const
{
    for (std::size_t i = 0; i < visibleTabs.size(); ++i) {
        if (visibleTabs[i] == tab) {
            return static_cast<int>(i);
        }
    }

    return -1;
}

int RibbonManager::indexOfTab(const QString& id) const
{
    if (id.isEmpty()) {
        return -1;
    }

    for (std::size_t i = 0; i < visibleTabs.size(); ++i) {
        if (visibleTabs[i]->id == id) {
            return static_cast<int>(i);
        }
    }

    return -1;
}

void RibbonManager::onWorkbenchActivated(const QString& workbench)
{
    if (updating || ribbonBar.isNull()) {
        return;
    }

    rebuildTabs(workbench);
}

void RibbonManager::onTabActivated(int index)
{
    if (updating || ribbonBar.isNull()) {
        return;
    }

    if (index < 0 || index >= static_cast<int>(visibleTabs.size())) {
        return;
    }

    const TabDefinition* tab = visibleTabs[index];
    if (!tab->context && !tab->workbench.isEmpty()
        && tab->workbench.toStdString() != WorkbenchManager::instance()->activeName()) {
        // The workbench has to come up first: it is what registers the commands
        // the page is about to resolve.
        Base::StateLocker lock(updating);
        Application::Instance->activateWorkbench(tab->workbench.toLatin1().constData());
    }

    buildPage(index);
}

void RibbonManager::loadWorkspace()
{
    if (workspaceLoaded) {
        return;
    }

    workspaceLoaded = true;

    QFile file(QString::fromLatin1(workspaceResource));
    if (!file.open(QIODevice::ReadOnly)) {
        Base::Console().error("Ribbon: cannot open the workspace definition '%s'\n", workspaceResource);
        return;
    }

    QJsonParseError error {};
    const QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &error);
    file.close();

    if (error.error != QJsonParseError::NoError) {
        Base::Console().error(
            "Ribbon: the workspace definition is not valid JSON: %s\n",
            error.errorString().toUtf8().constData()
        );
        return;
    }

    if (!document.isObject()) {
        Base::Console().error("Ribbon: the workspace definition is not a JSON object\n");
        return;
    }

    const QJsonObject root = document.object();

    const QJsonArray tabs = root.value(QLatin1String("tabs")).toArray();
    for (int i = 0; i < tabs.size(); ++i) {
        const QJsonValue value = tabs.at(i);
        if (!value.isObject()) {
            continue;
        }

        TabDefinition tab;
        if (parseTab(value.toObject(), tab)) {
            workspaceTabs.push_back(std::move(tab));
        }
    }

    const QJsonArray contexts = root.value(QLatin1String("contextTabs")).toArray();
    for (int i = 0; i < contexts.size(); ++i) {
        const QJsonValue value = contexts.at(i);
        if (!value.isObject()) {
            continue;
        }

        TabDefinition tab;
        if (parseTab(value.toObject(), tab)) {
            tab.context = true;
            contextTabs.push_back(std::move(tab));
        }
    }
}

bool RibbonManager::parseTab(const QJsonObject& source, TabDefinition& tab)
{
    tab.id = source.value(QLatin1String("id")).toString();
    if (tab.id.isEmpty()) {
        Base::Console().warning("Ribbon: skipping a tab without an id\n");
        return false;
    }

    tab.workbench = source.value(QLatin1String("workbench")).toString();
    tab.optional = source.value(QLatin1String("optional")).toBool(false);

    const QJsonArray panels = source.value(QLatin1String("panels")).toArray();
    for (int i = 0; i < panels.size(); ++i) {
        const QJsonValue value = panels.at(i);
        if (!value.isObject()) {
            continue;
        }

        PanelDefinition panel;
        if (parsePanel(value.toObject(), panel)) {
            tab.panels.push_back(std::move(panel));
        }
    }

    return true;
}

bool RibbonManager::parsePanel(const QJsonObject& source, PanelDefinition& panel)
{
    panel.caption = source.value(QLatin1String("caption")).toString();
    panel.alignRight = source.value(QLatin1String("align")).toString() == QLatin1String("right");
    panel.initWorkbench = source.value(QLatin1String("initWorkbench")).toString();

    if (!panel.initWorkbench.isEmpty()) {
        Application::Instance->ensureWorkbenchInitialized(
            panel.initWorkbench.toUtf8().constData()
        );
    }

    const QJsonArray items = source.value(QLatin1String("items")).toArray();
    for (int i = 0; i < items.size(); ++i) {
        const QJsonValue value = items.at(i);
        if (!value.isObject()) {
            continue;
        }

        ItemDefinition item;
        if (parseItem(value.toObject(), item) && !item.command.isEmpty()) {
            panel.items.push_back(std::move(item));
        }
    }

    const QJsonArray menuItems = source.value(QLatin1String("menu")).toArray();
    for (int i = 0; i < menuItems.size(); ++i) {
        const QJsonValue value = menuItems.at(i);
        if (!value.isObject()) {
            continue;
        }

        ItemDefinition item;
        if (parseItem(value.toObject(), item)) {
            panel.menuItems.push_back(std::move(item));
        }
    }

    return !panel.items.empty() || !panel.menuItems.empty();
}

bool RibbonManager::parseItem(const QJsonObject& source, ItemDefinition& item)
{
    item.command = source.value(QLatin1String("command")).toString();
    item.label = source.value(QLatin1String("label")).toString();
    item.primary = source.value(QLatin1String("primary")).toBool(false);
    item.optional = source.value(QLatin1String("optional")).toBool(false);

    const QJsonArray subCommands = source.value(QLatin1String("commands")).toArray();
    for (int i = 0; i < subCommands.size(); ++i) {
        const QString subCommand = subCommands.at(i).toString();
        if (!subCommand.isEmpty()) {
            item.subCommands.append(subCommand);
        }
    }

    // A menu entry may be a bare submenu, which needs a name and something to
    // put under it but no command of its own.
    return !item.command.isEmpty() || (!item.label.isEmpty() && !item.subCommands.isEmpty());
}

void RibbonManager::rebuildTabs(const QString& workbench)
{
    if (ribbonBar.isNull()) {
        return;
    }

    loadWorkspace();

    QStringList available;
    {
        Base::PyGILStateLocker lock;
        available = Application::Instance->workbenches();
    }

    int selected = -1;
    {
        Base::StateLocker lock(updating);

        ribbonBar->clear();
        visibleTabs.clear();
        pageBuilt.clear();

        for (const TabDefinition& tab : workspaceTabs) {
            const bool missing = !tab.workbench.isEmpty() && !available.contains(tab.workbench);
            if (tab.optional && missing) {
                Base::Console().log(
                    "Ribbon: dropping optional tab '%s', workbench '%s' is not available\n",
                    tab.id.toUtf8().constData(),
                    tab.workbench.toUtf8().constData()
                );
                continue;
            }

            visibleTabs.push_back(&tab);
        }

        bool described = false;
        for (const TabDefinition* tab : visibleTabs) {
            if (tab->workbench == workbench) {
                described = true;
                break;
            }
        }

        // A workbench that a context tab speaks for is described even while that tab is
        // not pushed yet. Sketcher activates before enterEditMode() pushes SKETCH, so
        // testing only the pushed tabs would generate a redundant "Sketcher" tab that
        // then sits next to the contextual one.
        for (const TabDefinition& tab : contextTabs) {
            if (!tab.workbench.isEmpty() && tab.workbench == workbench) {
                described = true;
                break;
            }
        }

        if (!described && !workbench.isEmpty()) {
            generatedTab.id = Application::Instance->workbenchMenuText(workbench);
            if (generatedTab.id.isEmpty()) {
                generatedTab.id = workbench;
            }
            generatedTab.workbench = workbench;
            generatedTab.optional = false;

            const auto remembered = rememberedToolBars.find(workbench);
            generatedTab.panels = remembered != rememberedToolBars.end()
                ? remembered->second
                : std::vector<PanelDefinition> {};

            visibleTabs.push_back(&generatedTab);
        }

        for (std::size_t i = 0; i < visibleTabs.size(); ++i) {
            if (selected < 0 && visibleTabs[i]->workbench == workbench) {
                selected = static_cast<int>(i);
            }
        }

        // Context tabs trail the strip, and a mode that is still running keeps
        // the tab it pushed rather than losing it to the workbench switch.
        for (const ContextTabState& state : activeContextTabs) {
            visibleTabs.push_back(state.tab);
        }

        for (const TabDefinition* tab : visibleTabs) {
            ribbonBar->addTab(tab->id);
        }

        ribbonBar->setContextTabPresent(!activeContextTabs.empty());

        pageBuilt.assign(visibleTabs.size(), false);

        if (visibleTabs.empty()) {
            return;
        }

        if (!activeContextTabs.empty()) {
            selected = static_cast<int>(visibleTabs.size()) - 1;
        }
        else if (selected < 0) {
            selected = 0;
        }

        ribbonBar->setCurrentIndex(selected);
    }

    buildPage(selected);
}

void RibbonManager::buildPage(int index)
{
    if (ribbonBar.isNull()) {
        return;
    }

    if (index < 0 || index >= static_cast<int>(visibleTabs.size())) {
        return;
    }

    if (index < static_cast<int>(pageBuilt.size()) && pageBuilt[index]) {
        return;
    }

    ribbonBar->setPage(index, createPage(*visibleTabs[index]));

    if (index < static_cast<int>(pageBuilt.size())) {
        pageBuilt[index] = true;
    }
}

QWidget* RibbonManager::createPage(const TabDefinition& tab) const
{
    auto* page = new QWidget();
    page->setObjectName(QStringLiteral("RibbonPage"));
    // A plain QWidget only honours a stylesheet background with this attribute.
    page->setAttribute(Qt::WA_StyledBackground, true);

    auto* layout = new QHBoxLayout(page);
    layout->setContentsMargins(2, 1, 2, 0);
    layout->setSpacing(0);

    std::vector<RibbonPanel*> leading;
    std::vector<RibbonPanel*> trailing;

    for (const PanelDefinition& panelDefinition : tab.panels) {
        auto* panel = new RibbonPanel(panelDefinition.caption, page);

        for (const ItemDefinition& item : panelDefinition.items) {
            auto* button = new RibbonButton(panel);
            const bool bound = button->setCommand(
                item.command,
                item.label,
                item.subCommands,
                ButtonSize::Large,
                item.optional
            );
            if (!bound) {
                delete button;
                continue;
            }

            button->setPrimary(item.primary);
            panel->addButton(button);
        }

        panel->setCaptionMenu(createPanelMenu(panelDefinition, panel));

        if (panel->isEmpty()) {
            delete panel;
            continue;
        }

        if (panelDefinition.alignRight) {
            trailing.push_back(panel);
        }
        else {
            leading.push_back(panel);
        }
    }

    for (RibbonPanel* panel : leading) {
        layout->addWidget(panel);
    }

    if (!leading.empty()) {
        leading.back()->setSeparatorVisible(false);
    }

    layout->addStretch(1);

    for (RibbonPanel* panel : trailing) {
        layout->addWidget(panel);
    }

    if (!trailing.empty()) {
        trailing.back()->setSeparatorVisible(false);
    }

    return page;
}

QMenu* RibbonManager::createPanelMenu(const PanelDefinition& panel, QWidget* parent)
{
    if (panel.menuItems.empty()) {
        return nullptr;
    }

    auto* menu = new QMenu(parent);
    menu->setObjectName(QStringLiteral("RibbonPanelMenu"));

    for (const ItemDefinition& item : panel.menuItems) {
        Gui::Action* guiAction = resolveGuiAction(item.command);
        QAction* action = guiAction ? guiAction->action() : nullptr;

        if (!item.command.isEmpty() && !action) {
            reportMissing(item.command, item.optional);
            continue;
        }

        QList<QAction*> children;
        for (const QString& subCommand : item.subCommands) {
            if (QAction* child = RibbonButton::resolveAction(subCommand)) {
                children.append(child);
            }
            else {
                reportMissing(subCommand, item.optional);
            }
        }

        // A group command already carries its variants, which is how a single
        // FreeCAD command stands in for a row of Fusion entries.
        Gui::ActionGroup* group = nullptr;
        if (children.isEmpty()) {
            group = qobject_cast<Gui::ActionGroup*>(guiAction);
            if (group) {
                children = group->actions();
            }
        }

        if (!children.isEmpty()) {
            const QString title = item.label.isEmpty() && action ? action->text() : item.label;
            if (title.isEmpty()) {
                continue;
            }

            QMenu* submenu = menu->addMenu(title);
            if (action) {
                submenu->setIcon(action->icon());
            }
            submenu->addActions(children);
            RibbonButton::followGroupMenu(group, submenu);
            continue;
        }

        if (!action) {
            continue;
        }

        menu->addAction(
            item.label.isEmpty() ? action : createLabelledAction(action, item.label, menu)
        );
    }

    if (menu->isEmpty()) {
        delete menu;
        return nullptr;
    }

    return menu;
}

#include "moc_RibbonManager.cpp"
