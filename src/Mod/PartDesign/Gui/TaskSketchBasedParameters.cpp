// SPDX-License-Identifier: LGPL-2.1-or-later

/***************************************************************************
 *   Copyright (c) 2013 Jan Rheinländer                                    *
 *                                   <jrheinlaender@users.sourceforge.net> *
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


#include <algorithm>
#include <set>

#include <QHBoxLayout>
#include <QLabel>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QSignalBlocker>
#include <QTextStream>
#include <QTimer>
#include <QToolButton>

#include <TopExp_Explorer.hxx>
#include <TopoDS_Shape.hxx>

#include <App/Document.h>
#include <App/Origin.h>
#include <Base/Console.h>
#include <Gui/Application.h>
#include <Gui/CommandT.h>
#include <Gui/Document.h>
#include <Gui/Selection/Selection.h>
#include <Gui/ViewProvider.h>
#include <Mod/Part/App/DatumFeature.h>
#include <Mod/PartDesign/App/FeatureSketchBased.h>
#include <Mod/Sketcher/App/SketchObject.h>

#include "TaskSketchBasedParameters.h"
#include "ReferenceSelection.h"

using namespace PartDesignGui;
using namespace Gui;

/* TRANSLATOR PartDesignGui::TaskSketchBasedParameters */

namespace
{
/// A dialog can be built from several task boxes (a pipe has three), but only one profile row.
std::set<const App::DocumentObject*>& featuresWithProfileRow()
{
    static std::set<const App::DocumentObject*> features;
    return features;
}
}  // namespace

ProfileSelectionWidget::ProfileSelectionWidget(
    PartDesign::ProfileBased* profileFeature,
    QWidget* parent
)
    : QWidget(parent)
    , Gui::SelectionObserver(false)
    , feature(profileFeature)
{
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(new QLabel(tr("Profile"), this));

    summaryLabel = new QLabel(this);
    layout->addWidget(summaryLabel);

    layout->addStretch(1);

    pickButton = new QToolButton(this);
    pickButton->setText(tr("Select"));
    pickButton->setCheckable(true);
    pickButton->setToolTip(tr("Click inside a closed sketch region to add or remove it"));
    layout->addWidget(pickButton);

    connect(pickButton, &QToolButton::toggled, this, &ProfileSelectionWidget::setPickingActive);

    updateSummary();
}

ProfileSelectionWidget::~ProfileSelectionWidget()
{
    if (isSelectionAttached()) {
        detachSelection();
    }
}

int ProfileSelectionWidget::countRegions(App::DocumentObject* profile)
{
    auto* sketch = freecad_cast<Sketcher::SketchObject*>(profile);
    if (!sketch) {
        return 0;
    }

    const TopoDS_Shape& shape = sketch->InternalShape.getValue();
    if (shape.IsNull()) {
        return 0;
    }

    int count = 0;
    for (TopExp_Explorer explorer(shape, TopAbs_FACE); explorer.More(); explorer.Next()) {
        ++count;
    }

    return count;
}

PartDesign::ProfileBased* ProfileSelectionWidget::getFeature() const
{
    return feature.get<PartDesign::ProfileBased>();
}

Gui::ViewProvider* ProfileSelectionWidget::getProfileViewProvider() const
{
    PartDesign::ProfileBased* profileBased = getFeature();
    if (!profileBased) {
        return nullptr;
    }

    App::DocumentObject* profile = profileBased->Profile.getValue();
    if (!profile) {
        return nullptr;
    }

    return Gui::Application::Instance->getViewProvider(profile);
}

bool ProfileSelectionWidget::isPickingActive() const
{
    return isSelectionAttached();
}

void ProfileSelectionWidget::setPickingActive(bool active)
{
    if (active != isSelectionAttached()) {
        if (active) {
            // The feature hides its profile, but the regions must be visible to be picked.
            if (Gui::ViewProvider* profileView = getProfileViewProvider()) {
                profileWasVisible = profileView->isVisible();
                profileView->show();
            }

            Gui::Selection().clearSelection();
            attachSelection();
        }
        else {
            detachSelection();

            if (Gui::ViewProvider* profileView = getProfileViewProvider()) {
                if (!profileWasVisible) {
                    profileView->hide();
                }
            }
        }
    }

    QSignalBlocker blocker(pickButton);
    pickButton->setChecked(active);
}

