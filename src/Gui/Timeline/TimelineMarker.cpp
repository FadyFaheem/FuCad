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


#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>
#include <QPalette>
#include <QRect>
#include <QSize>
#include <QStyle>
#include <QStyleOption>

#include <App/Application.h>
#include <App/Document.h>
#include <App/DocumentObject.h>
#include <Gui/Application.h>
#include <Gui/BitmapFactory.h>
#include <Gui/ViewProvider.h>

#include "TimelineMarker.h"


using namespace Gui::Timeline;

namespace
{
// Fusion's timeline is a thin strip of bare icons; the feature name lives in the
// tooltip rather than under the icon, which is what keeps the bar so short.
constexpr int markerWidth = 26;
constexpr int markerHeight = 26;
constexpr int markerPadding = 2;
constexpr int iconExtent = 20;
constexpr int badgeExtent = 8;

QString stateName(TimelineMarker::State state)
{
    switch (state) {
        case TimelineMarker::State::Selected:
            return QStringLiteral("selected");
        case TimelineMarker::State::RolledBack:
            return QStringLiteral("rolledBack");
        case TimelineMarker::State::Normal:
        default:
            return QStringLiteral("normal");
    }
}

const QPixmap& errorBadge()
{
    static const QPixmap badge = Gui::BitmapFactory().pixmapFromSvg(
        "overlay_error",
        QSizeF(badgeExtent, badgeExtent)
    );
    return badge;
}
}  // namespace


QSize TimelineMarker::tileSize()
{
    return {markerWidth, markerHeight};
}

TimelineMarker::TimelineMarker(App::DocumentObject* obj, QWidget* parent)
    : QWidget(parent)
{
    setObjectName(QStringLiteral("TimelineMarker"));
    setAttribute(Qt::WA_StyledBackground, true);
    setFixedSize(tileSize());
    setFocusPolicy(Qt::NoFocus);

    if (obj && obj->isAttachedToDocument()) {
        documentName = obj->getDocument()->getName();
        internalName = obj->getNameInDocument();
    }

    refresh();
}

App::DocumentObject* TimelineMarker::object() const
{
    if (documentName.empty() || internalName.empty()) {
        return nullptr;
    }

    App::Document* doc = App::GetApplication().getDocument(documentName.c_str());
    if (!doc) {
        return nullptr;
    }

    return doc->getObject(internalName.c_str());
}

void TimelineMarker::setState(State state)
{
    if (markerState == state) {
        return;
    }

    markerState = state;
    restyle();
}

void TimelineMarker::setTip(bool value)
{
    if (tip == value) {
        return;
    }

    tip = value;
    restyle();
}

void TimelineMarker::refresh()
{
    App::DocumentObject* obj = object();
    if (obj) {
        caption = QString::fromUtf8(obj->Label.getValue());
        error = obj->isError();

        Gui::ViewProvider* provider = Gui::Application::Instance
            ? Gui::Application::Instance->getViewProvider(obj)
            : nullptr;
        if (provider) {
            featureIcon = provider->getIcon();
        }

        const QString status = QString::fromUtf8(obj->getStatusString());
        setToolTip(status.isEmpty() ? caption : QStringLiteral("%1\n%2").arg(caption, status));
    }

    restyle();
}

void TimelineMarker::restyle()
{
    setProperty("timelineState", stateName(markerState));
    setProperty("timelineTip", tip);
    setProperty("timelineError", error);

    style()->unpolish(this);
    style()->polish(this);
    update();
}

void TimelineMarker::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)

    QStyleOption opt;
    opt.initFrom(this);

    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    const QRect content
        = rect().adjusted(markerPadding, markerPadding, -markerPadding, -markerPadding);

    QRect iconRect(0, 0, iconExtent, iconExtent);
    iconRect.moveCenter(content.center());

    const QIcon::Mode mode = markerState == State::RolledBack ? QIcon::Disabled : QIcon::Normal;
    featureIcon.paint(&painter, iconRect, Qt::AlignCenter, mode, QIcon::Off);

    if (error) {
        const QPixmap& badge = errorBadge();
        if (!badge.isNull()) {
            painter.drawPixmap(iconRect.right() - badgeExtent / 2, iconRect.top(), badge);
        }
    }
}

void TimelineMarker::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        Q_EMIT selectRequested(QString::fromUtf8(internalName.c_str()));
    }

    QWidget::mousePressEvent(event);
}

void TimelineMarker::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        Q_EMIT editRequested(QString::fromUtf8(internalName.c_str()));
    }

    QWidget::mouseDoubleClickEvent(event);
}

void TimelineMarker::contextMenuEvent(QContextMenuEvent* event)
{
    event->accept();

    const QString feature = QString::fromUtf8(internalName.c_str());
    Q_EMIT selectRequested(feature);
    Q_EMIT menuRequested(feature, event->globalPos());
}

#include "moc_TimelineMarker.cpp"
