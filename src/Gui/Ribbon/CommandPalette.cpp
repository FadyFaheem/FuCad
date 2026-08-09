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


#include <algorithm>
#include <vector>

#include <QAction>
#include <QCoreApplication>
#include <QCursor>
#include <QFontMetrics>
#include <QGridLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLayoutItem>
#include <QLineEdit>
#include <QMenu>
#include <QPoint>
#include <QRect>
#include <QScreen>
#include <QSize>
#include <QString>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>

#include <App/Application.h>
#include <Base/Parameter.h>
#include <Gui/Action.h>
#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Gui/MainWindow.h>

#include "CommandPalette.h"
#include "RibbonButton.h"


using namespace Gui;
using namespace Gui::Ribbon;

namespace
{
constexpr int paletteWidth = 640;
constexpr int gridColumns = 6;
constexpr int maxResults = 24;
constexpr int maxRecent = 12;
constexpr int tileWidth = 98;
constexpr int tileHeight = 74;
constexpr int tileIconExtent = 32;
constexpr int cursorGap = 12;

const char* const paletteParameters = "User parameter:BaseApp/Preferences/CommandPalette";
const char* const pinnedKey = "Pinned";
const char* const recentKey = "Recent";
const char* const paletteCommand = "Std_CommandPalette";

/// What Fusion offers before the user has pinned anything of their own.
const char* const defaultPinned = "PartDesign_Extrude,PartDesign_Revolve,PartDesign_Hole,"
                                  "PartDesign_Fillet,PartDesign_Chamfer,Std_Measure";

ParameterGrp::handle parameters()
{
    return App::GetApplication().GetParameterGroupByPath(paletteParameters);
}

QStringList readList(const char* key, const char* fallback)
{
    const std::string stored = parameters()->GetASCII(key, fallback);
    if (stored.empty()) {
        return {};
    }

    return QString::fromLatin1(stored.c_str()).split(QLatin1Char(','), Qt::SkipEmptyParts);
}

void writeList(const char* key, const QStringList& value)
{
    parameters()->SetASCII(key, value.join(QLatin1Char(',')).toLatin1().constData());
}

QStringList& pinnedCommands()
{
    static QStringList pinned = readList(pinnedKey, defaultPinned);
    return pinned;
}

QStringList& recentCommands()
{
    static QStringList recent = readList(recentKey, "");
    return recent;
}

/// The text a command is listed under, without the accelerator markers of a menu.
QString commandTitle(const Command* command)
{
    const char* text = command->getMenuText();
    if (!text || !*text) {
        return {};
    }

    return Action::cleanTitle(QCoreApplication::translate(command->className(), text));
}

QString commandTooltip(const Command* command)
{
    const char* text = command->getToolTipText();
    if (!text || !*text) {
        return {};
    }

    return QCoreApplication::translate(command->className(), text);
}
}  // namespace


CommandPalette* CommandPalette::_instance = nullptr;

CommandPalette::CommandPalette()
    : QWidget(getMainWindow(), Qt::Popup)
    , searchField(nullptr)
    , body(nullptr)
    , bodyLayout(nullptr)
    , firstTile(nullptr)
{
    setObjectName(QStringLiteral("CommandPalette"));
    setAttribute(Qt::WA_StyledBackground, true);
    setFixedWidth(paletteWidth);

    buildLayout();
}

CommandPalette* CommandPalette::instance()
{
    if (!_instance) {
        _instance = new CommandPalette();
    }

    return _instance;
}

void CommandPalette::destruct()
{
    delete _instance;
    _instance = nullptr;
}

void CommandPalette::noteCommandUsed(const char* command)
{
    // Opening the palette is how the user got to the list, not something the
    // list should then offer them.
    if (!command || !*command || paletteCommand == QLatin1String(command)) {
        return;
    }

    const QString name = QString::fromLatin1(command);
    QStringList& recent = recentCommands();
    if (!recent.isEmpty() && recent.front() == name) {
        return;
    }

    recent.removeAll(name);
    recent.prepend(name);
    while (recent.size() > maxRecent) {
        recent.removeLast();
    }

    writeList(recentKey, recent);
}

