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


#include <QFont>
#include <QFontMetrics>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QSizePolicy>
#include <QToolButton>
#include <QVBoxLayout>

#include "RibbonPanel.h"


using namespace Gui::Ribbon;

namespace
{
constexpr int panelMargin = 8;
constexpr int captionPointSizeDelta = 1;
constexpr double minimumCaptionPointSize = 6.0;
constexpr int separatorWidth = 1;
// Only the two border pixels the stylesheet draws: the caption row has to keep
// the height of the label it replaces, or the page outgrows the ribbon.
constexpr int captionButtonBorder = 2;
constexpr int captionButtonMinimumWidth = 56;
}  // namespace


RibbonPanel::RibbonPanel(const QString& caption, QWidget* parent)
    : QWidget(parent)
    , bodyLayout(nullptr)
    , buttonLayout(nullptr)
    , captionWidget(nullptr)
    , separator(nullptr)
    , captionText(caption)
{
    setObjectName(QStringLiteral("RibbonPanel"));
    setAttribute(Qt::WA_StyledBackground, true);

    auto* outerLayout = new QHBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(0);

    auto* body = new QWidget(this);
    body->setObjectName(QStringLiteral("RibbonPanelBody"));
    bodyLayout = new QVBoxLayout(body);
    bodyLayout->setContentsMargins(panelMargin, 2, panelMargin, 1);
    bodyLayout->setSpacing(0);

    auto* buttonRow = new QWidget(body);
    buttonRow->setObjectName(QStringLiteral("RibbonPanelButtons"));
    buttonLayout = new QHBoxLayout(buttonRow);
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setSpacing(1);
    buttonLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    auto* captionLabel = new QLabel(captionText, body);
    captionLabel->setObjectName(QStringLiteral("RibbonPanelCaption"));
    captionLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    applyCaptionFont(captionLabel);
    captionWidget = captionLabel;

    bodyLayout->addWidget(buttonRow, 1);
    bodyLayout->addWidget(captionWidget, 0);

    // A plain widget rather than a QFrame line: the rule is painted from the
    // stylesheet so that it picks up the border colour of the active theme.
    separator = new QWidget(this);
    separator->setObjectName(QStringLiteral("RibbonPanelSeparator"));
    separator->setAttribute(Qt::WA_StyledBackground, true);
    separator->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    separator->setFixedWidth(separatorWidth);

    outerLayout->addWidget(body);
    outerLayout->addWidget(separator);
}

void RibbonPanel::addButton(QWidget* button)
{
    if (!button) {
        return;
    }

    buttonLayout->addWidget(button);
    ++buttonCount;
}

void RibbonPanel::setCaptionMenu(QMenu* menu)
{
    if (!menu || captionMenu) {
        return;
    }

    auto* button = new QToolButton(captionWidget->parentWidget());
    button->setObjectName(QStringLiteral("RibbonPanelCaptionButton"));
    button->setText(captionText);
    button->setToolButtonStyle(Qt::ToolButtonTextOnly);
    button->setPopupMode(QToolButton::InstantPopup);
    button->setAutoRaise(true);
    button->setFocusPolicy(Qt::NoFocus);
    button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    applyCaptionFont(button);
    button->setMinimumWidth(captionButtonMinimumWidth);
    button->setFixedHeight(QFontMetrics(button->font()).height() + captionButtonBorder);

    button->setMenu(menu);
    captionMenu = menu;

    const int index = bodyLayout->indexOf(captionWidget);
    bodyLayout->removeWidget(captionWidget);
    delete captionWidget;
    captionWidget = button;
    bodyLayout->insertWidget(index, captionWidget, 0);
}

bool RibbonPanel::isEmpty() const
{
    return buttonCount == 0 && !captionMenu;
}

void RibbonPanel::applyCaptionFont(QWidget* widget) const
{
    QFont captionFont = widget->font();
    if (captionFont.pointSizeF() <= 0.0) {
        return;
    }

    captionFont.setPointSizeF(
        qMax(minimumCaptionPointSize, captionFont.pointSizeF() - captionPointSizeDelta)
    );
    widget->setFont(captionFont);
}

void RibbonPanel::setSeparatorVisible(bool visible)
{
    separator->setVisible(visible);
}

#include "moc_RibbonPanel.cpp"
