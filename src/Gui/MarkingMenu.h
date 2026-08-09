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

#include <vector>

#include <QColor>
#include <QPointer>
#include <QRect>
#include <QString>
#include <QWidget>

#include <FCGlobal.h>

class QAction;
class QMenu;
class QPoint;

namespace Gui
{

/**
 * The radial menu Fusion opens on a right-click in the viewport: the entries sit
 * around the cursor instead of in a list below it, so reaching one is a flick in
 * a direction rather than a distance to travel.
 *
 * The entries are the ones of the context menu the workbenches and the view
 * providers built, which is what makes the ring follow the selection without
 * knowing anything about it. Whatever does not fit in the ring stays reachable
 * through a last entry that opens that same menu unchanged.
 *
 * The colours are properties rather than a stylesheet rule because the ring is
 * painted rather than composed of widgets; a theme sets them with qproperty-.
 * @author FuCad contributors
 */
class GuiExport MarkingMenu: public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QColor backdropColor READ backdropColor WRITE setBackdropColor)
    Q_PROPERTY(QColor outlineColor READ outlineColor WRITE setOutlineColor)
    Q_PROPERTY(QColor accentColor READ accentColor WRITE setAccentColor)

public:
    /// Whether a right-click in the 3D view opens the ring instead of a list.
    static bool isEnabled();

    /**
     * Opens the entries of \a source in a ring centred on \a where.
     *
     * Takes \a source over: it is destroyed with the ring unless the user asks
     * for the entries that did not fit, in which case it is shown as the plain
     * menu it is and left to close itself.
     */
    static void popUp(QMenu* source, const QPoint& where, QWidget* parent);

    QColor backdropColor() const
    {
        return backdrop;
    }
    void setBackdropColor(const QColor& color)
    {
        backdrop = color;
    }

    QColor outlineColor() const
    {
        return outline;
    }
    void setOutlineColor(const QColor& color)
    {
        outline = color;
    }

    QColor accentColor() const
    {
        return accent;
    }
    void setAccentColor(const QColor& color)
    {
        accent = color;
    }

protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    explicit MarkingMenu(QMenu* source, QWidget* parent);
    ~MarkingMenu() override;

    struct Sector
    {
        QAction* action {nullptr};
        QString label;
        QRect box;
        /// Opens the source menu rather than running an action of its own.
        bool overflow {false};
    };

    /// Fills the ring from the top level entries of the source menu.
    void collect();
    /// The sector under \a pos, or -1. Beyond the dead zone the nearest
    /// direction wins even when the cursor has not reached its box, so that a
    /// flick outwards is enough to choose.
    int sectorAt(const QPoint& pos) const;
    void activate(int index);
    /**
     * Closes the ring and shows \a menu at the cursor instead. Either the source
     * menu itself, for the entries that did not fit, or one of its submenus.
     */
    void handOff(QMenu* menu);

    QPointer<QMenu> source;
    /// What the menu is about, drawn in the middle of the ring.
    QString heading;
    std::vector<Sector> sectors;
    int hovered {-1};
    /// Cleared once the source menu has been handed back to its own lifetime.
    bool ownsSource {true};

    QColor backdrop;
    QColor outline;
    QColor accent;

    Q_DISABLE_COPY(MarkingMenu)
};

}  // namespace Gui
