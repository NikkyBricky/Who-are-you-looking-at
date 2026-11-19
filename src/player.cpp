#include "player.h"

#include <cmath>

namespace wayla {

void Player::Init(Vector2 center, float speed) {
    position_ = center;
    speed_ = speed;
}

Rectangle Player::Bounds() const {
    return Rectangle{position_.x - kSize / 2.0f, position_.y - kSize / 2.0f, kSize, kSize};
}

void Player::Update(float dt, const Room& room) {
    Vector2 direction{0.0f, 0.0f};

    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))    direction.y -= 1.0f;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))  direction.y += 1.0f;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  direction.x -= 1.0f;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) direction.x += 1.0f;

    if (direction.x != 0.0f || direction.y != 0.0f) {
        const float length = std::hypot(direction.x, direction.y);
        direction.x /= length;
        direction.y /= length;
        facing_ = direction;
    }

    const float dx = direction.x * speed_ * dt;
    const float dy = direction.y * speed_ * dt;

    Rectangle box = Bounds();
    box.x += dx;
    if (!room.Collides(box)) {
        position_.x += dx;
    }

    box = Bounds();
    box.y += dy;
    if (!room.Collides(box)) {
        position_.y += dy;
    }
}

void Player::Draw() const {
    const Rectangle body = Bounds();

    DrawEllipse(position_.x, position_.y + kSize * 0.45f, kSize * 0.9f, kSize * 0.3f,
                Color{0, 0, 0, 90});

    DrawRectangleRec(body, Color{86, 150, 190, 255});
    DrawRectangleLinesEx(body, 2.0f, Color{30, 55, 70, 255});

    const float markerX = position_.x + facing_.x * (kSize * 0.3f) - 3.0f;
    const float markerY = position_.y + facing_.y * (kSize * 0.3f) - 3.0f;
    DrawRectangle(markerX, markerY, 6, 6, Color{235, 235, 210, 255});
}

}
