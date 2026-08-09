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

#include "ViewProviderDressUp.h"

namespace PartDesignGui
{

/// View provider of PartDesign::OffsetFace.
class PartDesignGuiExport ViewProviderOffsetFace: public ViewProviderDressUp
{
    Q_DECLARE_TR_FUNCTIONS(PartDesignGui::ViewProviderOffsetFace)
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesignGui::ViewProviderOffsetFace);

public:
    ViewProviderOffsetFace()
    {
        sPixmap = "PartDesign_OffsetFace";
        menuName = tr("Offset Face Parameters");
    }

    const std::string& featureName() const override;
    void setupContextMenu(QMenu* menu, QObject* receiver, const char* member) override;

protected:
    TaskDlgFeatureParameters* getEditDialog() override;
};

/// View provider of PartDesign::MoveFace.
class PartDesignGuiExport ViewProviderMoveFace: public ViewProviderDressUp
{
    Q_DECLARE_TR_FUNCTIONS(PartDesignGui::ViewProviderMoveFace)
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesignGui::ViewProviderMoveFace);

public:
    ViewProviderMoveFace()
    {
        sPixmap = "PartDesign_MoveFace";
        menuName = tr("Move Face Parameters");
    }

    const std::string& featureName() const override;
    void setupContextMenu(QMenu* menu, QObject* receiver, const char* member) override;

protected:
    TaskDlgFeatureParameters* getEditDialog() override;
};

/// View provider of PartDesign::DeleteFace.
class PartDesignGuiExport ViewProviderDeleteFace: public ViewProviderDressUp
{
    Q_DECLARE_TR_FUNCTIONS(PartDesignGui::ViewProviderDeleteFace)
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesignGui::ViewProviderDeleteFace);

public:
    ViewProviderDeleteFace()
    {
        sPixmap = "PartDesign_DeleteFace";
        menuName = tr("Delete Face Parameters");
    }

    const std::string& featureName() const override;
    void setupContextMenu(QMenu* menu, QObject* receiver, const char* member) override;

protected:
    TaskDlgFeatureParameters* getEditDialog() override;
};

}  // namespace PartDesignGui
