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
 ***************************************************************************/

#include <QEvent>
#include <QPointer>
#include <QString>
#include <QStyle>

#include <App/Application.h>
#include <Base/Parameter.h>
#include <Gui/Application.h>
#include <Gui/Document.h>
#include <Gui/MDIView.h>
#include <Gui/View3DInventor.h>

#include "SketchStatusChip.h"
#include "ViewProviderSketch.h"

using namespace SketcherGui;

namespace
{
constexpr int topInset = 12;
const char* const sketcherParameters = "User parameter:BaseApp/Preferences/Mod/Sketcher";

/// The badge currently on screen, if any. Cleared by Qt when the view goes.
QPointer<SketchStatusChip> liveChip;

/**
 * The stylesheet class that colours a state. Kept as a property rather than a
 * colour so that a theme decides what "under-constrained" looks like.
 */
QString severityOf(const QString& state)
{
    if (state == QLatin1String("fully_constrained")) {
        return QStringLiteral("solved");
    }
    if (state == QLatin1String("under_constrained") || state == QLatin1String("empty")) {
        return QStringLiteral("open");
    }
    if (state == QLatin1String("partially_redundant_constraints")
        || state == QLatin1String("redundant_constraints")) {
        return QStringLiteral("redundant");
    }

    return QStringLiteral("broken");
}
}  // namespace


bool SketchStatusChip::isEnabled()
{
    return App::GetApplication()
        .GetParameterGroupByPath(sketcherParameters)
        ->GetBool("ShowStatusChip", true);
}

void SketchStatusChip::showFor(ViewProviderSketch* sketch)
{
    // Only one sketch is ever open, so there is only ever one badge, and holding
    // it here is what lets leaving the sketch take it away again.
    dismiss();

    if (!sketch || !isEnabled()) {
        return;
    }

    Gui::Document* document = sketch->getDocument();
    auto* view = document ? freecad_cast<Gui::View3DInventor*>(document->getActiveView()) : nullptr;
    if (!view) {
        return;
    }

    auto* chip = new SketchStatusChip(view);
    liveChip = chip;

    // The badge is a child of the view, so leaving the sketch has to take it
    // away: the view outlives the editing session.
    chip->reporting = sketch->signalSetUp.connect(
        [chip](const QString& state, const QString& message, const QString&, const QString& detail) {
            chip->report(state, message, detail);
        }
    );

    chip->show();
    chip->raise();
}

SketchStatusChip::SketchStatusChip(QWidget* view)
    : QLabel(view)
{
    setObjectName(QStringLiteral("SketchStatusChip"));
    setAttribute(Qt::WA_StyledBackground, true);
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    setProperty("severity", QStringLiteral("open"));
    setText(tr("Sketch"));

    view->installEventFilter(this);
    adjustSize();
    reposition();
}

SketchStatusChip::~SketchStatusChip() = default;

void SketchStatusChip::dismiss()
{
    delete liveChip.data();
    liveChip = nullptr;
}

void SketchStatusChip::report(const QString& state, const QString& message, const QString& detail)
{
    // The solver report is two halves, a heading and the count that belongs to
    // it, which the task panel joins with a link between them.
    QString text = message.trimmed();
    if (!detail.isEmpty()) {
        text += QLatin1Char(' ') + detail;
    }

    setText(text);
    setProperty("severity", severityOf(state));

    // A property a stylesheet selects on only takes effect once the style is
    // asked for the widget again.
    style()->unpolish(this);
    style()->polish(this);

    adjustSize();
    reposition();
}

void SketchStatusChip::reposition()
{
    QWidget* view = parentWidget();
    if (!view) {
        return;
    }

    move((view->width() - width()) / 2, topInset);
}

bool SketchStatusChip::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == parentWidget()
        && (event->type() == QEvent::Resize || event->type() == QEvent::Show)) {
        reposition();
        raise();
    }

    return QLabel::eventFilter(watched, event);
}

#include "moc_SketchStatusChip.cpp"
