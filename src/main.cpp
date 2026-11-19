#include <raylib.h>

#include <vector>

#include "player.h"
#include "room.h"

namespace {

const std::vector<std::string> kRoomRows = {
    "############################",
    "#..........................#",
    "#..........................#",
    "#....TTTT.........TTTT.....#",
    "#....TTTT.........TTTT.....#",
    "#..........................#",
    "#..........................#",
    "#.......TTTTTTTTTT.........#",
    "#.......TTTTTTTTTT.........#",
    "#..........................#",
    "#..........................#",
    "#..TT.................TT...#",
    "#..TT.................TT...#",
    "#..........................#",
    "#..........................#",
    "#..........................#",
    "#..........................#",
    "############################",
};

}

int main() {
    const int screenWidth = wayla::Room::TileSize * 28;
    const int screenHeight = wayla::Room::TileSize * 18;

    InitWindow(screenWidth, screenHeight, "Who are you looking at? - stage 1");
    SetTargetFPS(60);

    wayla::Room room;
    room.Load(kRoomRows);

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
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
