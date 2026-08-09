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

#include <string>
#include <vector>

#include <QAbstractItemView>
#include <QAction>
#include <QFormLayout>
#include <QListWidget>
#include <QToolButton>
#include <QVBoxLayout>

#include <Base/Console.h>
#include <Gui/Selection/Selection.h>

#include "TaskFaceOperationParameters.h"

using namespace PartDesignGui;

namespace
{
constexpr int referenceListHeight = 96;
}

TaskFaceOperationParameters::TaskFaceOperationParameters(ViewProviderDressUp* view, QWidget* parent)
    : TaskDressUpParameters(view, false, true, parent)
    , selectButton(nullptr)
    , references(nullptr)
{
    build();
}

TaskFaceOperationParameters::~TaskFaceOperationParameters()
{
    try {
        Gui::Selection().clearSelection();
        Gui::Selection().rmvSelectionGate();
    }
    catch (const Py::Exception&) {
        Base::PyException e;
        e.reportException();
    }
}

void TaskFaceOperationParameters::build()
{
    proxy = new QWidget(this);
    auto* layout = new QVBoxLayout(proxy);
    layout->setContentsMargins(0, 0, 0, 0);

    selectButton = new QToolButton(proxy);
    selectButton->setCheckable(true);
    selectButton->setText(startSelectionLabel());
    selectButton->setToolTip(tr("Toggles between selection and preview mode"));
    layout->addWidget(selectButton);

    references = new QListWidget(proxy);
    references->setSelectionMode(QAbstractItemView::MultiSelection);
    references->setToolTip(tr("- select an item to highlight it\n"
                              "- double-click on an item to see the features"));
    references->setMinimumHeight(referenceListHeight);
    layout->addWidget(references);

    auto* parameters = new QFormLayout();
    layout->addLayout(parameters);
    addParameters(parameters);

    this->groupLayout()->addWidget(proxy);

    connect(selectButton, &QToolButton::toggled, this, &TaskFaceOperationParameters::onButtonRefSel);
    connect(references, &QListWidget::currentItemChanged, this, &TaskFaceOperationParameters::setSelection);
    connect(references, &QListWidget::itemClicked, this, &TaskFaceOperationParameters::setSelection);
    connect(references, &QListWidget::itemDoubleClicked, this, &TaskFaceOperationParameters::doubleClicked);

    createDeleteAction(references);
    connect(deleteAction, &QAction::triggered, this, &TaskFaceOperationParameters::onRefDeleted);

    const std::vector<std::string> picked = getDressUpView()->getObject<PartDesign::DressUp>()
                                                ->Base.getSubValues();
    for (const std::string& name : picked) {
        references->addItem(QString::fromStdString(name));
    }

    if (picked.empty()) {
        setSelectionMode(refSel);
    }
    else {
        hideOnError();
    }
}

void TaskFaceOperationParameters::addParameters(QFormLayout*)
{}

PartDesign::DressUp* TaskFaceOperationParameters::beforeChange()
{
    setButtons(none);
    setupTransaction();
    return getObject<PartDesign::DressUp>();
}

void TaskFaceOperationParameters::afterChange(PartDesign::DressUp* feature)
{
    feature->recomputeFeature();
    hideOnError();
}

void TaskFaceOperationParameters::setButtons(selectionModes mode)
{
    selectButton->setChecked(mode == refSel);
    selectButton->setText(mode == refSel ? stopSelectionLabel() : startSelectionLabel());
}

void TaskFaceOperationParameters::onSelectionChanged(const Gui::SelectionChanges& msg)
{
    if (msg.Type == Gui::SelectionChanges::AddSelection && selectionMode == refSel) {
        referenceSelected(msg, references);
    }
}

void TaskFaceOperationParameters::onRefDeleted()
{
    TaskDressUpParameters::deleteRef(references);
}

void TaskFaceOperationParameters::apply()
{
    if (references->count() == 0) {
        Base::Console().warning("No face was chosen, so the feature changes nothing.\n");
    }
}

#include "moc_TaskFaceOperationParameters.cpp"
