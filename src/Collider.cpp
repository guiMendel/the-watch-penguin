#include "Collider.h"

// Explicitly initialize box
Collider::Collider(GameObject &associatedObject, Rectangle box) : Component(associatedObject)
{
  SetBox(box);
}

// Use sprite's box
Collider::Collider(GameObject &associatedObject, std::shared_ptr<Sprite> sprite, Vector2 scale = Vector2::One())
    : Collider(associatedObject,
               Rectangle(-sprite->GetWidth() * scale.x / 2, -sprite->GetHeight() * scale.y / 2,
                         sprite->GetWidth() * scale.x, sprite->GetHeight() * scale.y)) {}

void Collider::SetBox(const Rectangle &newBox)
{
  box = newBox;

  maxVertexDistanceSquared = (box.width * box.width + box.height * box.height) / 4;
}

Rectangle Collider::GetBox() const { return box; }
