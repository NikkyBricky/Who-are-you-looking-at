#include <raylib.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <vector>

#include "player.h"
#include "room.h"

int main() {
    wayla::Room room;
    if (!room.LoadFromFile(WAYLA_DATA_DIR "/room.txt")) {
        std::cerr << "Failed to load room map\n";
        return 1;
    }

    const int screenWidth = wayla::Room::TileSize * 28;
    const int screenHeight = wayla::Room::TileSize * 18;
    const float worldWidth = static_cast<float>(room.Cols() * wayla::Room::TileSize);
    const float worldHeight = static_cast<float>(room.Rows() * wayla::Room::TileSize);

    InitWindow(screenWidth, screenHeight, "Who are you looking at?");
    SetTargetFPS(60);

    wayla::Player player;
    player.Init(Vector2{wayla::Room::TileSize * 3.5f, wayla::Room::TileSize * 8.5f}, 160.0f);

    Camera2D camera{};
    camera.offset = Vector2{screenWidth / 2.0f, screenHeight / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    camera.target = player.Position();

    struct DrawEntry {
        float y;
        int col;
        int row;
        bool isPlayer;
    };

    std::vector<DrawEntry> drawList;
    std::vector<wayla::RenderItem> roomItems;
    drawList.reserve(static_cast<std::size_t>(room.Cols() * room.Rows() + 1));
    roomItems.reserve(static_cast<std::size_t>(room.Cols() * room.Rows()));

    while (!WindowShouldClose()) {
        const float dt = GetFrameTime();

        player.Update(dt, room);

        Vector2 playerPos = player.Position();
        int nearestDoorCol = -1;
        int nearestDoorRow = -1;
        float nearestDoorDist = 1000000.0f;

        for (int row = 0; row < room.Rows(); ++row) {
            for (int col = 0; col < room.Cols(); ++col) {
                if (!room.IsDoor(col, row)) {
                    continue;
                }
                const float dx = playerPos.x - (col * wayla::Room::TileSize + wayla::Room::TileSize / 2.0f);
                const float dy = playerPos.y - (row * wayla::Room::TileSize + wayla::Room::TileSize / 2.0f);
                const float dist = std::hypot(dx, dy);
                if (dist < nearestDoorDist) {
                    nearestDoorDist = dist;
                    nearestDoorCol = col;
                    nearestDoorRow = row;
                }
            }
        }

        if (nearestDoorDist < wayla::Room::TileSize * 1.5f && IsKeyPressed(KEY_E)) {
            room.ToggleDoor(nearestDoorCol, nearestDoorRow);
            player.PushOutOf(room, nearestDoorCol, nearestDoorRow);
            playerPos = player.Position();
            const float dx = playerPos.x - (nearestDoorCol * wayla::Room::TileSize + wayla::Room::TileSize / 2.0f);
            const float dy = playerPos.y - (nearestDoorRow * wayla::Room::TileSize + wayla::Room::TileSize / 2.0f);
            nearestDoorDist = std::hypot(dx, dy);
        }

        const float halfViewWidth = screenWidth / 2.0f;
        const float halfViewHeight = screenHeight / 2.0f;

        if (worldWidth > screenWidth) {
            camera.target.x = std::clamp(playerPos.x, halfViewWidth, worldWidth - halfViewWidth);
        } else {
            camera.target.x = worldWidth / 2.0f;
        }

        if (worldHeight > screenHeight) {
            camera.target.y = std::clamp(playerPos.y, halfViewHeight, worldHeight - halfViewHeight);
        } else {
            camera.target.y = worldHeight / 2.0f;
        }

        drawList.clear();
        roomItems.clear();
        room.CollectRenderItems(roomItems);
        for (const wayla::RenderItem& item : roomItems) {
            drawList.push_back(DrawEntry{item.y, item.col, item.row, false});
        }
        const Rectangle playerBox = player.Bounds();
        drawList.push_back(DrawEntry{playerBox.y + playerBox.height, -1, -1, true});
        std::sort(drawList.begin(), drawList.end(), [](const DrawEntry& a, const DrawEntry& b) {
            if (a.y != b.y) {
                return a.y < b.y;
            }
            return a.col < b.col;
        });

        BeginDrawing();
            ClearBackground(BLACK);
            BeginMode2D(camera);
                room.DrawFloor();
                for (const DrawEntry& entry : drawList) {
                    if (entry.isPlayer) {
                        player.Draw();
                    } else {
                        room.DrawObject(entry.col, entry.row);
                    }
                }

                if (nearestDoorDist < wayla::Room::TileSize * 1.5f) {
                    const int hintX = nearestDoorCol * wayla::Room::TileSize;
                    const int hintY = nearestDoorRow * wayla::Room::TileSize - 16;
                    DrawText(room.IsDoorOpen(nearestDoorCol, nearestDoorRow) ? "E - close" : "E - open",
                             hintX, hintY, 10, Color{230, 220, 190, 255});
                }
            EndMode2D();

            DrawText("WASD / arrows - move    E - door    ESC - exit", 12, 12, 16,
                     Color{220, 220, 210, 255});
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
