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


#include <cstddef>
#include <cstring>
#include <unordered_set>

#include <QAction>
#include <QCursor>
#include <QEvent>
#include <QFrame>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QLayoutItem>
#include <QLineEdit>
#include <QMenu>
#include <QMouseEvent>
#include <QScrollArea>
#include <QSizePolicy>
#include <QString>
#include <QStyle>
#include <QTimer>
#include <QToolButton>

#include <App/Application.h>
#include <App/Document.h>
#include <App/DocumentObject.h>
#include <App/Property.h>
#include <App/PropertyLinks.h>
#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/Tools.h>
#include <Base/Type.h>
#include <Gui/ActiveObjectList.h>
#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Gui/Document.h>
#include <Gui/MDIView.h>
#include <Gui/ViewProvider.h>
#include <Gui/ViewProviderDocumentObject.h>

#include "TimelineMarker.h"
#include "TimelineWidget.h"


using namespace Gui::Timeline;

namespace
{
constexpr int stripMargin = 4;
constexpr int markerSpacing = 2;
constexpr int playheadWidth = 5;
// Long enough that a slider drag or a batch of property changes collapses into
// a single update, short enough to feel immediate.
constexpr int updateDelay = 120;

const char* const bodyTypeName = "PartDesign::Body";
const char* const solidTypeName = "PartDesign::Feature";
const char* const partFeatureTypeName = "Part::Feature";
}  // namespace


TimelineWidget::TimelineWidget(QWidget* parent)
    : QWidget(parent)
    // Attached at the end of the constructor: the observer must not deliver a
    // selection change before the strip exists.
    , Gui::SelectionObserver(false)
{
    setupUi();

    updateTimer = new QTimer(this);
    updateTimer->setSingleShot(true);
    updateTimer->setInterval(updateDelay);
    connect(updateTimer, &QTimer::timeout, this, &TimelineWidget::onTimeout);

    connectDocumentSignals();
    attachSelection();
    scheduleRebuild();
}

TimelineWidget::~TimelineWidget() = default;

