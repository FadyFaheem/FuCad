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

#include <App/PropertyGeo.h>

#include "FeatureFaceOperation.h"

namespace PartDesign
{

/**
 * Fusion's Move Face: takes the chosen faces along a direction of the user's
 * choosing rather than along their own normal.
 * @author FuCad contributors
 */
class PartDesignExport MoveFace: public SweptFace
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesign::MoveFace);

public:
    MoveFace();

    App::PropertyVectorDistance Direction;

    short mustExecute() const override;
    const char* getViewProviderName() const override
    {
        return "PartDesignGui::ViewProviderMoveFace";
    }

protected:
    gp_Vec displacement(const TopoDS_Face& face) const override;
};

}  // namespace PartDesign
