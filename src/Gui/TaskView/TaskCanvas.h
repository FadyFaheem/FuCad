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
 **************************************************************************/

#pragma once

#include <memory>
#include <QPointer>
#include <QWidget>

#include <App/Canvas.h>
#include <App/DocumentObserver.h>
#include <Gui/Selection/Selection.h>
#include <Gui/TaskView/TaskDialog.h>

namespace Gui
{

class InteractiveScale;
class View3DInventorViewer;
class ViewProviderCanvas;

class Ui_TaskCanvas;

/**
 * Task panel for a reference canvas, modelled on the Fusion 360 Canvas dialog.
 *
 * The canvas is created up front so every control previews live in the 3D view;
 * cancelling rolls the whole thing back through the document transaction.
 */
class TaskCanvas: public QWidget, public Gui::SelectionObserver
{
    Q_OBJECT
    Q_DISABLE_COPY(TaskCanvas)

public:
    explicit TaskCanvas(Image::Canvas* obj, bool creating, QWidget* parent = nullptr);
    ~TaskCanvas() override;

    void open();
    void accept();
    void reject();

    /// Enter the two point calibration mode
    void startCalibration();

    /// Whether the canvas has everything it needs to be kept
    bool isComplete() const;

protected:
    void onSelectionChanged(const Gui::SelectionChanges& msg) override;

private:
    void connectSignals();
    void loadFromFeature();

    void onSelectImage();
    void onSelectFace();
    /// Lay the canvas on the given face or plane, returns false if it defines no plane
    bool applyFaceReference(
        App::DocumentObject* obj,
        const char* subName,
        const Base::Vector3d& pickPoint,
        bool hasPickPoint
    );
    /// Take the face from whatever is already selected, used when the command starts
    void applyPreselectedFace();

    void changeOpacity(int value);
    void changeDisplayThrough(bool on);
    void changeSelectable(bool on);
    void changePlacementValues();
    void changeScaleX();
    void changeScaleY();
    void changeFlips();

    void acceptCalibration();
    void rejectCalibration();
    void enableApplyButton();
    void showToolHints() const;
    void toggleRotation();
    void toggleCentering();

    void applyCalibratedRotation();
    void applyCalibratedCentering(double factor);

    void updateScaleBoxes();
    void updateFaceLabel();
    void updateImageLabel();

    ViewProviderCanvas* getViewProvider() const;
    View3DInventorViewer* getViewer() const;
    /// Size of the image at its own resolution, the reference for the scale factors
    QSizeF naturalSize() const;

    std::unique_ptr<Ui_TaskCanvas> ui;
    QPointer<InteractiveScale> scale;
    App::WeakPtrT<Image::Canvas> feature;
    bool creating;
    bool pickingFace {false};
};

class TaskCanvasDialog: public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    explicit TaskCanvasDialog(Image::Canvas* obj, bool creating);

    void open() override;
    bool accept() override;
    bool reject() override;

    void startCalibration();

    QDialogButtonBox::StandardButtons getStandardButtons() const override
    {
        return QDialogButtonBox::Ok | QDialogButtonBox::Cancel;
    }

private:
    TaskCanvas* widget;
};

}  // namespace Gui
