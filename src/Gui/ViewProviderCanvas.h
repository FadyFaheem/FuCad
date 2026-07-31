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

#include <Gui/ViewProviderImagePlane.h>

class SoDepthBuffer;
class SoPolygonOffset;

namespace Gui
{

/**
 * Displays a reference image laid on a face.
 *
 * Unlike a plain image plane a canvas normally sits flush against the geometry it
 * was placed on, so it is nudged forward with a polygon offset to keep it out of a
 * depth fight with that face. DisplayThrough drops the depth test entirely so the
 * canvas stays visible from behind the model, matching how tracing references work.
 */
class GuiExport ViewProviderCanvas: public Gui::ViewProviderImagePlane
{
    PROPERTY_HEADER_WITH_OVERRIDE(Gui::ViewProviderCanvas);

public:
    ViewProviderCanvas();
    ~ViewProviderCanvas() override;

    App::PropertyBool DisplayThrough;

    void attach(App::DocumentObject* pcObject) override;
    void updateData(const App::Property*) override;
    void onChanged(const App::Property* prop) override;
    void setupContextMenu(QMenu*, QObject*, const char*) override;
    bool doubleClicked() override;

    /// Open the canvas task panel, optionally starting the two point calibration
    void editCanvas(bool calibrate = false);

protected:
    void addRenderStateNodes(SoSeparator* root) override;

private:
    void updateFlips();

    SoPolygonOffset* polygonOffset;
    SoDepthBuffer* depthBuffer;
};

}  // namespace Gui
