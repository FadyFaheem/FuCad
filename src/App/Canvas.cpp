// SPDX-License-Identifier: LGPL-2.1-or-later

/***************************************************************************
 *   Copyright (c) 2026 FuCad contributors                                 *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/

#include <Base/Tools.h>
#include <Base/Vector3D.h>

#include "Canvas.h"


using namespace Image;

PROPERTY_SOURCE(Image::Canvas, Image::ImagePlane)


Canvas::Canvas()
{
    static const char* group = "Canvas";

    ADD_PROPERTY_TYPE(
        AttachmentSupport,
        (nullptr),
        group,
        App::Prop_None,
        "Face or plane the canvas is laid on"
    );
    ADD_PROPERTY_TYPE(
        BasePlacement,
        (Base::Placement()),
        group,
        App::Prop_Hidden,
        "Frame of the supporting face"
    );
    ADD_PROPERTY_TYPE(XDistance, (0.0), group, App::Prop_None, "Offset along the plane X axis");
    ADD_PROPERTY_TYPE(YDistance, (0.0), group, App::Prop_None, "Offset along the plane Y axis");
    ADD_PROPERTY_TYPE(Offset, (0.0), group, App::Prop_None, "Lift off the face along its normal");
    ADD_PROPERTY_TYPE(ZAngle, (0.0), group, App::Prop_None, "Rotation within the plane");
    ADD_PROPERTY_TYPE(FlipHorizontal, (false), group, App::Prop_None, "Mirror the image left to right");
    ADD_PROPERTY_TYPE(FlipVertical, (false), group, App::Prop_None, "Mirror the image top to bottom");

    // Placement is derived from the base frame and the offsets above
    Placement.setStatus(App::Property::ReadOnly, true);
}

Base::Placement Canvas::computePlacement() const
{
    Base::Placement local(
        Base::Vector3d(XDistance.getValue(), YDistance.getValue(), Offset.getValue()),
        Base::Rotation(Base::Vector3d(0.0, 0.0, 1.0), Base::toRadians(ZAngle.getValue()))
    );

    return BasePlacement.getValue() * local;
}

void Canvas::onChanged(const App::Property* prop)
{
    const bool offsetChanged = prop == &BasePlacement || prop == &XDistance || prop == &YDistance
        || prop == &Offset || prop == &ZAngle;

    if (offsetChanged && getDocument() && !isRestoring()) {
        Placement.setValue(computePlacement());
    }

    ImagePlane::onChanged(prop);
}

App::DocumentObjectExecReturn* Canvas::execute()
{
    Placement.setValue(computePlacement());
    return App::DocumentObject::StdReturn;
}
