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


#include <array>
#include <cmath>
#include <cstddef>

#include <QAction>
#include <QCursor>
#include <QFontMetrics>
#include <QIcon>
#include <QKeyEvent>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QPoint>
#include <QRect>
#include <QScreen>
#include <QSize>
#include <QTimer>

#include <App/Application.h>
#include <Base/Parameter.h>

#include "Action.h"
#include "MarkingMenu.h"


using namespace Gui;

namespace
{
// The ring is an ellipse rather than a circle because the entries are wide and
// short: the diagonals need vertical room to clear the row above and below them,
// and horizontal room to clear each other.
constexpr int ringRadiusX = 100;
constexpr int ringRadiusY = 124;
constexpr int itemWidth = 128;
constexpr int itemHeight = 28;
constexpr int iconExtent = 16;
constexpr int margin = 6;
/// Around the centre nothing is chosen, so that opening the ring and letting go
/// without moving cancels instead of running whatever happened to be nearest.
constexpr int deadZone = 26;
constexpr int cornerRadius = 4;

/// Compass order, so that the first and most used entries land on the four
/// directions a hand reaches most easily.
constexpr std::array<int, 8> slotAngles = {90, 0, 270, 180, 45, 315, 225, 135};

const char* const viewParameters = "User parameter:BaseApp/Preferences/View";

QPoint slotOffset(int slot)
{
    const double radians = slotAngles.at(slot) * M_PI / 180.0;
    return QPoint(
        static_cast<int>(std::lround(std::cos(radians) * ringRadiusX)),
        static_cast<int>(std::lround(-std::sin(radians) * ringRadiusY))
    );
}
}  // namespace


bool MarkingMenu::isEnabled()
{
    return App::GetApplication()
        .GetParameterGroupByPath(viewParameters)
        ->GetBool("UseMarkingMenu", true);
}

void MarkingMenu::popUp(QMenu* menu, const QPoint& where, QWidget* parent)
{
    if (!menu) {
        return;
    }

    auto* ring = new MarkingMenu(menu, parent);
    if (ring->sectors.empty()) {
        // Nothing to arrange, so the plain menu is still the better answer.
        ring->ownsSource = false;
        delete ring;
        menu->popup(where);
        return;
    }

    ring->ensurePolished();

    QPoint origin(where.x() - ring->width() / 2, where.y() - ring->height() / 2);
    if (const QScreen* screen = parent ? parent->screen() : nullptr) {
        const QRect available = screen->availableGeometry();
        origin.setX(qBound(available.left(), origin.x(), available.right() - ring->width()));
        origin.setY(qBound(available.top(), origin.y(), available.bottom() - ring->height()));
    }

    ring->move(origin);
    ring->show();
}

MarkingMenu::MarkingMenu(QMenu* menu, QWidget* parent)
    : QWidget(parent, Qt::Popup)
    , source(menu)
    , backdrop(30, 30, 30, 235)
    , outline(90, 90, 90)
    , accent(6, 150, 215)
{
    setObjectName(QStringLiteral("MarkingMenu"));
    setAttribute(Qt::WA_DeleteOnClose);
    setMouseTracking(true);
    setFixedSize(
        2 * (ringRadiusX + itemWidth / 2) + 2 * margin,
        2 * (ringRadiusY + itemHeight / 2) + 2 * margin
    );

    // The menu keeps the parent it was built with. Reparenting it would clear the
    // window flags that make it a popup, and it has to stay a popup for the case
    // where the user asks for the entries that did not fit in the ring.
    collect();
}

MarkingMenu::~MarkingMenu()
{
    if (ownsSource && source) {
        delete source;
    }
}

