#pragma once

#include <raylib.h>

#include "room.h"

namespace wayla {

class Player {
public:
    void Init(Vector2 center, float speed);
    void Update(float dt, const Room& room);
    void Draw() const;

    Rectangle Bounds() const;

private:
    Vector2 position_{0.0f, 0.0f};
    Vector2 facing_{0.0f, 1.0f};
    float speed_ = 160.0f;

    static constexpr float kSize = 24.0f;
};

}
