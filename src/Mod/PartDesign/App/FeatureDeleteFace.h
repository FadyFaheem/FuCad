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

#include "FeatureFaceOperation.h"

namespace PartDesign
{

/**
 * Fusion's Delete Face: takes the chosen faces out of the solid and lets the
 * surfaces around them grow together to close the gap, which is how a hole or a
 * fillet is removed without having to find the feature that made it.
 * @author FuCad contributors
 */
class PartDesignExport DeleteFace: public FaceOperation
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesign::DeleteFace);

public:
    DeleteFace() = default;

    App::DocumentObjectExecReturn* execute() override;
    const char* getViewProviderName() const override
    {
        return "PartDesignGui::ViewProviderDeleteFace";
    }
};

}  // namespace PartDesign
