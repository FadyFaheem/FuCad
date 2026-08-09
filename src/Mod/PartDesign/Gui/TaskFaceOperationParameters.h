// SPDX-License-Identifier: LGPL-2.1-or-later

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

#include "TaskDressUpParameters.h"

class QFormLayout;
class QListWidget;
class QToolButton;

namespace PartDesignGui
{

/**
 * The panel the direct edit tools share: the button that starts picking, the
 * list of faces picked so far, and whatever single control the tool itself needs
 * underneath.
 *
 * Built in code rather than from a .ui file because each of these panels is a
 * list and at most one field, and three files of markup would say less than the
 * dozen lines here.
 * @author FuCad contributors
 */
class TaskFaceOperationParameters: public TaskDressUpParameters
{
    Q_OBJECT

public:
    explicit TaskFaceOperationParameters(ViewProviderDressUp* view, QWidget* parent = nullptr);
    ~TaskFaceOperationParameters() override;

    void apply() override;

protected:
    /**
     * Adds the controls of the tool to \a layout, which sits under the list of
     * faces. The default adds nothing, which is right for a tool that only needs
     * to know which faces to work on.
     */
    virtual void addParameters(QFormLayout* layout);

    /// Opens a transaction and hands back the feature, ready to be changed.
    PartDesign::DressUp* beforeChange();
    /// Recomputes and hides the feature again if that failed.
    void afterChange(PartDesign::DressUp* feature);

    void setButtons(selectionModes mode) override;
    void onSelectionChanged(const Gui::SelectionChanges& msg) override;

protected Q_SLOTS:
    void onRefDeleted() override;

private:
    void build();

    QToolButton* selectButton;
    QListWidget* references;
};

}  // namespace PartDesignGui
