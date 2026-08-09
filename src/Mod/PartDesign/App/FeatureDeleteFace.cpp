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

#include <vector>

#include <Standard_Failure.hxx>
#include <TopoDS_Shape.hxx>

#include <Base/Exception.h>

#include "FeatureDeleteFace.h"

FC_LOG_LEVEL_INIT("PartDesign", true, true)

using namespace PartDesign;

PROPERTY_SOURCE(PartDesign::DeleteFace, PartDesign::FaceOperation)

App::DocumentObjectExecReturn* DeleteFace::execute()
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
        this->positionByBaseFeature();
        this->Shape.setValue(base);
        return App::DocumentObject::StdReturn;
    }

    this->Placement.setValue(Base::Placement());

    std::vector<TopoDS_Shape> removals(faces.begin(), faces.end());

    Part::TopoShape result;
    try {
        result = Part::TopoShape(base.defeaturing(removals));
    }
    catch (Standard_Failure& e) {
        FC_ERR("Exception while deleting a face: " << e.GetMessageString());
        return new App::DocumentObjectExecReturn(
            QT_TRANSLATE_NOOP("Exception", "Cannot close the shape after deleting the faces")
        );
    }
    catch (Base::Exception& e) {
        return new App::DocumentObjectExecReturn(e.what());
    }

    if (result.isNull()) {
        return new App::DocumentObjectExecReturn(
            QT_TRANSLATE_NOOP("Exception", "Cannot close the shape after deleting the faces")
        );
    }

    this->rawShape = result;
    result = refineShapeIfActive(result);
    this->Shape.setValue(getSolid(result));
    return App::DocumentObject::StdReturn;
}
