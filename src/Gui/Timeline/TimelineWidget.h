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


#pragma once

#include <string>
#include <vector>

#include <QList>
#include <QPoint>
#include <QString>
#include <QWidget>

#include <fastsignals/signal.h>

#include <FCGlobal.h>
#include <Gui/Selection/Selection.h>

class QEvent;
class QHBoxLayout;
class QLabel;
class QTimer;
class QToolButton;

namespace App
{
class Document;
class DocumentObject;
}  // namespace App

namespace Gui
{
class Document;

namespace Timeline
{

class TimelineMarker;

/**
 * Fusion style history strip: one marker per modelling feature of the active
 * body, in creation order, with a playhead that rolls the model back by moving
 * the tip of the body.
 *
 * The strip is filled from the active PartDesign body of the active document.
 * PartDesign is not linked from Gui, so the body is recognised by type name and
 * read through its Group, Tip and BaseFeature properties, and its tip is moved
 * by running the PartDesign_MoveTip command by name; that keeps the timeline
 * out of the module dependency graph and keeps undo working. Outside a body the
 * strip falls back to the Part features of the document in dependency order,
 * which has no tip and therefore no playhead.
 *
 * Nothing here holds a document object across event loop turns: features are
 * remembered by internal name and resolved again on use, and every rebuild is
 * deferred through a timer so that it never runs inside a document signal.
 * @author FuCad contributors
 */
class GuiExport TimelineWidget: public QWidget, public Gui::SelectionObserver
{
    Q_OBJECT

public:
    explicit TimelineWidget(QWidget* parent = nullptr);
    ~TimelineWidget() override;

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
    /// Observer message from the Selection
    void onSelectionChanged(const Gui::SelectionChanges& msg) override;

private:
    void setupUi();
    void connectDocumentSignals();
    void trackDocument(Gui::Document* doc);

    void scheduleRebuild();
    void scheduleRefresh();
    void onTimeout();

    void rebuild();
    void clearStrip();
    void refreshMarkers();
    void applyStates();

    App::Document* currentDocument() const;
    App::DocumentObject* currentBody() const;
    App::DocumentObject* resolve(const std::string& name) const;
    App::DocumentObject* activeBody(Gui::Document* guiDoc) const;

    void collectBodyFeatures(
        App::DocumentObject* body,
        std::vector<App::DocumentObject*>& features
    ) const;
    void collectDocumentFeatures(
        App::Document* doc,
        std::vector<App::DocumentObject*>& features
    ) const;

    int markerIndexAt(const QPoint& stripPos) const;
    int indexOfFeature(const std::string& name) const;
    int solidAtOrBefore(int index) const;
    /// The solid feature before \a index, or -1 for the state with nothing applied.
    int previousSolid(int index) const;
    /// The solid feature after \a index, or \a index when there is none.
    int nextSolid(int index) const;

    void onMarkerSelect(const QString& feature);
    void onMarkerEdit(const QString& feature);
    void onMarkerMenu(const QString& feature, const QPoint& globalPos);

    void editFeature(const std::string& name);
    void renameFeature(const std::string& name);
    void deleteFeature(const std::string& name);

    void rollTo(int index);
    void stepBack();
    void stepForward();

    QWidget* strip {nullptr};
    QHBoxLayout* stripLayout {nullptr};
    QWidget* playhead {nullptr};
    QLabel* emptyLabel {nullptr};
    QToolButton* stepBackButton {nullptr};
    QToolButton* stepForwardButton {nullptr};
    QTimer* updateTimer {nullptr};

    QList<TimelineMarker*> markers;
    std::vector<std::string> featureNames;
    /// Indices into featureNames of the features that may become the tip.
    std::vector<int> solidIndices;
    std::string documentName;
    std::string bodyName;
    int tipIndex {-1};
    /// Where the playhead sits, which the arrows move one marker at a time. Only a
    /// solid feature can carry the tip, so stepping onto a sketch or datum leaves the
    /// tip on the solid behind it while the playhead still advances; without this the
    /// arrows could only jump between solids.
    int playheadIndex {-1};
    /// Survives the rebuild that moving the tip triggers, so a step is not snapped
    /// back onto the tip.
    int requestedPlayhead {-1};

    Gui::Document* trackedDocument {nullptr};
    bool pendingRebuild {false};
    bool draggingPlayhead {false};

    fastsignals::scoped_connection connActiveDocument;
    fastsignals::scoped_connection connNewDocument;
    fastsignals::scoped_connection connDeleteDocument;
    fastsignals::scoped_connection connNewObject;
    fastsignals::scoped_connection connDeletedObject;
    fastsignals::scoped_connection connChangedObject;
    fastsignals::scoped_connection connRelabelObject;
    fastsignals::scoped_connection connActivateView;
    fastsignals::scoped_connection connCloseView;
    /// Reconnected on every document switch, so only one document is observed.
    fastsignals::scoped_connection connActiveObject;

    Q_DISABLE_COPY(TimelineWidget)
};

}  // namespace Timeline
}  // namespace Gui