void ProfileSelectionWidget::updateSummary()
{
    PartDesign::ProfileBased* profileBased = getFeature();
    if (!profileBased) {
        summaryLabel->setText(tr("No profile"));
        return;
    }

    const std::vector<std::string> subs = profileBased->Profile.getSubValues();
    const auto selected = std::count_if(subs.begin(), subs.end(), [](const std::string& sub) {
        return !sub.empty();
    });

    if (selected > 0) {
        summaryLabel->setText(tr("%1 selected").arg(static_cast<int>(selected)));
        return;
    }

    const int regions = countRegions(profileBased->Profile.getValue());
    if (regions > 0) {
        summaryLabel->setText(tr("%1 available").arg(regions));
    }
    else {
        summaryLabel->setText(tr("Nothing selected"));
    }
}

void ProfileSelectionWidget::onSelectionChanged(const Gui::SelectionChanges& msg)
{
    if (msg.Type != Gui::SelectionChanges::AddSelection) {
        return;
    }

    PartDesign::ProfileBased* profileBased = getFeature();
    if (!profileBased) {
        return;
    }

    App::DocumentObject* profile = profileBased->Profile.getValue();
    if (!profile || !msg.pObjectName || !msg.pDocName) {
        return;
    }

    if (std::string(profile->getNameInDocument()) != msg.pObjectName
        || std::string(profile->getDocument()->getName()) != msg.pDocName) {
        return;
    }

    std::string sub = msg.pSubName ? msg.pSubName : "";
    const std::size_t separator = sub.rfind('.');
    if (separator != std::string::npos) {
        sub = sub.substr(separator + 1);
    }

    // The element may or may not still carry the prefix depending on the observer resolve mode.
    const std::string& prefix = Sketcher::SketchObject::internalPrefix();
    if (sub.rfind(prefix, 0) == 0) {
        sub = sub.substr(prefix.size());
    }

    // The shape of a sketch has no faces of its own, so a face is always an internal region.
    if (sub.size() <= 4 || sub.rfind("Face", 0) != 0) {
        return;
    }

    toggleRegion(prefix + sub);

    // Clearing from inside the observer callback would re-enter the selection machinery.
    QTimer::singleShot(0, this, []() {
        Gui::Selection().clearSelection();
    });
}

void ProfileSelectionWidget::toggleRegion(const std::string& subName)
{
    PartDesign::ProfileBased* profileBased = getFeature();
    if (!profileBased) {
        return;
    }

    App::DocumentObject* profile = profileBased->Profile.getValue();
    if (!profile) {
        return;
    }

    std::vector<std::string> subs = profileBased->Profile.getSubValues();
    subs.erase(
        std::remove_if(subs.begin(), subs.end(), [](const std::string& sub) { return sub.empty(); }),
        subs.end()
    );

    auto it = std::find(subs.begin(), subs.end(), subName);
    if (it != subs.end()) {
        subs.erase(it);
    }
    else {
        subs.push_back(subName);
    }

    profileBased->Profile.setValue(profile, subs);

    updateSummary();
    Q_EMIT profileChanged();
}

TaskSketchBasedParameters::TaskSketchBasedParameters(
    PartDesignGui::ViewProvider* vp,
    QWidget* parent,
    const std::string& pixmapname,
    const QString& parname
)
    : TaskFeatureParameters(vp, parent, pixmapname, parname)
{
    // disable selection
    this->blockSelection(true);

    auto* sketchBased = getObject<PartDesign::ProfileBased>();
    if (!sketchBased || !featuresWithProfileRow().insert(sketchBased).second) {
        return;
    }

    profileRowOwner = sketchBased;
    profileWidget = new ProfileSelectionWidget(sketchBased, this);
    groupLayout()->insertWidget(0, profileWidget);
    connect(profileWidget, &ProfileSelectionWidget::profileChanged, this, [this]() {
        recomputeFeature();
    });

    const int regions = ProfileSelectionWidget::countRegions(sketchBased->Profile.getValue());
    if (regions == 0) {
        profileWidget->hide();
        return;
    }

    const std::vector<std::string> subs = sketchBased->Profile.getSubValues();
    const bool hasRegion = std::any_of(subs.begin(), subs.end(), [](const std::string& sub) {
        return !sub.empty();
    });

    if (regions > 1 && !hasRegion) {
        profileWidget->setPickingActive(true);
    }
}

