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

#include <array>

#include <QFormLayout>

#include <Base/Quantity.h>
#include <Base/Vector3D.h>
#include <Gui/QuantitySpinBox.h>
#include <Mod/PartDesign/App/FeatureDeleteFace.h>
#include <Mod/PartDesign/App/FeatureMoveFace.h>
#include <Mod/PartDesign/App/FeatureOffsetFace.h>

#include "TaskFaceToolParameters.h"
#include "ViewProviderFaceTools.h"

using namespace PartDesignGui;

namespace
{
/// The bound the other PartDesign length fields use, in both directions because
/// these tools take material away for a negative distance.
constexpr double lengthLimit = 999999999.0;

/// A length field of the kind these panels use, already bound to \a property.
Gui::QuantitySpinBox* lengthField(QWidget* parent, App::PropertyQuantity& property)
{
    auto* field = new Gui::QuantitySpinBox(parent);
    field->setUnit(Base::Unit::Length);
    field->setKeyboardTracking(false);
    field->setMinimum(-lengthLimit);
    field->setMaximum(lengthLimit);
    field->setSingleStep(1.0);
    field->setValue(property.getValue());
    field->bind(property);
    return field;
}
}  // namespace


TaskOffsetFaceParameters::TaskOffsetFaceParameters(ViewProviderDressUp* view, QWidget* parent)
    : TaskFaceOperationParameters(view, parent)
    , offset(nullptr)
{}

void TaskOffsetFaceParameters::addParameters(QFormLayout* layout)
{
    auto* feature = getObject<PartDesign::OffsetFace>();

    offset = lengthField(proxy, feature->Offset);
    layout->addRow(tr("Offset"), offset);

    connect(offset, qOverload<double>(&Gui::QuantitySpinBox::valueChanged), this, [this](double value) {
        if (auto* changing = beforeChange()) {
            static_cast<PartDesign::OffsetFace*>(changing)->Offset.setValue(value);
            afterChange(changing);
        }
    });
}


TaskMoveFaceParameters::TaskMoveFaceParameters(ViewProviderDressUp* view, QWidget* parent)
    : TaskFaceOperationParameters(view, parent)
{}

void TaskMoveFaceParameters::addParameters(QFormLayout* layout)
{
    auto* feature = getObject<PartDesign::MoveFace>();
    const Base::Vector3d travel = feature->Direction.getValue();
    const std::array<double, 3> components = {travel.x, travel.y, travel.z};
    const std::array<QString, 3> labels = {tr("Along X"), tr("Along Y"), tr("Along Z")};

    for (int axis = 0; axis < 3; ++axis) {
        auto* field = new Gui::QuantitySpinBox(proxy);
        field->setUnit(Base::Unit::Length);
        field->setKeyboardTracking(false);
        field->setMinimum(-lengthLimit);
        field->setMaximum(lengthLimit);
        field->setSingleStep(1.0);
        field->setValue(components.at(axis));

        layout->addRow(labels.at(axis), field);
        along[axis] = field;

        connect(field, qOverload<double>(&Gui::QuantitySpinBox::valueChanged), this, [this](double) {
            onComponentChanged();
        });
    }
}

void TaskMoveFaceParameters::onComponentChanged()
{
    auto* changing = beforeChange();
    if (!changing) {
        return;
    }

    static_cast<PartDesign::MoveFace*>(changing)->Direction.setValue(
        along[0]->value().getValue(),
        along[1]->value().getValue(),
        along[2]->value().getValue()
    );
    afterChange(changing);
}


TaskDeleteFaceParameters::TaskDeleteFaceParameters(ViewProviderDressUp* view, QWidget* parent)
    : TaskFaceOperationParameters(view, parent)
{}


TaskDlgOffsetFaceParameters::TaskDlgOffsetFaceParameters(ViewProviderOffsetFace* view)
    : TaskDlgDressUpParameters(view)
{
    parameter = new TaskOffsetFaceParameters(view);
    Content.push_back(parameter);
    Content.push_back(preview);
}

TaskDlgMoveFaceParameters::TaskDlgMoveFaceParameters(ViewProviderMoveFace* view)
    : TaskDlgDressUpParameters(view)
{
    parameter = new TaskMoveFaceParameters(view);
    Content.push_back(parameter);
    Content.push_back(preview);
}

TaskDlgDeleteFaceParameters::TaskDlgDeleteFaceParameters(ViewProviderDeleteFace* view)
    : TaskDlgDressUpParameters(view)
{
    parameter = new TaskDeleteFaceParameters(view);
    Content.push_back(parameter);
    Content.push_back(preview);
}

#include "moc_TaskFaceToolParameters.cpp"
