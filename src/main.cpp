#include <raylib.h>

#include <iostream>

#include "player.h"
#include "room.h"

int main() {
    wayla::Room room;
    if (!room.LoadFromFile(WAYLA_DATA_DIR "/room.txt")) {
        std::cerr << "Failed to load room map\n";
        return 1;
    }

    const int screenWidth = wayla::Room::TileSize * room.Cols();
    const int screenHeight = wayla::Room::TileSize * room.Rows();

    InitWindow(screenWidth, screenHeight, "Who are you looking at? - stage 1");
    SetTargetFPS(60);

    wayla::Player player;
    player.Init(Vector2{wayla::Room::TileSize * 3.5f, wayla::Room::TileSize * 8.5f}, 160.0f);

    while (!WindowShouldClose()) {
        const float dt = GetFrameTime();

        player.Update(dt, room);

        BeginDrawing();
            ClearBackground(BLACK);
            room.Draw();
            player.Draw();

            DrawText("WASD / arrows - move    ESC - exit", 12, 12, 16,
                     Color{220, 220, 210, 255});
            DrawText("Stage 1: room + player", 12, screenHeight - 26, 16,
                     Color{150, 150, 145, 255});
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
