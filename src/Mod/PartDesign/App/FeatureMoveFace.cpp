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

#include <Base/Vector3D.h>

#include "FeatureMoveFace.h"

using namespace PartDesign;

PROPERTY_SOURCE(PartDesign::MoveFace, PartDesign::SweptFace)

MoveFace::MoveFace()
{
    ADD_PROPERTY_TYPE(
        Direction,
        (Base::Vector3d(0.0, 0.0, 1.0)),
        "MoveFace",
        App::Prop_None,
        "How far and in which direction the faces travel"
    );
}

short MoveFace::mustExecute() const
{
    if (Placement.isTouched() || Direction.isTouched()) {
        return 1;
    }

    return SweptFace::mustExecute();
}

gp_Vec MoveFace::displacement(const TopoDS_Face&) const
{
    const Base::Vector3d travel = Direction.getValue();
    return {travel.x, travel.y, travel.z};
}