const QString TaskSketchBasedParameters::onAddSelection(
    const Gui::SelectionChanges& msg,
    App::PropertyLinkSub& prop
)
{
    // Note: The validity checking has already been done in ReferenceSelection.cpp
    auto sketchBased = getObject<PartDesign::ProfileBased>();
    App::DocumentObject* selObj = sketchBased->getDocument()->getObject(msg.pObjectName);
    if (selObj == sketchBased) {
        return QString();
    }

    std::string subname = msg.pSubName;
    QString refStr;

    if (PartDesign::Feature::isDatum(selObj)) {
        // Check if it's a plane within a LCS
        auto datum = freecad_cast<App::DatumElement*>(selObj);
        if (datum && datum->getLCS()) {
            selObj = datum->getLCS();
            subname = datum->getNameInDocument();
            refStr = QString::fromStdString((std::string(selObj->getNameInDocument()) + ":" + subname));
        }
        else {
            // Remove subname for planes and datum features
            subname = "";
            refStr = QString::fromUtf8(selObj->getNameInDocument());
        }
    }
    else if (subname.size() > 4) {
        int faceId = std::atoi(&subname[4]);
        refStr = QString::fromUtf8(selObj->getNameInDocument()) + QStringLiteral(":")
            + QObject::tr("Face") + QString::number(faceId);
    }

    std::vector<std::string> upToFaces(1, subname);
    prop.setValue(selObj, upToFaces);
    recomputeFeature();

    return refStr;
}

void TaskSketchBasedParameters::startReferenceSelection(App::DocumentObject*, App::DocumentObject* base)
{
    auto* viewObj = getViewObject<ViewProvider>();
    if (!viewObj) {
        return;
    }

    const auto* bodyViewProvider = viewObj->getBodyViewProvider();
    if (bodyViewProvider) {
        previouslyVisibleViewProvider = bodyViewProvider->getShownViewProvider();
    }

    if (!base) {
        return;
    }

    if (Document* doc = getGuiDocument()) {
        if (previouslyVisibleViewProvider) {
            previouslyVisibleViewProvider->hide();
        }

        doc->setShow(base->getNameInDocument());
    }
}

void TaskSketchBasedParameters::finishReferenceSelection(App::DocumentObject*, App::DocumentObject* base)
{
    if (!previouslyVisibleViewProvider) {
        return;
    }

    if (Document* doc = getGuiDocument()) {
        if (base) {
            doc->setHide(base->getNameInDocument());
        }

        previouslyVisibleViewProvider->show();
        previouslyVisibleViewProvider = nullptr;
    }
}

void TaskSketchBasedParameters::onSelectReference(AllowSelectionFlags allow)
{
    // Note: Even if there is no solid, App::Plane and Part::Datum can still be selected
    if (auto sketchBased = getObject<PartDesign::ProfileBased>()) {
        // The solid this feature will be fused to
        App::DocumentObject* prevSolid = sketchBased->getBaseObject(/* silent =*/true);

        if (AllowSelectionFlags::Int(allow) != int(AllowSelection::NONE)) {
            startReferenceSelection(sketchBased, prevSolid);
            this->blockSelection(false);
            Gui::Selection().clearSelection();
            Gui::Selection().addSelectionGate(new ReferenceSelection(prevSolid, allow));
        }
        else {
            Gui::Selection().rmvSelectionGate();
            finishReferenceSelection(sketchBased, prevSolid);
            this->blockSelection(true);
        }
    }
}


void TaskSketchBasedParameters::exitSelectionMode()
{
    onSelectReference(AllowSelection::NONE);
}

QVariant TaskSketchBasedParameters::setUpToFace(const QString& text)
{
    if (text.isEmpty()) {
        return {};
    }

    QStringList parts = text.split(QChar::fromLatin1(':'));
    if (parts.length() < 2) {
        parts.push_back(QString());
    }

    // Check whether this is the name of an App::Plane or Part::Datum feature
    App::Document* doc = getAppDocument();
    if (!doc) {
        return {};
    }

    App::DocumentObject* obj = doc->getObject(parts[0].toLatin1());
    if (!obj) {
        return {};
    }

    if (obj->isDerivedFrom<App::Plane>()) {
        // everything is OK (we assume a Part can only have exactly 3 App::Plane objects
        // located at the base of the feature tree)
        return {};
    }

    if (obj->isDerivedFrom<Part::Datum>()) {
        // it's up to the document to check that the datum plane is in the same body
        return {};
    }

    // We must expect that "parts[1]" is the translation of "Face" followed by an ID.
    QString name;
    QTextStream str(&name);
    str << "^" << tr("Face") << "(\\d+)$";
    QRegularExpression rx(name);
    QRegularExpressionMatch match;
    if (parts[1].indexOf(rx, 0, &match) < 0) {
        return {};
    }

    int faceId = match.captured(1).toInt();
    std::stringstream ss;
    ss << "Face" << faceId;

    std::vector<std::string> upToFaces(1, ss.str());
    auto sketchBased = getObject<PartDesign::ProfileBased>();
    sketchBased->UpToFace.setValue(obj, upToFaces);
    recomputeFeature();

    return QByteArray(ss.str().c_str());
}