void CommandPalette::buildLayout()
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(8);

    searchField = new QLineEdit(this);
    searchField->setObjectName(QStringLiteral("CommandPaletteSearch"));
    searchField->setPlaceholderText(tr("Search commands"));
    searchField->setClearButtonEnabled(true);
    layout->addWidget(searchField);

    body = new QWidget(this);
    body->setObjectName(QStringLiteral("CommandPaletteBody"));
    bodyLayout = new QVBoxLayout(body);
    bodyLayout->setContentsMargins(0, 0, 0, 0);
    bodyLayout->setSpacing(6);
    layout->addWidget(body);

    connect(searchField, &QLineEdit::textChanged, this, [this]() {
        rebuild();
    });
    connect(searchField, &QLineEdit::returnPressed, this, [this]() {
        if (firstTile) {
            firstTile->click();
        }
    });
}

void CommandPalette::popUp()
{
    MainWindow* window = getMainWindow();
    if (!window) {
        return;
    }

    searchField->clear();
    rebuild();
    adjustSize();

    const QPoint cursor = QCursor::pos();
    QPoint origin(cursor.x() - width() / 2, cursor.y() + cursorGap);

    // Fusion opens the palette under the cursor wherever it is, so near an edge
    // the palette has to slide back into the screen to stay usable.
    const QScreen* screen = window->screen();
    if (screen) {
        const QRect available = screen->availableGeometry();
        origin.setX(std::clamp(origin.x(), available.left(), available.right() - width()));
        origin.setY(std::clamp(origin.y(), available.top(), available.bottom() - height()));
    }

    move(origin);
    show();
    searchField->setFocus(Qt::PopupFocusReason);
}

void CommandPalette::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        hide();
        event->accept();
        return;
    }

    QWidget::keyPressEvent(event);
}

void CommandPalette::clearBody()
{
    firstTile = nullptr;

    // Reparenting to nothing and deleting later keeps a tile that is still
    // inside its own clicked() handler alive until that handler has returned.
    while (QLayoutItem* item = bodyLayout->takeAt(0)) {
        if (QWidget* widget = item->widget()) {
            widget->setParent(nullptr);
            widget->deleteLater();
        }
        delete item;
    }
}

void CommandPalette::rebuild()
{
    clearBody();

    const QString query = searchField->text().trimmed();
    if (query.isEmpty()) {
        addSection(tr("Pinned"), pinnedCommands());
        addSection(tr("Recent"), recentCommands());
    }
    else {
        addSection(tr("Results"), search(query));
    }

    if (bodyLayout->isEmpty()) {
        auto* empty = new QLabel(tr("No matching command"), body);
        empty->setObjectName(QStringLiteral("CommandPaletteEmptyLabel"));
        bodyLayout->addWidget(empty);
    }

    body->adjustSize();
    adjustSize();
}

void CommandPalette::addSection(const QString& caption, const QStringList& commands)
{
    if (commands.isEmpty()) {
        return;
    }

    auto* grid = new QWidget(body);
    grid->setObjectName(QStringLiteral("CommandPaletteGrid"));

    auto* layout = new QGridLayout(grid);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(2);

    int placed = 0;
    for (const QString& command : commands) {
        QToolButton* tile = createTile(command, grid);
        if (!tile) {
            continue;
        }

        layout->addWidget(tile, placed / gridColumns, placed % gridColumns);
        if (!firstTile) {
            firstTile = tile;
        }
        ++placed;
    }

    if (placed == 0) {
        delete grid;
        return;
    }

    // Keep a short row left aligned instead of letting its tiles stretch apart.
    if (placed < gridColumns) {
        layout->setColumnStretch(gridColumns - 1, 1);
    }

    auto* label = new QLabel(caption, body);
    label->setObjectName(QStringLiteral("CommandPaletteSectionCaption"));

    bodyLayout->addWidget(label);
    bodyLayout->addWidget(grid);
}

