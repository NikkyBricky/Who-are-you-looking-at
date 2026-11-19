#pragma once

#include <raylib.h>

#include <cstddef>
#include <string>
#include <vector>

namespace wayla {

struct RenderItem {
    int col;
    int row;
    float y;
};

enum class Tile {
    Floor,
    Wall,
    Table,
    Door,
};

class Room {
public:
    static constexpr int TileSize = 32;

    void Load(const std::vector<std::string>& rows);
    bool LoadFromFile(const std::string& path);

    void DrawFloor() const;
    void DrawObject(int col, int row) const;
    void CollectRenderItems(std::vector<RenderItem>& out) const;

    bool IsSolid(int col, int row) const;
    bool Collides(Rectangle box) const;

    bool IsDoor(int col, int row) const;
    bool IsDoorOpen(int col, int row) const;
    void ToggleDoor(int col, int row);

    int Cols() const {return cols_;}
    int Rows() const {return static_cast<int>(rows_.size());}

private:
    Tile TileAt(int col, int row) const;
    std::size_t Index(int col, int row) const;

    std::vector<std::string> rows_;
    std::vector<char> open_;
    int cols_ = 0;
};

}
