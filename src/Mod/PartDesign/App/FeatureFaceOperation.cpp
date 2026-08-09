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

#include <string>
#include <vector>

#include <BRepGProp_Face.hxx>
#include <Precision.hxx>
#include <Standard_Failure.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopoDS.hxx>
#include <gp_Pnt.hxx>

#include <Base/Exception.h>

#include "FeatureFaceOperation.h"

FC_LOG_LEVEL_INIT("PartDesign", true, true)

using namespace PartDesign;

PROPERTY_SOURCE_ABSTRACT(PartDesign::FaceOperation, PartDesign::DressUp)

std::vector<TopoDS_Face> FaceOperation::selectedFaces(const Part::TopoShape& shape) const
{
    std::vector<TopoDS_Face> faces;

    for (const std::string& name : Base.getSubValues(true)) {
        TopoDS_Shape sub;
        try {
            sub = shape.getSubShape(name.c_str());
        }
        catch (const Standard_Failure&) {
        }
        catch (const Base::Exception&) {
        }

        if (sub.IsNull() || sub.ShapeType() != TopAbs_FACE) {
            throw Base::ValueError("Invalid face reference");
        }

        faces.push_back(TopoDS::Face(sub));
    }

    return faces;
}

gp_Vec FaceOperation::outwardNormal(const TopoDS_Face& face)
{
    BRepGProp_Face prop(face);

    Standard_Real uMin = 0.0;
    Standard_Real uMax = 0.0;
    Standard_Real vMin = 0.0;
    Standard_Real vMax = 0.0;
    prop.Bounds(uMin, uMax, vMin, vMax);

    gp_Pnt point;
    gp_Vec normal;
    prop.Normal((uMin + uMax) / 2.0, (vMin + vMax) / 2.0, point, normal);

    if (normal.Magnitude() < Precision::Confusion()) {
        throw Base::ValueError("The face has no usable normal");
    }

    normal.Normalize();

    // BRepGProp_Face reports the normal of the underlying surface, which points
    // out of the solid only while the face runs the same way as that surface.
    if (face.Orientation() == TopAbs_REVERSED) {
        normal.Reverse();
    }

    return normal;
}


PROPERTY_SOURCE_ABSTRACT(PartDesign::SweptFace, PartDesign::FaceOperation)

App::DocumentObjectExecReturn* SweptFace::execute()
{
    if (onlyHaveRefined()) {
        return App::DocumentObject::StdReturn;
    }

    Part::TopoShape base;
    try {
        base = getBaseTopoShape();
    }
    catch (Base::Exception& e) {
        return new App::DocumentObjectExecReturn(e.what());
    }

    std::vector<TopoDS_Face> faces;
    try {
        faces = selectedFaces(base);
    }
    catch (Base::Exception& e) {
        return new App::DocumentObjectExecReturn(e.what());
    }

    if (faces.empty()) {
        // Nothing chosen yet, which is the state the dialog opens in.
        this->positionByBaseFeature();
        this->Shape.setValue(base);
        return App::DocumentObject::StdReturn;
    }

    // The booleans below work in the coordinates the base shape is already in.
    this->Placement.setValue(Base::Placement());

    Part::TopoShape result = base;
    try {
        for (const TopoDS_Face& face : faces) {
            const gp_Vec travel = displacement(face);
            if (travel.Magnitude() < Precision::Confusion()) {
                continue;
            }

            // Sweeping the face itself gives exactly the volume between where it
            // was and where it ends up, so the change is a boolean with that
            // volume rather than a rebuild of the solid.
            const Part::TopoShape swept = Part::TopoShape(face).makeElementPrism(travel);

            // Travelling along the outward normal leaves material behind the
            // face; travelling against it eats into what is there.
            if (travel.Dot(outwardNormal(face)) > 0.0) {
                result = result.makeElementFuse(swept);
            }
            else {
                result = result.makeElementCut(swept);
            }
        }
    }
    catch (Standard_Failure& e) {
        FC_ERR("Exception while moving a face: " << e.GetMessageString());
        return new App::DocumentObjectExecReturn(
            QT_TRANSLATE_NOOP("Exception", "Failed to change the face")
        );
    }
    catch (Base::Exception& e) {
        return new App::DocumentObjectExecReturn(e.what());
    }

    this->rawShape = result;
    result = refineShapeIfActive(result);
    this->Shape.setValue(getSolid(result));
    return App::DocumentObject::StdReturn;
}
