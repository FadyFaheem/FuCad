// SPDX-License-Identifier: LGPL-2.1-or-later
/***************************************************************************
 *   Copyright (c) 2009 Jürgen Riegel <juergen.riegel@web.de>              *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#include <QAbstractButton>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QList>
#include <QSize>

#include "BitmapFactory.h"
#include "TaskEditControl.h"


using namespace Gui::TaskView;

namespace
{
constexpr int buttonIconExtent = 14;

/// The icon that says what a standard button does, or a null icon for the ones
/// that read plainly enough on their own.
QIcon iconForRole(QDialogButtonBox::ButtonRole role)
{
    switch (role) {
        case QDialogButtonBox::AcceptRole:
            return Gui::BitmapFactory().iconFromTheme("button_valid");
        case QDialogButtonBox::RejectRole:
            return Gui::BitmapFactory().iconFromTheme("button_invalid");
        case QDialogButtonBox::ApplyRole:
            return Gui::BitmapFactory().iconFromTheme("view-refresh");
        case QDialogButtonBox::HelpRole:
            return Gui::BitmapFactory().iconFromTheme("help-browser");
        default:
            return {};
    }
}
}  // namespace

TaskEditControl::TaskEditControl(QWidget* parent)
    : TaskWidget(parent)
{
    hboxLayout = new QHBoxLayout(this);
    buttonBox = new QDialogButtonBox(this);
    buttonBox->setObjectName(QStringLiteral("TaskEditControlButtons"));
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);

    // Fusion puts them at the trailing edge of the panel and marks accept and
    // cancel with a tick and a cross, so the pair can be read at a glance.
    buttonBox->setCenterButtons(false);
    hboxLayout->addStretch(1);
    hboxLayout->addWidget(buttonBox);

    decorateButtons();
}

void TaskEditControl::decorateButtons()
{
    for (QAbstractButton* button : buttonBox->buttons()) {
        if (!button->icon().isNull()) {
            continue;
        }

        const QIcon icon = iconForRole(buttonBox->buttonRole(button));
        if (!icon.isNull()) {
            button->setIcon(icon);
            button->setIconSize(QSize(buttonIconExtent, buttonIconExtent));
        }
    }
}

TaskEditControl::~TaskEditControl() = default;

QDialogButtonBox* TaskEditControl::standardButtons() const
{
    return buttonBox;
}

#include "moc_TaskEditControl.cpp"
