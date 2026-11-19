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

void Player::PushOutOf(const Room& room, int col, int row) {
    const Rectangle door{
        static_cast<float>(col * Room::TileSize),
        static_cast<float>(row * Room::TileSize),
        static_cast<float>(Room::TileSize),
        static_cast<float>(Room::TileSize)
    };

    if (!CheckCollisionRecs(Bounds(), door)) {
        return;
    }

    constexpr float kEpsilon = 0.01f;
    const float leftX = door.x - kSize / 2.0f - kEpsilon;
    const float rightX = door.x + door.width + kSize / 2.0f + kEpsilon;
    const float upY = door.y - kSize / 2.0f - kEpsilon;
    const float downY = door.y + door.height + kSize / 2.0f + kEpsilon;

    const Vector2 candidates[] = {
        Vector2{position_.x, upY},
        Vector2{position_.x, downY},
        Vector2{leftX, position_.y},
        Vector2{rightX, position_.y},
    };

    float bestDistance = -1.0f;
    Vector2 best = position_;

    for (const Vector2& candidate : candidates) {
        const Rectangle box{
            candidate.x - kSize / 2.0f,
            candidate.y - kSize / 2.0f,
            kSize,
            kSize
        };
        if (room.Collides(box)) {
            continue;
        }

        const float dx = candidate.x - position_.x;
        const float dy = candidate.y - position_.y;
        const float distance = dx * dx + dy * dy;
        if (bestDistance < 0.0f || distance < bestDistance) {
            bestDistance = distance;
            best = candidate;
        }
    }

    if (bestDistance >= 0.0f) {
        position_ = best;
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
