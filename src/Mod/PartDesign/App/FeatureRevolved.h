// SPDX-License-Identifier: LGPL-2.1-or-later

/***************************************************************************
 *   Copyright (c) 2010 Juergen Riegel <FreeCAD@juergen-riegel.net>        *
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

#include <App/PropertyUnits.h>
#include "FeatureSketchBased.h"

namespace Part
{
enum class RevolMode;
}

namespace PartDesign
{

class PartDesignExport Revolved: public ProfileBased
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesign::Revolved);

public:
    Revolved();

    App::PropertyEnumeration Type;
    App::PropertyVector Base;
    App::PropertyVector Axis;
    App::PropertyAngle Angle;
    App::PropertyAngle Angle2;

    /** if this property is set to a valid link, both Axis and Base properties
     *  are calculated according to the linked line
     */
    App::PropertyLinkSub ReferenceAxis;

    /** @name methods override feature */
    //@{
    short mustExecute() const override;
    //@}

    /// suggests a value for Reversed flag so that material is always added to the support
    bool suggestReversed();

    enum class RevolMethod
    {
        Angle,
        ThroughAll,
        ToLast = ThroughAll,
        ToFirst,
        ToFace,
        TwoAngles
    };

protected:
    /** Recalculate the feature
     * Revolves the Sketch around the given Axis (with basepoint Base)
     * The angle of the revolution is given by Angle.
     * If Midplane is true, then the revolution will extend for half of Angle on both sides of the
     * sketch plane. If Reversed is true then the direction of revolution will be reversed. The
     * created material is combined with the preceding solid as told by the Operation property.
     */
    App::DocumentObjectExecReturn* executeRevolved();

    /// Fuses the revolution into the base; Revolution overrides it to honour its FuseOrder
    virtual TopoShape makeFused(const TopoShape& base, const TopoShape& revolve) const;

private:
    App::DocumentObjectExecReturn* tryExecuteRevolved();
    TopoShape tryGetBaseShape() const;
    TopoShape tryGetSupportShape() const;
    TopoShape tryToRevolveToFace(
        const TopoShape& upToFace,
        gp_Pnt pnt,
        gp_Dir dir,
        TopoShape& base,
        TopoShape& supportface,
        const TopoShape& sketchshape,
        Part::RevolMode revolMode
    ) const;
    void setResult(const TopoShape& base, const TopoShape& revolved, bool combineWithBase);

    /// Applies the boolean selected by the Operation property against the base
    TopoShape combineShapes(const TopoShape& base, const TopoShape& revolved) const;

    /// updates Axis from ReferenceAxis
    void updateAxis();

    virtual bool suggestReversedAngle(double angle) const = 0;

    /**
     * Generates a revolution of the input sketchshape and stores it in the given \a revol.
     */
    void generateRevolution(
        TopoShape& revol,
        const TopoShape& sketchshape,
        const gp_Ax1& ax1,
        double angle,
        double angle2,
        bool midplane,
        bool reversed,
        RevolMethod method
    );

    /**
     * Generates a revolution of the input \a profileshape.
     * It will be a stand-alone solid created with BRepFeat_MakeRevol.
     */
    void generateRevolution(
        TopoShape& revol,
        const TopoShape& baseshape,
        const TopoDS_Shape& profileshape,
        const TopoDS_Face& supportface,
        const TopoDS_Face& uptoface,
        const gp_Ax1& ax1,
        RevolMethod method,
        Part::RevolMode Mode,
        Standard_Boolean Modify
    );

    /**
     * Disables settings that are not valid for the current method
     */
    void updateProperties(RevolMethod method);

    static const App::PropertyAngle::Constraints floatAngle;
};

}  // namespace PartDesign