void MarkingMenu::collect()
{
    const QPoint centre(width() / 2, height() / 2);

    const QList<QAction*> actions = source->actions();
    QList<QAction*> usable;
    for (QAction* action : actions) {
        if (action->isSeparator() || !action->isVisible() || action->text().isEmpty()) {
            continue;
        }

        // The row that says what the menu is about belongs in the middle of the
        // ring, where the cursor already is, not in a direction to reach for.
        if (action->objectName() == QLatin1String("ContextMenuHeader")) {
            heading = action->text();
            continue;
        }

        usable.append(action);
    }

    if (usable.isEmpty()) {
        return;
    }

    const bool overflows = usable.size() > static_cast<int>(slotAngles.size());
    const int placed = overflows ? static_cast<int>(slotAngles.size()) - 1 : usable.size();

    sectors.resize(overflows ? slotAngles.size() : static_cast<std::size_t>(placed));

    for (std::size_t slot = 0; slot < sectors.size(); ++slot) {
        Sector& sector = sectors[slot];
        const QPoint offset = slotOffset(static_cast<int>(slot));
        sector.box = QRect(
            centre.x() + offset.x() - itemWidth / 2,
            centre.y() + offset.y() - itemHeight / 2,
            itemWidth,
            itemHeight
        );

        if (static_cast<int>(slot) < placed) {
            sector.action = usable.at(static_cast<int>(slot));
            sector.label = Action::cleanTitle(sector.action->text());
        }
        else {
            sector.overflow = true;
            sector.label = tr("More…");
        }
    }
}

int MarkingMenu::sectorAt(const QPoint& pos) const
{
    for (std::size_t i = 0; i < sectors.size(); ++i) {
        if (sectors[i].box.contains(pos)) {
            return static_cast<int>(i);
        }
    }

    const QPoint centre(width() / 2, height() / 2);
    const QPoint delta = pos - centre;
    const double distance = std::hypot(delta.x(), delta.y());
    if (distance < deadZone) {
        return -1;
    }

    // Measured on the circle the ellipse came from, so that the direction a
    // slot sits in is the direction the cursor has to point.
    const double pointing = std::atan2(
                                -delta.y() / static_cast<double>(ringRadiusY),
                                delta.x() / static_cast<double>(ringRadiusX)
                            )
        * 180.0 / M_PI;

    int nearest = -1;
    double smallest = 360.0;
    for (std::size_t i = 0; i < sectors.size(); ++i) {
        double difference = std::fabs(pointing - slotAngles.at(i));
        if (difference > 180.0) {
            difference = 360.0 - difference;
        }

        if (difference < smallest) {
            smallest = difference;
            nearest = static_cast<int>(i);
        }
    }

    return nearest;
}