QVariant TaskSketchBasedParameters::objectNameByLabel(const QString& label, const QVariant& suggest) const
{
    // search for an object with the given label
    App::Document* doc = getAppDocument();
    if (!doc) {
        return {};
    }

    // for faster access try the suggestion
    if (suggest.isValid()) {
        App::DocumentObject* obj = doc->getObject(suggest.toByteArray());
        if (obj && QString::fromUtf8(obj->Label.getValue()) == label) {
            return QVariant(QByteArray(obj->getNameInDocument()));
        }
    }

    // go through all objects and check the labels
    std::string name = label.toUtf8().data();
    std::vector<App::DocumentObject*> objs = doc->getObjects();
    for (auto obj : objs) {
        if (name == obj->Label.getValue()) {
            return QVariant(QByteArray(obj->getNameInDocument()));
        }
    }

    return {};  // no such feature found
}

QString TaskSketchBasedParameters::getFaceReference(const QString& obj, const QString& sub) const
{
    App::Document* doc = getAppDocument();
    if (!doc) {
        return {};
    }

    QString o = obj.left(obj.indexOf(QStringLiteral(":")));
    if (o.isEmpty()) {
        return {};
    }

    return QString::fromUtf8(R"((App.getDocument("%1").%2, ["%3"]))")
        .arg(QString::fromUtf8(doc->getName()), o, sub);
}

QString TaskSketchBasedParameters::make2DLabel(
    const App::DocumentObject* section,
    const std::vector<std::string>& subValues
)
{
    if (section->isDerivedFrom<Part::Part2DObject>()) {
        return QString::fromUtf8(section->Label.getValue());
    }
    else if (subValues.empty()) {
        Base::Console().error("No valid subelement linked in %s\n", section->Label.getValue());
        return {};
    }
    else {
        return QString::fromStdString((std::string(section->getNameInDocument()) + ":" + subValues[0]));
    }
}

TaskSketchBasedParameters::~TaskSketchBasedParameters()
{
    if (profileWidget) {
        profileWidget->setPickingActive(false);
        featuresWithProfileRow().erase(profileRowOwner);
    }

    Gui::Selection().rmvSelectionGate();
}


//**************************************************************************
//**************************************************************************
// TaskDialog
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TaskDlgSketchBasedParameters::TaskDlgSketchBasedParameters(PartDesignGui::ViewProvider* vp)
    : TaskDlgFeatureParameters(vp)
{}

TaskDlgSketchBasedParameters::~TaskDlgSketchBasedParameters() = default;

//==== calls from the TaskView ===============================================================


bool TaskDlgSketchBasedParameters::accept()
{
    auto feature = getObject<PartDesign::ProfileBased>();

    // Make sure the feature is what we are expecting
    // Should be fine but you never know...
    if (!feature) {
        throw Base::TypeError("Bad object processed in the sketch based dialog.");
    }

    // First verify that the feature can be built and then hide the profile as otherwise
    // it will remain hidden if the feature's recompute fails
    if (TaskDlgFeatureParameters::accept()) {
        App::DocumentObject* sketch = feature->Profile.getValue();
        Gui::cmdAppObjectHide(sketch);
        return true;
    }

    return false;
}

bool TaskDlgSketchBasedParameters::reject()
{
    auto feature = getObject<PartDesign::ProfileBased>();

    // Make sure the feature is what we are expecting
    // Should be fine but you never know...
    if (!feature) {
        throw Base::TypeError("Bad object processed in the sketch based dialog.");
    }

    App::DocumentObjectWeakPtrT weakptr(feature);
    auto sketch = dynamic_cast<Sketcher::SketchObject*>(feature->Profile.getValue());

    bool value = TaskDlgFeatureParameters::reject();

    // if abort command deleted the object the sketch is visible again.
    // The previous one feature already should be made visible
    if (weakptr.expired()) {
        // Make the sketch visible
        if (sketch && Gui::Application::Instance->getViewProvider(sketch)) {
            Gui::Application::Instance->getViewProvider(sketch)->show();
        }
    }

    return value;
}

#include "moc_TaskSketchBasedParameters.cpp"