QToolButton* CommandPalette::createTile(const QString& command, QWidget* parent)
{
    QAction* action = RibbonButton::resolveAction(command);
    if (!action) {
        return nullptr;
    }

    auto* tile = new QToolButton(parent);
    tile->setObjectName(QStringLiteral("CommandPaletteTile"));
    tile->setAutoRaise(true);
    tile->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    tile->setIcon(action->icon());
    tile->setIconSize(QSize(tileIconExtent, tileIconExtent));
    tile->setFixedSize(tileWidth, tileHeight);
    tile->setEnabled(action->isEnabled());

    const QString title = Action::cleanTitle(action->text());
    tile->setText(tile->fontMetrics().elidedText(title, Qt::ElideRight, tileWidth - 8));
    tile->setToolTip(action->toolTip().isEmpty() ? title : action->toolTip());

    tile->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tile, &QWidget::customContextMenuRequested, this, [this, tile, command](const QPoint& at) {
        showTileMenu(command, tile->mapToGlobal(at));
    });
    connect(tile, &QToolButton::clicked, this, [this, command]() {
        launchCommand(command);
    });

    return tile;
}

QStringList CommandPalette::search(const QString& query)
{
    if (!Application::Instance) {
        return {};
    }

    const QStringList words = query.split(QLatin1Char(' '), Qt::SkipEmptyParts);

    // Ranked so that what the user is most likely aiming at leads the grid: a
    // title that starts with the query first, then one that merely contains it,
    // then the matches that only the internal name or the tooltip explain.
    struct Match
    {
        int rank;
        QString title;
        QString name;
    };
    std::vector<Match> matches;

    for (const Command* command : Application::Instance->commandManager().getAllCommands()) {
        const QString title = commandTitle(command);
        if (title.isEmpty()) {
            continue;
        }

        const QString name = QString::fromLatin1(command->getName());
        const QString haystack = title + QLatin1Char(' ') + name + QLatin1Char(' ')
            + commandTooltip(command);

        const bool matched = std::all_of(words.begin(), words.end(), [&haystack](const QString& word) {
            return haystack.contains(word, Qt::CaseInsensitive);
        });
        if (!matched) {
            continue;
        }

        int rank = 2;
        if (title.startsWith(query, Qt::CaseInsensitive)) {
            rank = 0;
        }
        else if (title.contains(query, Qt::CaseInsensitive)) {
            rank = 1;
        }

        matches.push_back({rank, title, name});
    }

    std::sort(matches.begin(), matches.end(), [](const Match& left, const Match& right) {
        if (left.rank != right.rank) {
            return left.rank < right.rank;
        }

        return left.title.compare(right.title, Qt::CaseInsensitive) < 0;
    });

    QStringList names;
    for (const Match& match : matches) {
        if (names.size() >= maxResults) {
            break;
        }

        names.append(match.name);
    }

    return names;
}

void CommandPalette::launchCommand(const QString& command)
{
    hide();

    // The popup owns the mouse and keyboard grab until the event loop returns
    // to it, and a command that opens a dialog under that grab would leave the
    // dialog unable to take input.
    QTimer::singleShot(0, this, [command]() {
        if (!Application::Instance) {
            return;
        }

        CommandManager& manager = Application::Instance->commandManager();
        if (Command* cmd = manager.getCommandByName(command.toLatin1().constData())) {
            // Triggered rather than merely run: picking a tile is the user
            // reaching for the command, so it belongs in the recent list.
            cmd->invoke(0, Command::TriggerAction);
        }
    });
}

void CommandPalette::showTileMenu(const QString& command, const QPoint& where)
{
    const bool pinned = pinnedCommands().contains(command);

    QMenu menu(this);
    QAction* toggle = menu.addAction(pinned ? tr("Unpin from Palette") : tr("Pin to Palette"));
    if (menu.exec(where) == toggle) {
        togglePinned(command);
    }
}

void CommandPalette::togglePinned(const QString& command)
{
    QStringList& pinned = pinnedCommands();
    if (!pinned.removeAll(command)) {
        pinned.append(command);
    }

    writeList(pinnedKey, pinned);
    rebuild();
}

#include "moc_CommandPalette.cpp"
