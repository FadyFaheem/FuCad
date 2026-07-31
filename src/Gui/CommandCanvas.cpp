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
 **************************************************************************/

#include <QFileInfo>
#include <QImageReader>

#include <App/Application.h>
#include <App/Canvas.h>
#include <App/Document.h>

#include "Application.h"
#include "Command.h"
#include "Control.h"
#include "Document.h"
#include "FileDialog.h"
#include "MainWindow.h"
#include "Selection/Selection.h"
#include "TaskView/TaskCanvas.h"
#include "ViewProviderCanvas.h"


using namespace Gui;

namespace
{
Gui::FileDialog::FilterList imageFilters()
{
    QStringList patterns;
    const auto formats = QImageReader::supportedImageFormats();
    patterns.reserve(formats.size());
    for (const QByteArray& format : formats) {
        patterns << QStringLiteral("*.%1").arg(QString::fromLatin1(format));
    }

    return {Gui::FileDialog::Filter(QObject::tr("Images"), patterns),
            Gui::FileDialog::Filter::AllFiles()};
}

/// The single canvas the user has selected, if that is what the selection holds
Image::Canvas* selectedCanvas()
{
    const auto canvases = Gui::Selection().getObjectsOfType<Image::Canvas>();
    return canvases.size() == 1 ? canvases.front() : nullptr;
}
}  // namespace


// ==================================================================
// Std_InsertCanvas
// ==================================================================
DEF_STD_CMD_A(StdCmdInsertCanvas)

StdCmdInsertCanvas::StdCmdInsertCanvas()
    : Command("Std_InsertCanvas")
{
    sGroup = "Insert";
    sMenuText = QT_TR_NOOP("Canvas");
    sToolTipText = QT_TR_NOOP("Lay a reference image on a face and scale it to size");
    sWhatsThis = "Std_InsertCanvas";
    sStatusTip = sToolTipText;
    sPixmap = "image-plane";
    eType = AlterDoc;
}

bool StdCmdInsertCanvas::isActive()
{
    return App::GetApplication().getActiveDocument() != nullptr && !Gui::Control().activeDialog();
}

void StdCmdInsertCanvas::activated(int)
{
    App::Document* doc = App::GetApplication().getActiveDocument();
    if (!doc) {
        return;
    }

    const QString path = FileDialog::getOpenFileName(
        getMainWindow(),
        QObject::tr("Select Image"),
        QString(),
        imageFilters()
    );
    if (path.isEmpty()) {
        return;
    }

    // Kept open until the task panel is accepted or cancelled, so backing out
    // of the dialog also removes the canvas
    openCommand(QT_TRANSLATE_NOOP("Command", "Insert canvas"));

    const std::string name = QFileInfo(path).completeBaseName().toUtf8().constData();
    auto* canvas = freecad_cast<Image::Canvas*>(doc->addObject("Image::Canvas", "Canvas"));
    if (!canvas) {
        abortCommand();
        return;
    }

    canvas->ImageFile.setValue(path.toUtf8().constData());
    if (!name.empty()) {
        canvas->Label.setValue(name);
    }
    doc->recompute();

    Gui::Control().showDialog(new TaskCanvasDialog(canvas, true), doc);
}

// ==================================================================
// Std_CalibrateCanvas
// ==================================================================
DEF_STD_CMD_A(StdCmdCalibrateCanvas)

StdCmdCalibrateCanvas::StdCmdCalibrateCanvas()
    : Command("Std_CalibrateCanvas")
{
    sGroup = "Insert";
    sMenuText = QT_TR_NOOP("Calibrate Canvas");
    sToolTipText = QT_TR_NOOP("Scale a canvas by picking two points on it and giving the real distance");
    sWhatsThis = "Std_CalibrateCanvas";
    sStatusTip = sToolTipText;
    sPixmap = "image-scaling";
    eType = AlterDoc;
}

bool StdCmdCalibrateCanvas::isActive()
{
    return selectedCanvas() != nullptr && !Gui::Control().activeDialog();
}

void StdCmdCalibrateCanvas::activated(int)
{
    Image::Canvas* canvas = selectedCanvas();
    if (!canvas) {
        return;
    }

    auto* vp = freecad_cast<ViewProviderCanvas*>(
        Application::Instance->getViewProvider(canvas)
    );
    if (vp) {
        vp->editCanvas(true);
    }
}

namespace Gui
{

void CreateCanvasCommands()
{
    CommandManager& rcCmdMgr = Application::Instance->commandManager();

    rcCmdMgr.addCommand(new StdCmdInsertCanvas());
    rcCmdMgr.addCommand(new StdCmdCalibrateCanvas());
}

}  // namespace Gui
