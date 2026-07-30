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


#pragma once

#include <App/DocumentObserver.h>
#include <Gui/Selection/Selection.h>
#include "ViewProvider.h"

#include "TaskFeatureParameters.h"
#include "EnumFlags.h"

namespace App
{
class Property;
class PropertyLinkSubList;
}  // namespace App

namespace PartDesign
{
class ProfileBased;
}

class QLabel;
class QToolButton;

namespace PartDesignGui
{

/** Fusion style profile row.
 *
 * Closed regions bounded by the sketch curves are precomputed by
 * Sketcher::SketchObject as InternalFace1..N. This widget lets the user pick those
 * regions directly in the 3D view and writes them into the feature's Profile link.
 */
class ProfileSelectionWidget: public QWidget, public Gui::SelectionObserver
{
    Q_OBJECT

public:
    explicit ProfileSelectionWidget(PartDesign::ProfileBased* feature, QWidget* parent = nullptr);
    ~ProfileSelectionWidget() override;

    /// Number of closed regions the profile provides, 0 if it is not a sketch with regions.
    static int countRegions(App::DocumentObject* profile);

    bool isPickingActive() const;
    void updateSummary();

public Q_SLOTS:
    void setPickingActive(bool active);

Q_SIGNALS:
    void profileChanged();

private:
    void onSelectionChanged(const Gui::SelectionChanges& msg) override;
    void toggleRegion(const std::string& subName);
    PartDesign::ProfileBased* getFeature() const;
    Gui::ViewProvider* getProfileViewProvider() const;

    App::DocumentObjectWeakPtrT feature;
    QLabel* summaryLabel;
    QToolButton* pickButton;
    bool profileWasVisible {false};
};

/// Convenience class to collect common methods for all SketchBased features
class TaskSketchBasedParameters: public PartDesignGui::TaskFeatureParameters,
                                 public Gui::SelectionObserver
{
    Q_OBJECT

public:
    TaskSketchBasedParameters(
        PartDesignGui::ViewProvider* vp,
        QWidget* parent,
        const std::string& pixmapname,
        const QString& parname
    );
    ~TaskSketchBasedParameters() override;

protected:
    void onSelectionChanged(const Gui::SelectionChanges& msg) override = 0;
    const QString onAddSelection(const Gui::SelectionChanges& msg, App::PropertyLinkSub& prop);
    virtual void startReferenceSelection(App::DocumentObject* profile, App::DocumentObject* base);
    virtual void finishReferenceSelection(App::DocumentObject* profile, App::DocumentObject* base);
    /*!
     * \brief onSelectReference
     * Start reference selection mode to allow one to select objects of the type defined
     * with \a AllowSelectionFlags.
     * If AllowSelection::NONE is passed the selection mode is finished.
     */
    void onSelectReference(AllowSelectionFlags);
    void exitSelectionMode();
    QVariant setUpToFace(const QString& text);
    /// Try to find the name of a feature with the given label.
    /// For faster access a suggested name can be tested, first.
    QVariant objectNameByLabel(const QString& label, const QVariant& suggest) const;

    QString getFaceReference(const QString& obj, const QString& sub) const;
    /// Create a label for the 2D feature: the objects name if it's already 2D,
    /// or the subelement's name if the object is a solid.
    QString make2DLabel(const App::DocumentObject* section, const std::vector<std::string>& subValues);

    ProfileSelectionWidget* getProfileSelectionWidget() const
    {
        return profileWidget;
    }

private:
    /// Hide the feature preview while the profile is still the whole sketch, so an
    /// ambiguous sketch does not show every region extruded before one is picked.
    void setPreviewSuppressed(bool suppress);

    Gui::ViewProvider* previouslyVisibleViewProvider {nullptr};
    ProfileSelectionWidget* profileWidget {nullptr};
    const App::DocumentObject* profileRowOwner {nullptr};
    bool previewSuppressed {false};
};

class TaskDlgSketchBasedParameters: public PartDesignGui::TaskDlgFeatureParameters
{
    Q_OBJECT

public:
    explicit TaskDlgSketchBasedParameters(PartDesignGui::ViewProvider* vp);
    ~TaskDlgSketchBasedParameters() override;

public:
    /// is called by the framework if the dialog is accepted (Ok)
    bool accept() override;
    /// is called by the framework if the dialog is rejected (Cancel)
    bool reject() override;
};

}  // namespace PartDesignGui
