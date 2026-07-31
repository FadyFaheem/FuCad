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

#include <cmath>
#include <QCheckBox>
#include <QFileInfo>
#include <QImageReader>
#include <QPushButton>
#include <QSignalBlocker>

#include <App/Application.h>
#include <App/Document.h>
#include <App/GeoFeature.h>
#include <Base/Console.h>
#include <Base/Tools.h>
#include <Gui/Application.h>
#include <Gui/BitmapFactory.h>
#include <Gui/Document.h>
#include <Gui/FileDialog.h>
#include <Gui/InputHint.h>
#include <Gui/MainWindow.h>
#include <Gui/View3DInventor.h>
#include <Gui/View3DInventorViewer.h>
#include <Gui/ViewProviderCanvas.h>
#include <Gui/TaskView/TaskImage.h>

#include "TaskCanvas.h"
#include "ui_TaskCanvas.h"


using namespace Gui;

/* TRANSLATOR Gui::TaskCanvas */

namespace
{
/// Fusion snaps a calibrated canvas to the nearest eighth turn
constexpr double rotationSnapDegrees = 45.0;

Gui::FileDialog::FilterList imageFilters()
{
    QStringList patterns;
    const auto formats = QImageReader::supportedImageFormats();
    patterns.reserve(formats.size());
    for (const QByteArray& format : formats) {
        patterns << QStringLiteral("*.%1").arg(QString::fromLatin1(format));
    }

    return {Gui::FileDialog::Filter(TaskCanvas::tr("Images"), patterns),
            Gui::FileDialog::Filter::AllFiles()};
}
}  // namespace


TaskCanvas::TaskCanvas(Image::Canvas* obj, bool creating, QWidget* parent)
    : QWidget(parent)
    , Gui::SelectionObserver(false)
    , ui(new Ui_TaskCanvas)
    , feature(obj)
    , creating(creating)
{
    ui->setupUi(this);
    ui->groupBoxCalibration->hide();

    if (creating) {
        applyPreselectedFace();
    }

    loadFromFeature();
    connectSignals();
}

TaskCanvas::~TaskCanvas()
{
    if (isSelectionAttached()) {
        detachSelection();
    }

    if (scale) {
        if (scale->isActive()) {
            scale->deactivate();
        }
        scale->deleteLater();
    }
}

void TaskCanvas::connectSignals()
{
    // clang-format off
    connect(ui->buttonSelectImage, &QPushButton::clicked,
        this, &TaskCanvas::onSelectImage);
    connect(ui->buttonSelectFace, &QPushButton::clicked,
        this, &TaskCanvas::onSelectFace);

    connect(ui->sliderOpacity, &QSlider::valueChanged,
        this, &TaskCanvas::changeOpacity);
    connect(ui->spinBoxOpacity, qOverload<int>(&QSpinBox::valueChanged),
        this, &TaskCanvas::changeOpacity);
    connect(ui->checkBoxDisplayThrough, &QCheckBox::toggled,
        this, &TaskCanvas::changeDisplayThrough);
    connect(ui->checkBoxSelectable, &QCheckBox::toggled,
        this, &TaskCanvas::changeSelectable);

    connect(ui->spinBoxXDistance, qOverload<double>(&QuantitySpinBox::valueChanged),
        this, &TaskCanvas::changePlacementValues);
    connect(ui->spinBoxYDistance, qOverload<double>(&QuantitySpinBox::valueChanged),
        this, &TaskCanvas::changePlacementValues);
    connect(ui->spinBoxZAngle, qOverload<double>(&QuantitySpinBox::valueChanged),
        this, &TaskCanvas::changePlacementValues);
    connect(ui->spinBoxOffset, qOverload<double>(&QuantitySpinBox::valueChanged),
        this, &TaskCanvas::changePlacementValues);

    connect(ui->spinBoxScaleX, qOverload<double>(&QDoubleSpinBox::valueChanged),
        this, &TaskCanvas::changeScaleX);
    connect(ui->spinBoxScaleY, qOverload<double>(&QDoubleSpinBox::valueChanged),
        this, &TaskCanvas::changeScaleY);

    connect(ui->checkBoxFlipHorizontal, &QCheckBox::toggled,
        this, &TaskCanvas::changeFlips);
    connect(ui->checkBoxFlipVertical, &QCheckBox::toggled,
        this, &TaskCanvas::changeFlips);

    connect(ui->pushButtonCalibrate, &QPushButton::clicked,
        this, &TaskCanvas::startCalibration);
    connect(ui->pushButtonApply, &QPushButton::clicked,
        this, &TaskCanvas::acceptCalibration);
    connect(ui->pushButtonCancel, &QPushButton::clicked,
        this, &TaskCanvas::rejectCalibration);
    // clang-format on
}

