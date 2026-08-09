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

#include <vector>

#include <TopoDS_Face.hxx>
#include <gp_Vec.hxx>

#include "FeatureDressUp.h"

namespace PartDesign
{

/**
 * Base of the direct edit tools: features that take faces of the solid before
 * them and change the solid by acting on those faces, rather than by adding a
 * profile of their own.
 *
 * They are dress-ups because that is exactly the shape of the problem: a link to
 * the previous feature plus a list of its faces, which is also what gives them
 * the reference picking the fillet and chamfer dialogs already use.
 * @author FuCad contributors
 */
class PartDesignExport FaceOperation: public DressUp
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesign::FaceOperation);

public:
    FaceOperation() = default;

protected:
    /// The faces the Base property names, in the order it names them.
    std::vector<TopoDS_Face> selectedFaces(const Part::TopoShape& shape) const;

    /// The outward normal of \a face at the middle of its parameter range.
    static gp_Vec outwardNormal(const TopoDS_Face& face);
};

/**
 * A face operation that sweeps its faces through space and puts the swept volume
 * back into the solid: added where the sweep leaves the material, taken away
 * where it goes into it. Offsetting a face and moving one differ only in the
 * displacement they ask for, which is what the subclass provides.
 * @author FuCad contributors
 */
class PartDesignExport SweptFace: public FaceOperation
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesign::SweptFace);

public:
    SweptFace() = default;

    App::DocumentObjectExecReturn* execute() override;

protected:
    /// How far and in which direction \a face travels.
    virtual gp_Vec displacement(const TopoDS_Face& face) const = 0;
};

}  // namespace PartDesign
