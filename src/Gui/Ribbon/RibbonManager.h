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

#include <map>
#include <vector>

#include <QObject>
#include <QPointer>
#include <QString>
#include <QStringList>

#include <FCGlobal.h>

// QPointer needs the complete type of its target.
#include "RibbonBar.h"

class QJsonObject;
class QMenu;
class QWidget;

namespace Gui
{

class ToolBarItem;

namespace Ribbon
{

/**
 * Owns the ribbon content: it reads the workspace definition, resolves the
 * command names it contains through the command manager, builds the tabs and
 * activates the workbench that backs a tab when the user selects it.
 *
 * Workbenches the definition does not describe (third-party add-ons) get a tab
 * generated from the ToolBarItem tree they hand to Workbench::activate(), so
 * they stay reachable.
 * @author FuCad contributors
 */
class GuiExport RibbonManager: public QObject
{
    Q_OBJECT

public:
    /// The one and only instance.
    static RibbonManager* instance();
    static void destruct();

    /**
     * Whether the ribbon shell replaces the classic toolbars and menu bar.
     * Reads BaseApp/Preferences/MainWindow/UseRibbon, defaulting to true.
     */
    static bool isEnabled();

    /// Takes over \a bar and starts following workbench activations.
    void attach(RibbonBar* bar);

    /**
     * Keeps the toolbar tree of \a workbench so that a tab can be generated for
     * workbenches the workspace definition does not describe.
     */
    void rememberToolBars(const QString& workbench, const ToolBarItem* toolBars);

    /**
     * Appends the context tab \a id of the workspace definition to the tab strip
     * and selects it, the way Fusion swaps to a contextual tab when a mode such
     * as sketch editing starts.
     *
     * Pushing an id that is already shown only selects it again, and pushing an
     * id the definition does not describe does nothing, so a caller may push
     * from a hook that fires more than once without tracking its own state.
     */
    void pushContextTab(const QString& id);

    /**
     * Drops the context tab \a id again and selects the tab that was current
     * when it was pushed. Popping an id that is not shown does nothing.
     */
    void popContextTab(const QString& id);

private:
    struct ItemDefinition
    {
        QString command;
        /// Fusion wording shown instead of the menu text the command registered.
        QString label;
        QStringList subCommands;
        bool primary {false};
        /// Comes from a module the page does not pull in, so its absence is not
        /// a mistake in the definition and must not be reported as one.
        bool optional {false};
    };

    struct PanelDefinition
    {
        QString caption;
        /// Workbench whose Initialize() has to run before the panel's commands exist.
        /// A Python workbench registers its commands on first activation, so without
        /// this the panel would stay empty until the user visited that workbench.
        QString initWorkbench;
        /// Pushed to the trailing edge of the page instead of packed to the left.
        bool alignRight {false};
        /// The frequently used entries, shown as buttons in the panel itself.
        std::vector<ItemDefinition> items;
        /// The full command set, reached through the caption drop-down. Empty
        /// leaves the caption the plain label it has always been.
        std::vector<ItemDefinition> menuItems;
    };

    struct TabDefinition
    {
        QString id;
        QString workbench;
        bool optional {false};
        /// Only on the strip while a mode pushed it, and never auto-activates
        /// its workbench: the mode that owns it decides which one is active.
        bool context {false};
        std::vector<PanelDefinition> panels;
    };

    struct ContextTabState
    {
        const TabDefinition* tab {nullptr};
        /// Id of the tab to select again once this one is popped.
        QString restore;
    };

    RibbonManager();
    ~RibbonManager() override;

    void onWorkbenchActivated(const QString& workbench);
    void onTabActivated(int index);

    void loadWorkspace();
    void rebuildTabs(const QString& workbench);
    void buildPage(int index);
    QWidget* createPage(const TabDefinition& tab) const;
    /// The caption drop-down of \a panel, or nullptr when nothing resolved.
    static QMenu* createPanelMenu(const PanelDefinition& panel, QWidget* parent);

    const TabDefinition* findContextTab(const QString& id) const;
    int indexOfTab(const TabDefinition* tab) const;
    int indexOfTab(const QString& id) const;

    static bool parseTab(const QJsonObject& source, TabDefinition& tab);
    static bool parsePanel(const QJsonObject& source, PanelDefinition& panel);
    static bool parseItem(const QJsonObject& source, ItemDefinition& item);

    static RibbonManager* _instance;

    QPointer<RibbonBar> ribbonBar;
    std::vector<TabDefinition> workspaceTabs;
    /// Filled once by loadWorkspace(), so that pushed tabs stay addressable.
    std::vector<TabDefinition> contextTabs;
    /// Innermost push last; every entry points into contextTabs.
    std::vector<ContextTabState> activeContextTabs;
    /// Points into workspaceTabs, contextTabs and generatedTab, all of which
    /// outlive it.
    std::vector<const TabDefinition*> visibleTabs;
    std::vector<bool> pageBuilt;
    std::map<QString, std::vector<PanelDefinition>> rememberedToolBars;
    TabDefinition generatedTab;
    bool workspaceLoaded {false};
    bool updating {false};

    Q_DISABLE_COPY(RibbonManager)
};

}  // namespace Ribbon
}  // namespace Gui