ViewProviderCanvas* TaskCanvas::getViewProvider() const
{
    if (feature.expired()) {
        return nullptr;
    }

    return freecad_cast<ViewProviderCanvas*>(
        Application::Instance->getViewProvider(feature.get())
    );
}

View3DInventorViewer* TaskCanvas::getViewer() const
{
    auto* vp = getViewProvider();
    if (!vp) {
        return nullptr;
    }

    auto* view = qobject_cast<View3DInventor*>(vp->getDocument()->getViewOfViewProvider(vp));
    return view ? view->getViewer() : nullptr;
}

QSizeF TaskCanvas::naturalSize() const
{
    auto* vp = getViewProvider();
    return vp ? vp->naturalSize() : QSizeF();
}

void TaskCanvas::loadFromFeature()
{
    if (feature.expired()) {
        return;
    }

    const QSignalBlocker blockX(ui->spinBoxXDistance);
    const QSignalBlocker blockY(ui->spinBoxYDistance);
    const QSignalBlocker blockA(ui->spinBoxZAngle);
    const QSignalBlocker blockO(ui->spinBoxOffset);
    const QSignalBlocker blockFH(ui->checkBoxFlipHorizontal);
    const QSignalBlocker blockFV(ui->checkBoxFlipVertical);
    const QSignalBlocker blockDT(ui->checkBoxDisplayThrough);
    const QSignalBlocker blockSel(ui->checkBoxSelectable);
    const QSignalBlocker blockSlider(ui->sliderOpacity);
    const QSignalBlocker blockSpin(ui->spinBoxOpacity);

    ui->spinBoxXDistance->setValue(feature->XDistance.getValue());
    ui->spinBoxYDistance->setValue(feature->YDistance.getValue());
    ui->spinBoxZAngle->setValue(feature->ZAngle.getValue());
    ui->spinBoxOffset->setValue(feature->Offset.getValue());
    ui->checkBoxFlipHorizontal->setChecked(feature->FlipHorizontal.getValue());
    ui->checkBoxFlipVertical->setChecked(feature->FlipVertical.getValue());

    if (auto* vp = getViewProvider()) {
        const int opacity = 100 - vp->Transparency.getValue();
        ui->sliderOpacity->setValue(opacity);
        ui->spinBoxOpacity->setValue(opacity);
        ui->checkBoxDisplayThrough->setChecked(vp->DisplayThrough.getValue());
        ui->checkBoxSelectable->setChecked(vp->Selectable.getValue());
    }

    updateImageLabel();
    updateFaceLabel();
    updateScaleBoxes();
}

void TaskCanvas::updateImageLabel()
{
    if (feature.expired()) {
        return;
    }

    const QString path = QString::fromUtf8(feature->ImageFile.getValue());
    ui->labelImageFile->setText(
        path.isEmpty() ? tr("No image selected") : QFileInfo(path).fileName()
    );
}

void TaskCanvas::updateFaceLabel()
{
    if (feature.expired()) {
        return;
    }

    const App::DocumentObject* support = feature->AttachmentSupport.getValue();
    if (!support) {
        ui->labelFaceRef->setText(tr("No face selected, using the XY plane"));
        return;
    }

    QString text = QString::fromUtf8(support->Label.getValue());
    const auto& subs = feature->AttachmentSupport.getSubValues();
    if (!subs.empty() && !subs.front().empty()) {
        text += QStringLiteral(" (%1)").arg(QString::fromUtf8(subs.front().c_str()));
    }

    ui->labelFaceRef->setText(text);
}

void TaskCanvas::updateScaleBoxes()
{
    const QSizeF natural = naturalSize();
    const bool usable = !feature.expired() && natural.width() > 0 && natural.height() > 0;

    ui->spinBoxScaleX->setEnabled(usable);
    ui->spinBoxScaleY->setEnabled(usable);
    if (!usable) {
        return;
    }

    const QSignalBlocker blockX(ui->spinBoxScaleX);
    const QSignalBlocker blockY(ui->spinBoxScaleY);
    ui->spinBoxScaleX->setValue(feature->XSize.getValue() / natural.width());
    ui->spinBoxScaleY->setValue(feature->YSize.getValue() / natural.height());
}

