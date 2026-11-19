#include "room.h"

#include <cmath>
#include <cstddef>
#include <fstream>

namespace wayla {

namespace {

constexpr Color kFloorA{58, 54, 50, 255};
constexpr Color kFloorB{64, 59, 54, 255};
constexpr Color kWallBase{38, 36, 40, 255};
constexpr Color kWallTop{74, 69, 66, 255};
constexpr Color kTableBase{82, 58, 38, 255};
constexpr Color kTableTop{126, 92, 56, 255};
constexpr Color kDoorBase{110, 70, 40, 255};
constexpr Color kDoorTop{150, 105, 60, 255};
constexpr Color kDoorLine{60, 40, 25, 255};
constexpr Color kDoorFrame{70, 50, 35, 255};
constexpr Color kTableGrain{150, 110, 70, 255};
constexpr Color kTableLeg{60, 42, 28, 255};
constexpr Color kWallShade{28, 27, 30, 255};
constexpr int kLift = 14;

Tile TileFromChar(char c) {
    switch (c) {
        case '#': return Tile::Wall;
        case 'T': return Tile::Table;
        case 'D': return Tile::Door;
        default:  return Tile::Floor;
    }
}

void DrawWallTile(int x, int y) {
    const int top = y - kLift;
    DrawRectangle(x, top, Room::TileSize, Room::TileSize + kLift, kWallBase);
    DrawRectangle(x, top, Room::TileSize, 8, kWallTop);
    DrawRectangle(x, top + 8, Room::TileSize, 2, kWallShade);
}

void DrawTableTile(int x, int y) {
    const int top = y - kLift;
    DrawRectangle(x, top, Room::TileSize, Room::TileSize - 6, kTableTop);
    DrawRectangle(x, y + 12, Room::TileSize, kLift + 6, kTableBase);
    DrawRectangle(x + 3, top + 4, Room::TileSize - 6, 2, kTableGrain);
    DrawRectangle(x + 3, top + 11, Room::TileSize - 6, 1, kTableGrain);
    DrawRectangle(x + 3, y + 24, 4, 8, kTableLeg);
    DrawRectangle(x + Room::TileSize - 7, y + 24, 4, 8, kTableLeg);
}

void DrawDoorTile(int x, int y, bool open) {
    const int top = y - kLift;
    if (open) {
        DrawRectangle(x, top, 4, Room::TileSize + kLift, kDoorFrame);
        DrawRectangle(x + Room::TileSize - 4, top, 4, Room::TileSize + kLift, kDoorFrame);
        DrawRectangle(x, top, Room::TileSize, 4, kDoorFrame);
    } else {
        DrawRectangle(x, top, Room::TileSize, Room::TileSize + kLift, kDoorBase);
        DrawRectangle(x, top, Room::TileSize, 6, kDoorTop);
        DrawRectangle(x + Room::TileSize / 2 - 1, top, 2, Room::TileSize + kLift, kDoorLine);
        DrawRectangle(x + Room::TileSize - 8, y + 6, 4, 3, kDoorTop);
    }
}

}

void Room::Load(const std::vector<std::string>& rows) {
    rows_ = rows;
    cols_ = rows_.empty() ? 0 : static_cast<int>(rows_.front().size());

    for (const auto& row : rows_) {
        if (static_cast<int>(row.size()) != cols_) {
            cols_ = -1;
            open_.clear();
            return;
        }
    }

    open_.assign(rows_.size() * static_cast<std::size_t>(cols_), 0);
}

bool Room::LoadFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }

    std::vector<std::string> rows;
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (!line.empty()) {
            rows.push_back(line);
        }
    }

    Load(rows);
    return cols_ > 0;
}

Tile Room::TileAt(int col, int row) const {
    if (col < 0 || row < 0 || col >= cols_ || row >= static_cast<int>(rows_.size())) {
        return Tile::Wall;
    }
    return TileFromChar(rows_[static_cast<std::size_t>(row)][static_cast<std::size_t>(col)]);
}

std::size_t Room::Index(int col, int row) const {
    return static_cast<std::size_t>(row) * static_cast<std::size_t>(cols_) + static_cast<std::size_t>(col);
}

bool Room::IsDoor(int col, int row) const {
    return TileAt(col, row) == Tile::Door;
}

bool Room::IsDoorOpen(int col, int row) const {
    return IsDoor(col, row) && open_[Index(col, row)] != 0;
}

void Room::ToggleDoor(int col, int row) {
    if (!IsDoor(col, row)) {
        return;
    }
    char& state = open_[Index(col, row)];
    state = state == 0 ? 1 : 0;
}

bool Room::IsSolid(int col, int row) const {
    const Tile tile = TileAt(col, row);
    if (tile == Tile::Door) {
        return !IsDoorOpen(col, row);
    }
    return tile != Tile::Floor;
}

bool Room::Collides(Rectangle box) const {
    const int left = static_cast<int>(std::floor(box.x / Room::TileSize));
    const int top = static_cast<int>(std::floor(box.y / Room::TileSize));
    const int right = static_cast<int>(std::floor((box.x + box.width - 0.001f) / Room::TileSize));
    const int bottom = static_cast<int>(std::floor((box.y + box.height - 0.001f) / Room::TileSize));

    for (int row = top; row <= bottom; ++row) {
        for (int col = left; col <= right; ++col) {
            if (IsSolid(col, row)) {
                return true;
            }
        }
    }
    return false;
}

void Room::DrawFloor() const {
    for (int row = 0; row < static_cast<int>(rows_.size()); ++row) {
        for (int col = 0; col < cols_; ++col) {
            const Color floorColor = (col + row) % 2 == 0 ? kFloorA : kFloorB;
            DrawRectangle(col * Room::TileSize, row * Room::TileSize, Room::TileSize, Room::TileSize, floorColor);
        }
    }
}

void Room::DrawObject(int col, int row) const {
    const int x = col * Room::TileSize;
    const int y = row * Room::TileSize;

    switch (TileAt(col, row)) {
        case Tile::Wall:
            DrawWallTile(x, y);
            break;
        case Tile::Table:
            DrawTableTile(x, y);
            break;
        case Tile::Door:
            DrawDoorTile(x, y, IsDoorOpen(col, row));
            break;
        case Tile::Floor:
            break;
    }
}

void Room::CollectRenderItems(std::vector<RenderItem>& out) const {
    for (int row = 0; row < static_cast<int>(rows_.size()); ++row) {
        for (int col = 0; col < cols_; ++col) {
            if (TileAt(col, row) != Tile::Floor) {
                out.push_back(RenderItem{col, row, static_cast<float>((row + 1) * Room::TileSize)});
            }
        }
    }
}

}
