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
#include <QHBoxLayout>
#include <QLabel>
#include <QSizePolicy>
#include <QVBoxLayout>

#include "RibbonPanel.h"


using namespace Gui::Ribbon;

namespace
{
constexpr int panelMargin = 8;
constexpr int captionPointSizeDelta = 1;
constexpr double minimumCaptionPointSize = 6.0;
constexpr int separatorWidth = 1;
}  // namespace


RibbonPanel::RibbonPanel(const QString& caption, QWidget* parent)
    : QWidget(parent)
    , buttonLayout(nullptr)
    , separator(nullptr)
{
    setObjectName(QStringLiteral("RibbonPanel"));
    setAttribute(Qt::WA_StyledBackground, true);

    auto* outerLayout = new QHBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(0);

    auto* body = new QWidget(this);
    body->setObjectName(QStringLiteral("RibbonPanelBody"));
    auto* bodyLayout = new QVBoxLayout(body);
    bodyLayout->setContentsMargins(panelMargin, 2, panelMargin, 1);
    bodyLayout->setSpacing(0);

    auto* buttonRow = new QWidget(body);
    buttonRow->setObjectName(QStringLiteral("RibbonPanelButtons"));
    buttonLayout = new QHBoxLayout(buttonRow);
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setSpacing(1);
    buttonLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    auto* captionLabel = new QLabel(caption, body);
    captionLabel->setObjectName(QStringLiteral("RibbonPanelCaption"));
    captionLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    QFont captionFont = captionLabel->font();
    if (captionFont.pointSizeF() > 0.0) {
        captionFont.setPointSizeF(
            qMax(minimumCaptionPointSize, captionFont.pointSizeF() - captionPointSizeDelta)
        );
        captionLabel->setFont(captionFont);
    }

    bodyLayout->addWidget(buttonRow, 1);
    bodyLayout->addWidget(captionLabel, 0);

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

bool RibbonPanel::isEmpty() const
{
    return buttonCount == 0;
}

void RibbonPanel::setSeparatorVisible(bool visible)
{
    separator->setVisible(visible);
}

#include "moc_RibbonPanel.cpp"