void TaskCanvas::onSelectImage()
{
    const QString path = FileDialog::getOpenFileName(
        this,
        tr("Select Image"),
        QString(),
        imageFilters()
    );

    if (path.isEmpty() || feature.expired()) {
        return;
    }

    feature->ImageFile.setValue(path.toUtf8().constData());
    if (feature->Label.getStrValue().empty() || creating) {
        feature->Label.setValue(QFileInfo(path).completeBaseName().toUtf8().constData());
    }

    updateImageLabel();
    updateScaleBoxes();
}

void TaskCanvas::onSelectFace()
{
    pickingFace = ui->buttonSelectFace->isChecked();

    if (pickingFace) {
        Gui::Selection().clearSelection();
        if (!isSelectionAttached()) {
            attachSelection();
        }
    }
    else if (isSelectionAttached()) {
        detachSelection();
    }
}

void TaskCanvas::onSelectionChanged(const Gui::SelectionChanges& msg)
{
    if (!pickingFace || msg.Type != SelectionChanges::AddSelection) {
        return;
    }

    App::Document* doc = App::GetApplication().getDocument(msg.pDocName);
    App::DocumentObject* obj = doc ? doc->getObject(msg.pObjectName) : nullptr;
    if (!obj) {
        return;
    }

    const bool applied = applyFaceReference(
        obj,
        msg.pSubName,
        Base::Vector3d(msg.x, msg.y, msg.z),
        msg.hasPickedPoint
    );
    if (!applied) {
        Base::Console().warning(
            std::string("Canvas"),
            "The selected geometry does not define a plane to lay the canvas on.\n"
        );
        return;
    }

    ui->buttonSelectFace->setChecked(false);
    onSelectFace();
    Gui::Selection().clearSelection();
}

void TaskCanvas::applyPreselectedFace()
{
    if (feature.expired() || feature->AttachmentSupport.getValue()) {
        return;
    }

    for (auto& selection : Gui::Selection().getSelectionEx()) {
        App::DocumentObject* obj = selection.getObject();
        if (!obj) {
            continue;
        }

        const auto subs = selection.getSubNames();
        const auto points = selection.getPickedPoints();
        const char* subName = subs.empty() ? nullptr : subs.front().c_str();
        const bool hasPick = !points.empty();
        const Base::Vector3d pick = hasPick ? points.front() : Base::Vector3d();

        if (applyFaceReference(obj, subName, pick, hasPick)) {
            return;
        }
    }
}

bool TaskCanvas::applyFaceReference(
    App::DocumentObject* obj,
    const char* subName,
    const Base::Vector3d& pickPoint,
    bool hasPickPoint
)
{
    if (feature.expired() || !obj || obj == feature.get()) {
        return false;
    }

    if (subName && !*subName) {
        subName = nullptr;
    }

    auto* geo = freecad_cast<App::GeoFeature*>(obj);
    Base::Vector3d dirZ;
    Base::Vector3d dirX;
    if (!geo || !geo->getCameraAlignmentDirection(dirZ, dirX, subName)) {
        return false;
    }

    const Base::Rotation rotation
        = Base::Rotation::makeRotationByAxes(dirX, Base::Vector3d(), dirZ, "ZXY");

    // Sit at the middle of the picked element, but keep it on the plane the
    // normal was measured from in case that element is curved
    Base::Vector3d origin;
    if (!geo->getElementCenter(origin, subName)) {
        origin = hasPickPoint ? pickPoint : geo->globalPlacement().getPosition();
    }
    else if (hasPickPoint) {
        origin -= dirZ * ((origin - pickPoint) * dirZ);
    }

    std::vector<std::string> subs;
    if (subName) {
        subs.emplace_back(subName);
    }
    feature->AttachmentSupport.setValue(obj, subs);
    feature->BasePlacement.setValue(Base::Placement(origin, rotation));

    if (scale) {
        scale->setPlacement(feature->globalPlacement());
    }

    updateFaceLabel();
    return true;
}

void TaskCanvas::changeOpacity(int value)
{
    const QSignalBlocker blockSlider(ui->sliderOpacity);
    const QSignalBlocker blockSpin(ui->spinBoxOpacity);
    ui->sliderOpacity->setValue(value);
    ui->spinBoxOpacity->setValue(value);

    if (auto* vp = getViewProvider()) {
        vp->Transparency.setValue(100 - value);
    }
}

void TaskCanvas::changeDisplayThrough(bool on)
{
    if (auto* vp = getViewProvider()) {
        vp->DisplayThrough.setValue(on);
    }
}

