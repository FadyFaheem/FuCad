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

#pragma once

#include <App/ImagePlane.h>
#include <App/PropertyLinks.h>
#include <App/PropertyStandard.h>
#include <App/PropertyUnits.h>

namespace Image
{

/**
 * A reference image laid onto a planar face or datum plane.
 *
 * The placement is not edited directly. It is derived from BasePlacement, which
 * describes the frame of the face the canvas sits on, combined with the in-plane
 * offsets so that the canvas can be nudged around without losing its plane.
 */
class AppExport Canvas: public ImagePlane
{
    PROPERTY_HEADER_WITH_OVERRIDE(Image::Canvas);

public:
    Canvas();
    ~Canvas() override = default;

    /// Face or plane the canvas is laid on, kept for reference and re-editing
    App::PropertyLinkSub AttachmentSupport;
    /// Frame of the supporting face: origin on the face, Z along its normal
    App::PropertyPlacement BasePlacement;

    App::PropertyDistance XDistance;
    App::PropertyDistance YDistance;
    /// Lift off the face along its normal, keeps the canvas clear of the surface
    App::PropertyDistance Offset;
    App::PropertyAngle ZAngle;

    App::PropertyBool FlipHorizontal;
    App::PropertyBool FlipVertical;

    App::DocumentObjectExecReturn* execute() override;
    void onChanged(const App::Property* prop) override;

    /// Placement of the canvas plane derived from the base frame and the offsets
    Base::Placement computePlacement() const;

    const char* getViewProviderName() const override
    {
        return "Gui::ViewProviderCanvas";
    }
};

}  // namespace Image
