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

#include <QIcon>
#include <QPoint>
#include <QSize>
#include <QString>
#include <QWidget>

#include <FCGlobal.h>

class QContextMenuEvent;
class QMouseEvent;
class QPaintEvent;

namespace App
{
class DocumentObject;
}

namespace Gui
{
namespace Timeline
{

/**
 * One entry of the timeline strip: the icon of a modelling feature above a
 * short label, drawn as a fixed size tile.
 *
 * A marker never owns nor keeps a pointer to the feature it stands for. It
 * remembers the document and the internal name and resolves the object on
 * demand, so that a marker which outlives its feature reports a null object
 * instead of handing out a dangling pointer.
 * @author FuCad contributors
 */
class GuiExport TimelineMarker: public QWidget
{
    Q_OBJECT

public:
    enum class State
    {
        Normal,
        Selected,
        /// Behind the playhead, that is after the tip of the body.
        RolledBack
    };

    explicit TimelineMarker(App::DocumentObject* obj, QWidget* parent = nullptr);
    ~TimelineMarker() override = default;

    /// The fixed tile size every marker is drawn at.
    static QSize tileSize();

    /// The feature this marker stands for, or nullptr once it is gone.
    App::DocumentObject* object() const;

    void setState(State state);
    /// Whether the feature is the current tip of its body.
    void setTip(bool value);

    /// Re-reads icon, label and error state from the document object.
    void refresh();

Q_SIGNALS:
    /// All three carry the internal name of the feature rather than the marker,
    /// so that a handler which outlives the marker has nothing to dangle on.
    void selectRequested(const QString& feature);
    void editRequested(const QString& feature);
    void menuRequested(const QString& feature, const QPoint& globalPos);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

private:
    void restyle();

    std::string documentName;
    std::string internalName;
    QIcon featureIcon;
    QString caption;
    State markerState {State::Normal};
    bool tip {false};
    bool error {false};

    Q_DISABLE_COPY(TimelineMarker)
};

}  // namespace Timeline
}  // namespace Gui
