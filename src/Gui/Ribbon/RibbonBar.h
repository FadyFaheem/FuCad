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
#include <QWidget>

#include <FCGlobal.h>

class QStackedWidget;
class QTabBar;

namespace Gui
{
namespace Ribbon
{

/**
 * The tab strip of the Fusion-style shell: a QTabBar stacked on top of a
 * QStackedWidget so that selecting a tab swaps the row of tool panels below it.
 *
 * The bar only owns the presentation; RibbonManager decides which tabs exist
 * and fills the pages.
 * @author FuCad contributors
 */
class GuiExport RibbonBar: public QWidget
{
    Q_OBJECT

public:
    explicit RibbonBar(QWidget* parent = nullptr);
    ~RibbonBar() override = default;

    /// Drops every tab and page. Does not emit tabActivated().
    void clear();
    /// Appends a tab with an empty page and returns its index.
    int addTab(const QString& title);
    /// Drops the tab at \a index and its page. Does not emit tabActivated().
    void removeTab(int index);
    int count() const;
    QString tabText(int index) const;

    /**
     * Tells the stylesheet whether the trailing tab is a context tab, so that
     * it can tint it the way Fusion tints the tab a mode adds.
     */
    void setContextTabPresent(bool present);

    /// Replaces the page at \a index; the bar takes ownership of \a page and
    /// destroys whatever was there before.
    void setPage(int index, QWidget* page);

    int currentIndex() const;
    void setCurrentIndex(int index);

Q_SIGNALS:
    void tabActivated(int index);

private:
    QTabBar* tabBar;
    QStackedWidget* pageStack;

    Q_DISABLE_COPY(RibbonBar)
};

}  // namespace Ribbon
}  // namespace Gui
