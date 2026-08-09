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

#include "FeatureOffsetFace.h"

using namespace PartDesign;

PROPERTY_SOURCE(PartDesign::OffsetFace, PartDesign::SweptFace)

OffsetFace::OffsetFace()
{
    ADD_PROPERTY_TYPE(
        Offset,
        (1.0),
        "OffsetFace",
        App::Prop_None,
        "How far the faces travel along their normal. A negative distance takes material away."
    );
}

short OffsetFace::mustExecute() const
{
    if (Placement.isTouched() || Offset.isTouched()) {
        return 1;
    }

    return SweptFace::mustExecute();
}

gp_Vec OffsetFace::displacement(const TopoDS_Face& face) const
{
    return outwardNormal(face) * Offset.getValue();
}
