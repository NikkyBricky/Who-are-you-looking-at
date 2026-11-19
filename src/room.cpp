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

Tile TileFromChar(char c) {
    switch (c) {
        case '#': return Tile::Wall;
        case 'T': return Tile::Table;
        case 'D': return Tile::Door;
        default:  return Tile::Floor;
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
    const int left = static_cast<int>(std::floor(box.x / TileSize));
    const int top = static_cast<int>(std::floor(box.y / TileSize));
    const int right = static_cast<int>(std::floor((box.x + box.width - 0.001f) / TileSize));
    const int bottom = static_cast<int>(std::floor((box.y + box.height - 0.001f) / TileSize));

    for (int row = top; row <= bottom; ++row) {
        for (int col = left; col <= right; ++col) {
            if (IsSolid(col, row)) {
                return true;
            }
        }
    }
    return false;
}

void Room::Draw() const {
    for (int row = 0; row < static_cast<int>(rows_.size()); ++row) {
        for (int col = 0; col < cols_; ++col) {
            const int x = col * TileSize;
            const int y = row * TileSize;

            const Color floorColor = (col + row) % 2 == 0 ? kFloorA : kFloorB;
            DrawRectangle(x, y, TileSize, TileSize, floorColor);

            switch (TileAt(col, row)) {
                case Tile::Wall:
                    DrawRectangle(x, y, TileSize, TileSize, kWallBase);
                    DrawRectangle(x, y, TileSize, 6, kWallTop);
                    break;
                case Tile::Table:
                    DrawRectangle(x, y, TileSize, TileSize, kTableBase);
                    DrawRectangle(x, y, TileSize, 8, kTableTop);
                    break;
                case Tile::Door:
                    if (IsDoorOpen(col, row)) {
                        DrawRectangle(x, y, 4, TileSize, kDoorFrame);
                        DrawRectangle(x + TileSize - 4, y, 4, TileSize, kDoorFrame);
                    } else {
                        DrawRectangle(x, y, TileSize, TileSize, kDoorBase);
                        DrawRectangle(x, y, TileSize, 6, kDoorTop);
                        DrawRectangle(x + TileSize / 2 - 1, y, 2, TileSize, kDoorLine);
                    }
                    break;
                case Tile::Floor:
                    break;
            }
        }
    }
}

}