void TaskCanvas::changeSelectable(bool on)
{
    if (auto* vp = getViewProvider()) {
        vp->Selectable.setValue(on);
    }
}

void TaskCanvas::changePlacementValues()
{
    if (feature.expired()) {
        return;
    }

    feature->XDistance.setValue(ui->spinBoxXDistance->value().getValue());
    feature->YDistance.setValue(ui->spinBoxYDistance->value().getValue());
    feature->ZAngle.setValue(ui->spinBoxZAngle->value().getValue());
    feature->Offset.setValue(ui->spinBoxOffset->value().getValue());

    if (scale) {
        scale->setPlacement(feature->globalPlacement());
    }
}

void TaskCanvas::changeScaleX()
{
    if (ui->checkBoxScalePlanar->isChecked()) {
        const QSignalBlocker block(ui->spinBoxScaleY);
        ui->spinBoxScaleY->setValue(ui->spinBoxScaleX->value());
    }

    const QSizeF natural = naturalSize();
    if (!feature.expired() && natural.width() > 0) {
        feature->XSize.setValue(natural.width() * ui->spinBoxScaleX->value());
        feature->YSize.setValue(natural.height() * ui->spinBoxScaleY->value());
    }
}

void TaskCanvas::changeScaleY()
{
    if (ui->checkBoxScalePlanar->isChecked()) {
        const QSignalBlocker block(ui->spinBoxScaleX);
        ui->spinBoxScaleX->setValue(ui->spinBoxScaleY->value());
    }

    const QSizeF natural = naturalSize();
    if (!feature.expired() && natural.height() > 0) {
        feature->XSize.setValue(natural.width() * ui->spinBoxScaleX->value());
        feature->YSize.setValue(natural.height() * ui->spinBoxScaleY->value());
    }
}

void TaskCanvas::changeFlips()
{
    if (!feature.expired()) {
        feature->FlipHorizontal.setValue(ui->checkBoxFlipHorizontal->isChecked());
        feature->FlipVertical.setValue(ui->checkBoxFlipVertical->isChecked());
    }
}

void TaskCanvas::startCalibration()
{
    if (feature.expired()) {
        return;
    }

    if (!scale) {
        View3DInventorViewer* viewer = getViewer();
        if (!viewer) {
            return;
        }

        auto* vp = Application::Instance->getViewProvider(feature.get());
        scale = new InteractiveScale(viewer, vp, feature->globalPlacement());
        connect(scale, &InteractiveScale::scaleRequired, this, &TaskCanvas::acceptCalibration);
        connect(scale, &InteractiveScale::scaleCanceled, this, &TaskCanvas::rejectCalibration);
        connect(scale, &InteractiveScale::enableApplyBtn, this, &TaskCanvas::enableApplyButton);
        connect(scale, &InteractiveScale::showToolHints, this, &TaskCanvas::showToolHints);
        connect(scale, &InteractiveScale::toggleRotation, this, &TaskCanvas::toggleRotation);
        connect(scale, &InteractiveScale::toggleCentering, this, &TaskCanvas::toggleCentering);
    }

    scale->activate();
    ui->pushButtonCalibrate->hide();
    ui->groupBoxCalibration->show();
    ui->pushButtonApply->setEnabled(false);
    ui->spinBoxScaleX->setEnabled(false);
    ui->spinBoxScaleY->setEnabled(false);

    showToolHints();
}

void TaskCanvas::acceptCalibration()
{
    if (!scale || feature.expired()) {
        return;
    }

    const double factor = scale->getScaleFactor();

    if (ui->checkBoxOrient->isChecked()) {
        applyCalibratedRotation();
    }
    if (ui->checkBoxCenterMidpoint->isChecked()) {
        applyCalibratedCentering(factor);
    }

    feature->XSize.setValue(feature->XSize.getValue() * factor);
    feature->YSize.setValue(feature->YSize.getValue() * factor);
    updateScaleBoxes();

    rejectCalibration();
}

void TaskCanvas::applyCalibratedRotation()
{
    const double currentAngle = ui->spinBoxZAngle->value().getValue();
    const double lineAngle = scale->getAngleDegrees();
    const double displayAngle = currentAngle + lineAngle;

    const double snapped = rotationSnapDegrees * std::round(displayAngle / rotationSnapDegrees);
    ui->spinBoxZAngle->setValue(currentAngle + (snapped - displayAngle));
}

