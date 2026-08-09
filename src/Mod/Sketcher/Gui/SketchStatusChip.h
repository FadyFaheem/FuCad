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


#pragma once

#include <QLabel>

#include <fastsignals/signal.h>

#include <Mod/Sketcher/SketcherGlobal.h>

namespace SketcherGui
{

class ViewProviderSketch;

/**
 * The badge Fusion shows while a sketch is open: how constrained it is, in a
 * colour that says at a glance whether there is anything left to do.
 *
 * It floats over the top of the canvas rather than living in the task panel,
 * because the answer matters while the eye is on the geometry. The wording comes
 * from the same solver report the task panel prints, so the two never disagree.
 * @author FuCad contributors
 */
class SketcherGuiExport SketchStatusChip: public QLabel
{
    Q_OBJECT

public:
    /// Whether a sketch being edited floats the badge over the canvas.
    static bool isEnabled();

    /**
     * Floats a badge over the view \a sketch is being edited in, and follows that
     * sketch until either of them goes away. Does nothing without a 3D view.
     */
    static void showFor(ViewProviderSketch* sketch);

    /// Takes the badge away again, which leaving the sketch has to do.
    static void dismiss();

    ~SketchStatusChip() override;

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    explicit SketchStatusChip(QWidget* view);

    void report(const QString& state, const QString& message, const QString& detail);
    void reposition();

    fastsignals::scoped_connection reporting;
};

}  // namespace SketcherGui
