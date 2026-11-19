#include <raylib.h>

#include <algorithm>
#include <iostream>

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

    while (!WindowShouldClose()) {
        const float dt = GetFrameTime();

        player.Update(dt, room);

        const Vector2 playerPos = player.Position();
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

        BeginDrawing();
            ClearBackground(BLACK);
            BeginMode2D(camera);
                room.Draw();
                player.Draw();
            EndMode2D();

            DrawText("WASD / arrows - move    ESC - exit", 12, 12, 16,
                     Color{220, 220, 210, 255});
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
