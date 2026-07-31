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

#include <QAction>
#include <QMenu>
#include <Inventor/nodes/SoDepthBuffer.h>
#include <Inventor/nodes/SoPolygonOffset.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTextureCoordinate2.h>

#include <App/Canvas.h>
#include <Gui/ActionFunction.h>
#include <Gui/BitmapFactory.h>
#include <Gui/Control.h>
#include <Gui/Document.h>
#include <Gui/TaskView/TaskCanvas.h>

#include "ViewProviderCanvas.h"


using namespace Gui;

PROPERTY_SOURCE(Gui::ViewProviderCanvas, Gui::ViewProviderImagePlane)

namespace
{
// Enough to clear the face the canvas lies on without visibly floating above it
constexpr float offsetFactor = -4.0F;
constexpr float offsetUnits = -4.0F;
}  // namespace


ViewProviderCanvas::ViewProviderCanvas()
{
    ADD_PROPERTY_TYPE(
        DisplayThrough,
        (false),
        "Object Style",
        App::Prop_None,
        "Show the canvas through the model instead of letting geometry hide it"
    );

    polygonOffset = new SoPolygonOffset;
    polygonOffset->styles = SoPolygonOffset::FILLED;
    polygonOffset->factor = offsetFactor;
    polygonOffset->units = offsetUnits;
    polygonOffset->ref();

    depthBuffer = new SoDepthBuffer;
    depthBuffer->test = TRUE;
    depthBuffer->write = TRUE;
    depthBuffer->ref();

    sPixmap = "image-plane";
}

ViewProviderCanvas::~ViewProviderCanvas()
{
    polygonOffset->unref();
    depthBuffer->unref();
}

void ViewProviderCanvas::addRenderStateNodes(SoSeparator* root)
{
    root->addChild(depthBuffer);
    root->addChild(polygonOffset);
}

void ViewProviderCanvas::attach(App::DocumentObject* pcObj)
{
    ViewProviderImagePlane::attach(pcObj);
    updateFlips();
}

void ViewProviderCanvas::updateFlips()
{
    auto* canvas = freecad_cast<Image::Canvas*>(getObject());
    if (!canvas) {
        return;
    }

    const float uLeft = canvas->FlipHorizontal.getValue() ? 1.0F : 0.0F;
    const float uRight = canvas->FlipHorizontal.getValue() ? 0.0F : 1.0F;
    const float vBottom = canvas->FlipVertical.getValue() ? 1.0F : 0.0F;
    const float vTop = canvas->FlipVertical.getValue() ? 0.0F : 1.0F;

    textureCoords->point.set1Value(0, uLeft, vBottom);
    textureCoords->point.set1Value(1, uRight, vBottom);
    textureCoords->point.set1Value(2, uRight, vTop);
    textureCoords->point.set1Value(3, uLeft, vTop);
}

void ViewProviderCanvas::updateData(const App::Property* prop)
{
    auto* canvas = freecad_cast<Image::Canvas*>(getObject());
    if (canvas && (prop == &canvas->FlipHorizontal || prop == &canvas->FlipVertical)) {
        updateFlips();
        return;
    }

    ViewProviderImagePlane::updateData(prop);
}

void ViewProviderCanvas::onChanged(const App::Property* prop)
{
    if (prop == &DisplayThrough) {
        // Skipping the depth write as well keeps the canvas from masking the
        // geometry it is meant to be seen through
        const SbBool depth = DisplayThrough.getValue() ? FALSE : TRUE;
        depthBuffer->test = depth;
        depthBuffer->write = depth;
    }

    ViewProviderImagePlane::onChanged(prop);
}

void ViewProviderCanvas::setupContextMenu(QMenu* menu, QObject* receiver, const char* member)
{
    auto* func = new Gui::ActionFunction(menu);

    QAction* edit = menu->addAction(QObject::tr("Edit Canvas"));
    edit->setIcon(Gui::BitmapFactory().iconFromTheme("image-plane"));
    func->trigger(edit, [this]() { this->editCanvas(false); });

    QAction* calibrate = menu->addAction(QObject::tr("Calibrate"));
    calibrate->setIcon(Gui::BitmapFactory().iconFromTheme("image-scaling"));
    func->trigger(calibrate, [this]() { this->editCanvas(true); });

    // Skip ViewProviderImagePlane, its "Edit Image Plane" entry duplicates this one
    ViewProviderGeometryObject::setupContextMenu(menu, receiver, member);
}

bool ViewProviderCanvas::doubleClicked()
{
    editCanvas(false);
    return true;
}

void ViewProviderCanvas::editCanvas(bool calibrate)
{
    auto* canvas = getObject<Image::Canvas>();
    if (!canvas) {
        return;
    }

    App::Document* doc = getDocument()->getDocument();
    auto* dialog = new TaskCanvasDialog(canvas, false);
    Gui::Control().showDialog(dialog, doc);

    // showDialog drops the dialog when another one is already up
    if (calibrate && Gui::Control().activeDialog(doc) == dialog) {
        dialog->startCalibration();
    }
}