void TimelineWidget::setupUi()
{
    setObjectName(QStringLiteral("Timeline"));
    setAttribute(Qt::WA_StyledBackground, true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    auto* outerLayout = new QHBoxLayout(this);
    outerLayout->setContentsMargins(stripMargin, stripMargin, stripMargin, stripMargin);
    outerLayout->setSpacing(stripMargin);

    auto* controls = new QWidget(this);
    controls->setObjectName(QStringLiteral("TimelineControls"));
    controls->setAttribute(Qt::WA_StyledBackground, true);

    auto* controlLayout = new QHBoxLayout(controls);
    controlLayout->setContentsMargins(0, 0, 0, 0);
    controlLayout->setSpacing(1);

    stepBackButton = new QToolButton(controls);
    stepBackButton->setObjectName(QStringLiteral("TimelineStepButton"));
    stepBackButton->setIcon(style()->standardIcon(QStyle::SP_MediaSeekBackward));
    stepBackButton->setAutoRaise(true);
    stepBackButton->setFocusPolicy(Qt::NoFocus);
    stepBackButton->setToolTip(tr("Roll the model back one feature"));
    connect(stepBackButton, &QToolButton::clicked, this, &TimelineWidget::stepBack);

    stepForwardButton = new QToolButton(controls);
    stepForwardButton->setObjectName(QStringLiteral("TimelineStepButton"));
    stepForwardButton->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
    stepForwardButton->setAutoRaise(true);
    stepForwardButton->setFocusPolicy(Qt::NoFocus);
    stepForwardButton->setToolTip(tr("Roll the model forward one feature"));
    connect(stepForwardButton, &QToolButton::clicked, this, &TimelineWidget::stepForward);

    controlLayout->addWidget(stepBackButton);
    controlLayout->addWidget(stepForwardButton);

    auto* scrollArea = new QScrollArea(this);
    scrollArea->setObjectName(QStringLiteral("TimelineScroll"));
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    strip = new QWidget;
    strip->setObjectName(QStringLiteral("TimelineStrip"));
    strip->setAttribute(Qt::WA_StyledBackground, true);
    scrollArea->setWidget(strip);

    stripLayout = new QHBoxLayout(strip);
    stripLayout->setContentsMargins(0, 0, 0, 0);
    stripLayout->setSpacing(markerSpacing);

    playhead = new QWidget(strip);
    playhead->setObjectName(QStringLiteral("TimelinePlayhead"));
    playhead->setAttribute(Qt::WA_StyledBackground, true);
    playhead->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    playhead->setFixedWidth(playheadWidth);
    playhead->setCursor(Qt::SizeHorCursor);
    playhead->setToolTip(tr("Drag to roll the model history back and forward"));
    playhead->installEventFilter(this);
    playhead->hide();

    emptyLabel = new QLabel(tr("No feature history"), strip);
    emptyLabel->setObjectName(QStringLiteral("TimelineEmptyLabel"));
    emptyLabel->hide();

    outerLayout->addWidget(controls, 0, Qt::AlignVCenter);
    outerLayout->addWidget(scrollArea, 1);

    const int scrollBarExtent = style()->pixelMetric(QStyle::PM_ScrollBarExtent);
    setFixedHeight(TimelineMarker::tileSize().height() + 2 * stripMargin + scrollBarExtent);
}

void TimelineWidget::connectDocumentSignals()
{
    Gui::Application* app = Gui::Application::Instance;
    if (!app) {
        return;
    }

    connActiveDocument = app->signalActiveDocument.connect([this](const Gui::Document&) {
        scheduleRebuild();
    });
    connNewDocument = app->signalNewDocument.connect([this](const Gui::Document&, bool) {
        scheduleRebuild();
    });
    connDeleteDocument = app->signalDeleteDocument.connect([this](const Gui::Document& doc) {
        if (&doc == trackedDocument) {
            connActiveObject.disconnect();
            trackedDocument = nullptr;
        }
        scheduleRebuild();
    });
    connNewObject = app->signalNewObject.connect([this](const Gui::ViewProvider&) {
        scheduleRebuild();
    });
    connDeletedObject = app->signalDeletedObject.connect([this](const Gui::ViewProvider&) {
        scheduleRebuild();
    });
    connChangedObject = app->signalChangedObject.connect(
        [this](const Gui::ViewProvider&, const App::Property& prop) {
            const char* name = prop.getName();
            const bool structural = name != nullptr
                && (std::strcmp(name, "Group") == 0 || std::strcmp(name, "Tip") == 0
                    || std::strcmp(name, "BaseFeature") == 0);
            if (structural) {
                scheduleRebuild();
            }
            else {
                scheduleRefresh();
            }
        }
    );
    connRelabelObject = app->signalRelabelObject.connect([this](const Gui::ViewProvider&) {
        scheduleRefresh();
    });
    connActivateView = app->signalActivateView.connect([this](const Gui::MDIView*) {
        scheduleRebuild();
    });
    connCloseView = app->signalCloseView.connect([this](const Gui::MDIView*) {
        scheduleRebuild();
    });
}

void TimelineWidget::trackDocument(Gui::Document* doc)
{
    if (doc == trackedDocument) {
        return;
    }

    connActiveObject.disconnect();
    trackedDocument = doc;

    if (doc) {
        connActiveObject = doc->signalActivatedViewProvider.connect(
            [this](const Gui::ViewProviderDocumentObject*, const char*) { scheduleRebuild(); }
        );
    }
}

void TimelineWidget::scheduleRebuild()
{
    pendingRebuild = true;
    updateTimer->start();
}

void TimelineWidget::scheduleRefresh()
{
    updateTimer->start();
}

void TimelineWidget::onTimeout()
{
    if (pendingRebuild) {
        pendingRebuild = false;
        rebuild();
    }
    else {
        refreshMarkers();
    }
}

void TimelineWidget::clearStrip()
{
    while (QLayoutItem* item = stripLayout->takeAt(0)) {
        if (QWidget* widget = item->widget()) {
            widget->hide();
        }
        delete item;
    }
}

void TimelineWidget::rebuild()
{
    clearStrip();

    for (TimelineMarker* marker : markers) {
        marker->deleteLater();
    }
    markers.clear();
    featureNames.clear();
    solidIndices.clear();
    documentName.clear();
    bodyName.clear();
    tipIndex = -1;

    Gui::Application* app = Gui::Application::Instance;
    Gui::Document* guiDoc = app ? app->activeDocument() : nullptr;
    trackDocument(guiDoc);

    App::Document* appDoc = guiDoc ? guiDoc->getDocument() : nullptr;
    std::vector<App::DocumentObject*> features;
    App::DocumentObject* body = nullptr;

    if (appDoc) {
        documentName = appDoc->getName();
        body = activeBody(guiDoc);
        if (body) {
            bodyName = body->getNameInDocument();
            collectBodyFeatures(body, features);
        }
        else {
            collectDocumentFeatures(appDoc, features);
        }
    }

    App::DocumentObject* baseFeature = nullptr;
    App::DocumentObject* tip = nullptr;
    if (body) {
        auto* baseProp = freecad_cast<App::PropertyLink*>(body->getPropertyByName("BaseFeature"));
        if (baseProp) {
            baseFeature = baseProp->getValue();
        }
        auto* tipProp = freecad_cast<App::PropertyLink*>(body->getPropertyByName("Tip"));
        if (tipProp) {
            tip = tipProp->getValue();
        }
    }

    const Base::Type solidType = Base::Type::fromName(solidTypeName);

    for (std::size_t i = 0; i < features.size(); ++i) {
        App::DocumentObject* obj = features[i];
        auto* marker = new TimelineMarker(obj, strip);
        connect(marker, &TimelineMarker::selectRequested, this, &TimelineWidget::onMarkerSelect);
        connect(marker, &TimelineMarker::editRequested, this, &TimelineWidget::onMarkerEdit);
        // Queued: the context menu spins a nested event loop, which must not run
        // inside the event handler of the marker that opened it.
        connect(
            marker,
            &TimelineMarker::menuRequested,
            this,
            &TimelineWidget::onMarkerMenu,
            Qt::QueuedConnection
        );

        markers.append(marker);
        featureNames.emplace_back(obj->getNameInDocument());

        if (obj == baseFeature || (!solidType.isBad() && obj->isDerivedFrom(solidType))) {
            solidIndices.push_back(static_cast<int>(i));
        }
        if (obj == tip) {
            tipIndex = static_cast<int>(i);
        }
    }

    // Without a body there is no tip to roll to, so nothing is ever behind the
    // playhead and the playhead itself stays out of the strip.
    if (!body) {
        tipIndex = static_cast<int>(features.size()) - 1;
    }

    const int count = static_cast<int>(markers.size());
    for (int i = 0; i < count; ++i) {
        if (body && i == tipIndex + 1) {
            stripLayout->addWidget(playhead);
            playhead->show();
        }
        stripLayout->addWidget(markers[i]);
        markers[i]->show();
    }
    if (body && tipIndex + 1 >= count) {
        stripLayout->addWidget(playhead);
        playhead->show();
    }

    if (count == 0) {
        stripLayout->addWidget(emptyLabel);
        emptyLabel->show();
    }

    stripLayout->addStretch(1);

    applyStates();
}

void TimelineWidget::refreshMarkers()
{
    for (TimelineMarker* marker : markers) {
        marker->refresh();
    }

    applyStates();
}

void TimelineWidget::applyStates()
{
    const int count = static_cast<int>(markers.size());
    for (int i = 0; i < count; ++i) {
        TimelineMarker* marker = markers[i];
        App::DocumentObject* obj = marker->object();
        const bool selected = obj != nullptr && Gui::Selection().isSelected(obj);

        if (selected) {
            marker->setState(TimelineMarker::State::Selected);
        }
        else if (i > tipIndex) {
            marker->setState(TimelineMarker::State::RolledBack);
        }
        else {
            marker->setState(TimelineMarker::State::Normal);
        }

        marker->setTip(i == tipIndex);
    }

    const int position = tipSolidPosition();
    const bool hasBody = !bodyName.empty();
    stepBackButton->setEnabled(hasBody && position >= 0);
    stepForwardButton->setEnabled(hasBody && position + 1 < static_cast<int>(solidIndices.size()));
}

App::Document* TimelineWidget::currentDocument() const
{
    if (documentName.empty()) {
        return nullptr;
    }

    return App::GetApplication().getDocument(documentName.c_str());
}

App::DocumentObject* TimelineWidget::resolve(const std::string& name) const
{
    if (name.empty()) {
        return nullptr;
    }

    App::Document* doc = currentDocument();
    return doc ? doc->getObject(name.c_str()) : nullptr;
}

App::DocumentObject* TimelineWidget::currentBody() const
{
    App::DocumentObject* body = resolve(bodyName);
    if (!body) {
        return nullptr;
    }

    const Base::Type bodyType = Base::Type::fromName(bodyTypeName);
    if (bodyType.isBad() || !body->isDerivedFrom(bodyType)) {
        return nullptr;
    }

    return body;
}

App::DocumentObject* TimelineWidget::activeBody(Gui::Document* guiDoc) const
{
    if (!guiDoc) {
        return nullptr;
    }

    Gui::MDIView* view = guiDoc->getActiveView();
    if (!view) {
        return nullptr;
    }

    const Base::Type bodyType = Base::Type::fromName(bodyTypeName);
    if (bodyType.isBad()) {
        return nullptr;
    }

    App::DocumentObject* body = view->getActiveObject<App::DocumentObject*>(PDBODYKEY);
    if (!body || !body->isAttachedToDocument() || !body->isDerivedFrom(bodyType)) {
        return nullptr;
    }
    if (body->getDocument() != guiDoc->getDocument()) {
        return nullptr;
    }

    return body;
}

void TimelineWidget::collectBodyFeatures(
    App::DocumentObject* body,
    std::vector<App::DocumentObject*>& features
) const
{
    // The origin and its planes and axes are part of the body group but they
    // are not history, so they never reach the strip.
    std::unordered_set<const App::DocumentObject*> skipped;
    auto* originProp = freecad_cast<App::PropertyLink*>(body->getPropertyByName("Origin"));
    if (originProp) {
        App::DocumentObject* origin = originProp->getValue();
        if (origin) {
            skipped.insert(origin);
            auto* originGroup
                = freecad_cast<App::PropertyLinkList*>(origin->getPropertyByName("Group"));
            if (originGroup) {
                for (App::DocumentObject* child : originGroup->getValues()) {
                    if (child) {
                        skipped.insert(child);
                    }
                }
            }
        }
    }

    auto* baseProp = freecad_cast<App::PropertyLink*>(body->getPropertyByName("BaseFeature"));
    if (baseProp) {
        App::DocumentObject* base = baseProp->getValue();
        if (base && base->isAttachedToDocument()) {
            features.push_back(base);
        }
    }

    auto* groupProp = freecad_cast<App::PropertyLinkList*>(body->getPropertyByName("Group"));
    if (groupProp) {
        for (App::DocumentObject* child : groupProp->getValues()) {
            if (child && child->isAttachedToDocument() && skipped.count(child) == 0) {
                features.push_back(child);
            }
        }
    }
}

void TimelineWidget::collectDocumentFeatures(
    App::Document* doc,
    std::vector<App::DocumentObject*>& features
) const
{
    const Base::Type featureType = Base::Type::fromName(partFeatureTypeName);
    if (featureType.isBad()) {
        return;
    }

    // A dependency loop makes the sort give up; an unsorted list is still a
    // better panel than an empty one.
    std::vector<App::DocumentObject*> sorted;
    try {
        sorted = doc->topologicalSort();
    }
    catch (...) {
        sorted = doc->getObjects();
    }

    for (App::DocumentObject* obj : sorted) {
        if (obj && obj->isAttachedToDocument() && obj->isDerivedFrom(featureType)) {
            features.push_back(obj);
        }
    }
}

int TimelineWidget::markerIndexAt(const QPoint& stripPos) const
{
    int index = -1;
    const int count = static_cast<int>(markers.size());
    for (int i = 0; i < count; ++i) {
        if (markers[i]->geometry().center().x() <= stripPos.x()) {
            index = i;
        }
    }

    return index;
}

int TimelineWidget::indexOfFeature(const std::string& name) const
{
    const int count = static_cast<int>(featureNames.size());
    for (int i = 0; i < count; ++i) {
        if (featureNames[static_cast<std::size_t>(i)] == name) {
            return i;
        }
    }

    return -1;
}

int TimelineWidget::solidAtOrBefore(int index) const
{
    int result = -1;
    for (int solid : solidIndices) {
        if (solid > index) {
            break;
        }
        result = solid;
    }

    return result;
}

int TimelineWidget::tipSolidPosition() const
{
    int position = -1;
    const int count = static_cast<int>(solidIndices.size());
    for (int i = 0; i < count; ++i) {
        if (solidIndices[i] > tipIndex) {
            break;
        }
        position = i;
    }

    return position;
}

bool TimelineWidget::eventFilter(QObject* watched, QEvent* event)
{
    if (watched != playhead) {
        return QWidget::eventFilter(watched, event);
    }

    switch (event->type()) {
        case QEvent::MouseButtonPress:
            if (static_cast<QMouseEvent*>(event)->button() == Qt::LeftButton) {
                draggingPlayhead = true;
                return true;
            }
            return false;
        case QEvent::MouseMove:
            return draggingPlayhead;
        case QEvent::MouseButtonRelease: {
            if (!draggingPlayhead) {
                return false;
            }
            draggingPlayhead = false;
            rollTo(markerIndexAt(strip->mapFromGlobal(QCursor::pos())));
            return true;
        }
        default:
            return QWidget::eventFilter(watched, event);
    }
}

void TimelineWidget::onMarkerSelect(const QString& feature)
{
    App::DocumentObject* obj = resolve(feature.toStdString());
    if (!obj || !obj->isAttachedToDocument()) {
        return;
    }

    Gui::Selection().clearSelection();
    Gui::Selection().addSelection(obj->getDocument()->getName(), obj->getNameInDocument());
}

void TimelineWidget::onMarkerEdit(const QString& feature)
{
    editFeature(feature.toStdString());
}

void TimelineWidget::onMarkerMenu(const QString& feature, const QPoint& globalPos)
{
    const std::string name = feature.toStdString();
    if (!resolve(name)) {
        return;
    }

    const int index = indexOfFeature(name);

    QMenu menu(this);
    QAction* editAction = menu.addAction(tr("Edit"));
    QAction* renameAction = menu.addAction(tr("Rename"));
    QAction* deleteAction = menu.addAction(tr("Delete"));
    QAction* rollAction = nullptr;
    if (!bodyName.empty() && index >= 0) {
        menu.addSeparator();
        rollAction = menu.addAction(tr("Roll History Here"));
    }

    // The menu runs its own event loop, so the marker may be gone by the time it
    // closes. Everything below works off the name captured above.
    QAction* chosen = menu.exec(globalPos);
    if (!chosen) {
        return;
    }

    if (chosen == editAction) {
        editFeature(name);
    }
    else if (chosen == renameAction) {
        renameFeature(name);
    }
    else if (chosen == deleteAction) {
        deleteFeature(name);
    }
    else if (chosen == rollAction) {
        rollTo(index);
    }
}

void TimelineWidget::editFeature(const std::string& name)
{
    App::DocumentObject* obj = resolve(name);
    if (!obj) {
        return;
    }

    Gui::Application* app = Gui::Application::Instance;
    if (!app) {
        return;
    }

    Gui::Document* guiDoc = app->getDocument(obj->getDocument());
    if (!guiDoc) {
        return;
    }

    Gui::ViewProvider* provider = guiDoc->getViewProvider(obj);
    if (!provider) {
        return;
    }

    try {
        guiDoc->setEdit(provider);
    }
    catch (const Base::Exception& e) {
        e.reportException();
    }
}

void TimelineWidget::renameFeature(const std::string& name)
{
    App::DocumentObject* obj = resolve(name);
    if (!obj) {
        return;
    }

    const QString current = QString::fromUtf8(obj->Label.getValue());
    bool accepted = false;
    const QString label = QInputDialog::getText(
        this,
        tr("Rename"),
        tr("New name:"),
        QLineEdit::Normal,
        current,
        &accepted
    );
    if (!accepted || label.isEmpty() || label == current) {
        return;
    }

    obj = resolve(name);
    if (!obj) {
        return;
    }

    try {
        const int tid
            = Gui::Command::openActiveDocumentCommand(std::string("Rename timeline feature"));
        Gui::Command::doCommand(
            Gui::Command::Doc,
            "%s.Label = u'%s'",
            Gui::Command::getObjectCmd(obj).c_str(),
            Base::Tools::escapeEncodeString(label.toStdString()).c_str()
        );
        Gui::Command::commitCommand(tid);
    }
    catch (const Base::Exception& e) {
        e.reportException();
    }
}

void TimelineWidget::deleteFeature(const std::string& name)
{
    App::DocumentObject* obj = resolve(name);
    if (!obj || !obj->isAttachedToDocument()) {
        return;
    }

    Gui::Application* app = Gui::Application::Instance;
    if (!app) {
        return;
    }

    Gui::Selection().clearSelection();
    if (!Gui::Selection().addSelection(obj->getDocument()->getName(), obj->getNameInDocument())) {
        return;
    }

    app->commandManager().runCommandByName("Std_Delete");
}

void TimelineWidget::rollTo(int index)
{
    App::DocumentObject* body = currentBody();
    if (!body) {
        return;
    }

    // Only a solid feature can carry the tip, so a sketch or a datum rolls to
    // the solid feature it sits behind. Nothing before the first solid feature
    // means an empty body, which is the body itself as the target.
    App::DocumentObject* target = body;
    const int solid = solidAtOrBefore(index);
    if (solid >= 0 && solid < static_cast<int>(featureNames.size())) {
        App::DocumentObject* feature = resolve(featureNames[static_cast<std::size_t>(solid)]);
        if (feature) {
            target = feature;
        }
    }

    if (!target->isAttachedToDocument()) {
        return;
    }

    Gui::Application* app = Gui::Application::Instance;
    if (!app || !app->commandManager().getCommandByName("PartDesign_MoveTip")) {
        Base::Console().warning("Timeline: PartDesign_MoveTip is not available\n");
        return;
    }

    const char* docName = target->getDocument()->getName();
    Gui::Selection().clearSelection();
    if (!Gui::Selection().addSelection(docName, target->getNameInDocument())) {
        return;
    }

    app->commandManager().runCommandByName("PartDesign_MoveTip");
}

void TimelineWidget::stepBack()
{
    const int position = tipSolidPosition();
    if (position < 0) {
        return;
    }
    if (position == 0) {
        rollTo(-1);
        return;
    }

    rollTo(solidIndices[static_cast<std::size_t>(position - 1)]);
}

void TimelineWidget::stepForward()
{
    const int next = tipSolidPosition() + 1;
    if (next < 0 || next >= static_cast<int>(solidIndices.size())) {
        return;
    }

    rollTo(solidIndices[static_cast<std::size_t>(next)]);
}

void TimelineWidget::onSelectionChanged(const Gui::SelectionChanges& msg)
{
    Q_UNUSED(msg)

    applyStates();
}

#include "moc_TimelineWidget.cpp"
