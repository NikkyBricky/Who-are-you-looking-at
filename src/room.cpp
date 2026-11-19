#include "room.h"

#include <cmath>
#include <cstddef>

namespace wayla {

namespace {

constexpr Color kFloorA{58, 54, 50, 255};
constexpr Color kFloorB{64, 59, 54, 255};
constexpr Color kWallBase{38, 36, 40, 255};
constexpr Color kWallTop{74, 69, 66, 255};
constexpr Color kTableBase{82, 58, 38, 255};
constexpr Color kTableTop{126, 92, 56, 255};

Tile TileFromChar(char c) {
    switch (c) {
        case '#': return Tile::Wall;
        case 'T': return Tile::Table;
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
            return;
        }
    }
}

Tile Room::TileAt(int col, int row) const {
    if (col < 0 || row < 0 || col >= cols_ || row >= static_cast<int>(rows_.size())) {
        return Tile::Wall;
    }
    return TileFromChar(rows_[static_cast<std::size_t>(row)][static_cast<std::size_t>(col)]);
}

bool Room::IsSolid(int col, int row) const {
    return TileAt(col, row) != Tile::Floor;
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
                case Tile::Floor:
                    break;
            }
        }
    }
}

}
