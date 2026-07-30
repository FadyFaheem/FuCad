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


#include <QFrame>
#include <QSignalBlocker>
#include <QSizePolicy>
#include <QStackedWidget>
#include <QStyle>
#include <QTabBar>
#include <QVBoxLayout>

#include "RibbonBar.h"


using namespace Gui::Ribbon;

namespace
{
// Matches Fusion: a tab strip over a page tall enough for a row of icon-only
// buttons, plus the panel caption underneath it.
constexpr int ribbonBarHeight = 86;
// Selected on by the stylesheet as QTabBar#RibbonTabBar[contextTab="true"].
constexpr const char* contextTabProperty = "contextTab";
}  // namespace


RibbonBar::RibbonBar(QWidget* parent)
    : QWidget(parent)
    , tabBar(nullptr)
    , pageStack(nullptr)
{
    setObjectName(QStringLiteral("RibbonBar"));
    setAttribute(Qt::WA_StyledBackground, true);
    setFixedHeight(ribbonBarHeight);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    tabBar = new QTabBar(this);
    tabBar->setObjectName(QStringLiteral("RibbonTabBar"));
    tabBar->setExpanding(false);
    tabBar->setDrawBase(false);
    tabBar->setUsesScrollButtons(true);
    tabBar->setElideMode(Qt::ElideNone);
    tabBar->setFocusPolicy(Qt::NoFocus);
    tabBar->setProperty(contextTabProperty, false);

    pageStack = new QStackedWidget(this);
    pageStack->setObjectName(QStringLiteral("RibbonPages"));
    pageStack->setFrameShape(QFrame::NoFrame);

    layout->addWidget(tabBar, 0);
    layout->addWidget(pageStack, 1);

    connect(tabBar, &QTabBar::currentChanged, this, [this](int index) {
        if (index >= 0 && index < pageStack->count()) {
            pageStack->setCurrentIndex(index);
        }
        Q_EMIT tabActivated(index);
    });
}

void RibbonBar::clear()
{
    const QSignalBlocker blocker(tabBar);

    while (tabBar->count() > 0) {
        tabBar->removeTab(0);
    }

    while (pageStack->count() > 0) {
        QWidget* page = pageStack->widget(0);
        pageStack->removeWidget(page);
        delete page;
    }
}

int RibbonBar::addTab(const QString& title)
{
    const QSignalBlocker blocker(tabBar);

    const int index = tabBar->addTab(title);
    pageStack->insertWidget(index, new QWidget(pageStack));
    return index;
}

void RibbonBar::removeTab(int index)
{
    if (index < 0 || index >= tabBar->count()) {
        return;
    }

    const QSignalBlocker blocker(tabBar);

    tabBar->removeTab(index);

    if (index < pageStack->count()) {
        QWidget* page = pageStack->widget(index);
        pageStack->removeWidget(page);
        delete page;
    }
}

int RibbonBar::count() const
{
    return tabBar->count();
}

void RibbonBar::setContextTabPresent(bool present)
{
    if (tabBar->property(contextTabProperty).toBool() == present) {
        return;
    }

    tabBar->setProperty(contextTabProperty, present);

    // A rule that selects on a dynamic property is only re-evaluated once the
    // style is asked to look at the widget again.
    tabBar->style()->unpolish(tabBar);
    tabBar->style()->polish(tabBar);
    tabBar->update();
}

QString RibbonBar::tabText(int index) const
{
    return tabBar->tabText(index);
}

void RibbonBar::setPage(int index, QWidget* page)
{
    if (!page) {
        return;
    }

    if (index < 0 || index >= pageStack->count()) {
        delete page;
        return;
    }

    QWidget* previous = pageStack->widget(index);
    const bool wasCurrent = pageStack->currentIndex() == index;

    pageStack->insertWidget(index, page);
    pageStack->removeWidget(previous);
    delete previous;

    if (wasCurrent) {
        pageStack->setCurrentIndex(index);
    }
}

int RibbonBar::currentIndex() const
{
    return tabBar->currentIndex();
}

void RibbonBar::setCurrentIndex(int index)
{
    if (index < 0 || index >= tabBar->count()) {
        return;
    }

    tabBar->setCurrentIndex(index);
    pageStack->setCurrentIndex(index);
}

#include "moc_RibbonBar.cpp"