void TaskCanvas::applyCalibratedCentering(double factor)
{
    const SbVec3f midpoint = scale->getMidPoint() * static_cast<float>(factor);

    // The midpoint is measured along the canvas axes, which the Z angle has turned
    // away from the plane axes the distances are expressed in
    const double angle = Base::toRadians(ui->spinBoxZAngle->value().getValue());
    const double shiftX = -midpoint[0] * std::cos(angle) + midpoint[1] * std::sin(angle);
    const double shiftY = -midpoint[0] * std::sin(angle) - midpoint[1] * std::cos(angle);

    ui->spinBoxXDistance->setValue(ui->spinBoxXDistance->value().getValue() + shiftX);
    ui->spinBoxYDistance->setValue(ui->spinBoxYDistance->value().getValue() + shiftY);
}

void TaskCanvas::rejectCalibration()
{
    if (scale) {
        scale->deactivate();
    }

    ui->pushButtonCalibrate->show();
    ui->groupBoxCalibration->hide();
    ui->spinBoxScaleX->setEnabled(true);
    ui->spinBoxScaleY->setEnabled(true);

    Gui::getMainWindow()->hideHints();
}

void TaskCanvas::enableApplyButton()
{
    ui->pushButtonApply->setEnabled(true);
}

void TaskCanvas::toggleRotation()
{
    ui->checkBoxOrient->toggle();
}

void TaskCanvas::toggleCentering()
{
    ui->checkBoxCenterMidpoint->toggle();
}

void TaskCanvas::showToolHints() const
{
    if (!scale) {
        return;
    }

    using enum Gui::InputHint::UserInput;

    const InputHint snap = {tr("%1 snap angle"), {ModifierCtrl}};
    const InputHint toggleCenter = {tr("%1 toggle centering"), {KeyC}};
    const InputHint toggleRotate = {tr("%1 toggle rotating to line"), {KeyR}};
    const InputHint acceptDistance = {tr("%1 accept distance"), {KeyEnter}};

    std::list<Gui::InputHint> hints = Gui::lookupHints<InteractiveScaleState>(
        scale->getState(),
        {
            {.state = InteractiveScaleState::PickingFirst,
             .hints = {{tr("%1 pick first point"), {MouseLeft}}, toggleCenter, toggleRotate}},
            {.state = InteractiveScaleState::PickingSecond,
             .hints = {snap, {tr("%1 pick second point"), {MouseLeft}}, toggleCenter, toggleRotate}},
            {.state = InteractiveScaleState::Pending, .hints = {acceptDistance}},
        }
    );

    Gui::getMainWindow()->showHints(hints);
}

bool TaskCanvas::isComplete() const
{
    return !feature.expired() && !std::string(feature->ImageFile.getValue()).empty();
}

void TaskCanvas::open()
{
    // In creation mode the command already opened a transaction holding the new canvas
    if (!creating && !feature.expired()) {
        feature->getDocument()->openTransaction(QT_TRANSLATE_NOOP("Command", "Edit canvas"));
    }
}

void TaskCanvas::accept()
{
    if (scale && scale->isActive()) {
        rejectCalibration();
    }

    if (!feature.expired()) {
        App::Document* doc = feature->getDocument();
        doc->commitTransaction();
        doc->recompute();
    }
}

void TaskCanvas::reject()
{
    if (scale && scale->isActive()) {
        rejectCalibration();
    }

    if (feature.expired()) {
        return;
    }

    App::Document* doc = feature->getDocument();
    doc->abortTransaction();

    // Aborting a creation transaction takes the canvas with it
    if (!feature.expired()) {
        feature->purgeTouched();
    }
}

// ----------------------------------------------------------------------------

TaskCanvasDialog::TaskCanvasDialog(Image::Canvas* obj, bool creating)
    : widget {new TaskCanvas(obj, creating)}
{
    addTaskBox(Gui::BitmapFactory().pixmap("image-plane"), widget);

    associateToObject3dView(obj);
}

void TaskCanvasDialog::open()
{
    widget->open();
}

void TaskCanvasDialog::startCalibration()
{
    widget->startCalibration();
}

bool TaskCanvasDialog::accept()
{
    if (!widget->isComplete()) {
        Base::Console().warning(
            std::string("Canvas"),
            "Select an image before finishing the canvas.\n"
        );
        return false;
    }

    widget->accept();
    return true;
}

bool TaskCanvasDialog::reject()
{
    widget->reject();
    return true;
}

#include "moc_TaskCanvas.cpp"
