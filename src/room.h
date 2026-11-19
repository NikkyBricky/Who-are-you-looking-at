#pragma once

#include <raylib.h>

#include <string>
#include <vector>

namespace wayla {

enum class Tile {
    Floor,
    Wall,
    Table,
};

class Room {
public:
    static constexpr int TileSize = 32;

    void Load(const std::vector<std::string>& rows);

    void Draw() const;

    bool IsSolid(int col, int row) const;

    bool Collides(Rectangle box) const;

    int Cols() const { return cols_; }
    int Rows() const { return static_cast<int>(rows_.size()); }

private:
    Tile TileAt(int col, int row) const;

    std::vector<std::string> rows_;
    int cols_ = 0;
};

}