void MarkingMenu::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const QPoint centre(width() / 2, height() / 2);
    const QColor text = palette().color(QPalette::Active, QPalette::WindowText);
    const QColor greyed = palette().color(QPalette::Disabled, QPalette::WindowText);

    // The middle of the ring, which is both what the menu is about and the place
    // where letting go cancels.
    if (heading.isEmpty()) {
        painter.setPen(QPen(outline, 1));
        painter.setBrush(backdrop);
        painter.drawEllipse(centre, 5, 5);
    }
    else {
        const QString label = painter.fontMetrics().elidedText(heading, Qt::ElideMiddle, itemWidth);
        QRect plate = painter.fontMetrics().boundingRect(label).adjusted(-8, -4, 8, 4);
        plate.moveCenter(centre);

        painter.setPen(QPen(outline, 1));
        painter.setBrush(backdrop);
        painter.drawRoundedRect(plate, cornerRadius, cornerRadius);

        painter.setPen(greyed);
        painter.drawText(plate, Qt::AlignCenter, label);
    }

    for (std::size_t i = 0; i < sectors.size(); ++i) {
        const Sector& sector = sectors[i];
        const bool enabled = sector.overflow || (sector.action && sector.action->isEnabled());
        const bool active = static_cast<int>(i) == hovered && enabled;

        painter.setPen(QPen(active ? accent : outline, 1));
        painter.setBrush(active ? accent.darker(160) : backdrop);
        painter.drawRoundedRect(sector.box, cornerRadius, cornerRadius);

        QRect content = sector.box.adjusted(6, 0, -6, 0);
        if (sector.action && !sector.action->icon().isNull()) {
            const QRect iconBox(
                content.left(),
                content.center().y() - iconExtent / 2 + 1,
                iconExtent,
                iconExtent
            );
            sector.action->icon().paint(
                &painter,
                iconBox,
                Qt::AlignCenter,
                enabled ? QIcon::Normal : QIcon::Disabled
            );
            content.setLeft(iconBox.right() + 6);
        }

        painter.setPen(enabled ? text : greyed);

        // The arrow a menu entry that leads somewhere carries, so that a sector
        // which opens a list rather than doing something says so.
        const bool leadsOn = sector.overflow || (sector.action && sector.action->menu());
        if (leadsOn) {
            const QPointF tip(content.right(), content.center().y() + 0.5);
            QPainterPath arrow;
            arrow.moveTo(tip.x() - 5, tip.y() - 4);
            arrow.lineTo(tip.x(), tip.y());
            arrow.lineTo(tip.x() - 5, tip.y() + 4);
            painter.setBrush(Qt::NoBrush);
            painter.strokePath(arrow, QPen(enabled ? text : greyed, 1.4));
            content.setRight(content.right() - 10);
        }

        painter.drawText(
            content,
            Qt::AlignVCenter | Qt::AlignLeft,
            painter.fontMetrics().elidedText(sector.label, Qt::ElideRight, content.width())
        );
    }
}

void MarkingMenu::mouseMoveEvent(QMouseEvent* event)
{
    const int under = sectorAt(event->pos());
    if (under != hovered) {
        hovered = under;
        update();
    }

    QWidget::mouseMoveEvent(event);
}

void MarkingMenu::mouseReleaseEvent(QMouseEvent* event)
{
    const int chosen = sectorAt(event->pos());
    if (chosen < 0) {
        close();
        return;
    }

    activate(chosen);
}

void MarkingMenu::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        close();
        event->accept();
        return;
    }

    QWidget::keyPressEvent(event);
}

void MarkingMenu::activate(int index)
{
    if (index < 0 || index >= static_cast<int>(sectors.size())) {
        return;
    }

    const Sector& sector = sectors[static_cast<std::size_t>(index)];
    if (sector.overflow) {
        handOff(source);
        return;
    }

    if (!sector.action || !sector.action->isEnabled()) {
        return;
    }

    // An entry that leads to a submenu has nothing of its own to run, so what
    // it leads to opens instead.
    if (QMenu* submenu = sector.action->menu()) {
        handOff(submenu);
        return;
    }

    QAction* action = sector.action;
    QMenu* menu = source;
    hide();

    // The ring holds the mouse and keyboard grab until the event loop comes back
    // to it, and a command that opens a dialog under that grab could not be used.
    // The menu outlives the ring so that the action it owns is still there when
    // the deferred call arrives, and goes once the command has run.
    ownsSource = false;
    QTimer::singleShot(0, menu, [menu, action]() {
        action->trigger();
        menu->deleteLater();
    });

    close();
}

void MarkingMenu::handOff(QMenu* menu)
{
    if (!menu || !source) {
        close();
        return;
    }

    QMenu* owner = source;
    const QPoint where = QCursor::pos();

    ownsSource = false;
    hide();

    QTimer::singleShot(0, owner, [owner, menu, where]() {
        if (menu != owner) {
            // A submenu belongs to the menu it came from, so that one has to
            // stay until the submenu is done with, and nothing else frees it
            // now that the ring has let go.
            QObject::connect(menu, &QMenu::aboutToHide, owner, [owner]() {
                owner->deleteLater();
            });
        }

        menu->popup(where);
    });

    close();
}

#include "moc_MarkingMenu.cpp"
