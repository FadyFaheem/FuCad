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

#include "TaskFaceOperationParameters.h"

namespace Gui
{
class QuantitySpinBox;
}

namespace PartDesignGui
{

class ViewProviderOffsetFace;
class ViewProviderMoveFace;
class ViewProviderDeleteFace;

/// The face list plus the distance the faces travel along their normal.
class TaskOffsetFaceParameters: public TaskFaceOperationParameters
{
    Q_OBJECT

public:
    explicit TaskOffsetFaceParameters(ViewProviderDressUp* view, QWidget* parent = nullptr);

protected:
    void addParameters(QFormLayout* layout) override;

private:
    Gui::QuantitySpinBox* offset;
};

/// The face list plus the vector the faces travel along.
class TaskMoveFaceParameters: public TaskFaceOperationParameters
{
    Q_OBJECT

public:
    explicit TaskMoveFaceParameters(ViewProviderDressUp* view, QWidget* parent = nullptr);

protected:
    void addParameters(QFormLayout* layout) override;

private:
    void onComponentChanged();

    Gui::QuantitySpinBox* along[3] {nullptr, nullptr, nullptr};
};

/// Only the face list: there is nothing to say about a face being removed.
class TaskDeleteFaceParameters: public TaskFaceOperationParameters
{
    Q_OBJECT

public:
    explicit TaskDeleteFaceParameters(ViewProviderDressUp* view, QWidget* parent = nullptr);
};

class TaskDlgOffsetFaceParameters: public TaskDlgDressUpParameters
{
    Q_OBJECT

public:
    explicit TaskDlgOffsetFaceParameters(ViewProviderOffsetFace* view);
};

class TaskDlgMoveFaceParameters: public TaskDlgDressUpParameters
{
    Q_OBJECT

public:
    explicit TaskDlgMoveFaceParameters(ViewProviderMoveFace* view);
};

class TaskDlgDeleteFaceParameters: public TaskDlgDressUpParameters
{
    Q_OBJECT

public:
    explicit TaskDlgDeleteFaceParameters(ViewProviderDeleteFace* view);
};

}  // namespace PartDesignGui
