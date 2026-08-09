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

#include "TaskFaceToolParameters.h"
#include "ViewProviderFaceTools.h"

using namespace PartDesignGui;

PROPERTY_SOURCE(PartDesignGui::ViewProviderOffsetFace, PartDesignGui::ViewProviderDressUp)
PROPERTY_SOURCE(PartDesignGui::ViewProviderMoveFace, PartDesignGui::ViewProviderDressUp)
PROPERTY_SOURCE(PartDesignGui::ViewProviderDeleteFace, PartDesignGui::ViewProviderDressUp)

const std::string& ViewProviderOffsetFace::featureName() const
{
    static const std::string name = "OffsetFace";
    return name;
}

void ViewProviderOffsetFace::setupContextMenu(QMenu* menu, QObject* receiver, const char* member)
{
    addDefaultAction(menu, QObject::tr("Edit Offset Face"));
    PartDesignGui::ViewProvider::setupContextMenu(menu, receiver, member);
}

TaskDlgFeatureParameters* ViewProviderOffsetFace::getEditDialog()
{
    return new TaskDlgOffsetFaceParameters(this);
}


const std::string& ViewProviderMoveFace::featureName() const
{
    static const std::string name = "MoveFace";
    return name;
}

void ViewProviderMoveFace::setupContextMenu(QMenu* menu, QObject* receiver, const char* member)
{
    addDefaultAction(menu, QObject::tr("Edit Move Face"));
    PartDesignGui::ViewProvider::setupContextMenu(menu, receiver, member);
}

TaskDlgFeatureParameters* ViewProviderMoveFace::getEditDialog()
{
    return new TaskDlgMoveFaceParameters(this);
}


const std::string& ViewProviderDeleteFace::featureName() const
{
    static const std::string name = "DeleteFace";
    return name;
}

void ViewProviderDeleteFace::setupContextMenu(QMenu* menu, QObject* receiver, const char* member)
{
    addDefaultAction(menu, QObject::tr("Edit Delete Face"));
    PartDesignGui::ViewProvider::setupContextMenu(menu, receiver, member);
}

TaskDlgFeatureParameters* ViewProviderDeleteFace::getEditDialog()
{
    return new TaskDlgDeleteFaceParameters(this);
}
