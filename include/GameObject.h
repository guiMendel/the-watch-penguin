#ifndef __GAME_OBJECT__
#define __GAME_OBJECT__

#include <typeinfo>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <utility>
#include "GameObject.h"
#include "Component.h"
#include "Collider.h"
#include "Vector2.h"
#include "Helper.h"

class GameState;

class GameObject
{
  friend GameState;
  friend Collider;

public:
  // With dimensions
  GameObject(Vector2 coordinates = Vector2(0, 0), double rotation = 0.0, std::shared_ptr<GameObject> parent = nullptr);

  // Called once per frame
  void Update(float deltaTime)
  {
    for (const auto &component : components)
    {
      if (component->IsEnabled())
        component->Update(deltaTime);
    }
  }

  // Whether is dead
  bool DestroyRequested() const { return destroyRequested; }

  // Destroys the object
  void RequestDestroy() { destroyRequested = true; }

  // Adds a new component
  template <class T, typename... Args>
  auto AddComponent(Args &&...args) -> std::shared_ptr<T>
  {
    auto component = std::make_shared<T>(*this, std::forward<Args>(args)...);

    components.push_back(component);

    // Start it
    if (started)
      component->StartAndRegisterLayer();

    return component;
  }

  // Removes an existing component
  void RemoveComponent(Component *component);

  // Gets pointer to a component of the given type
  // Needs to be in header file so the compiler knows how to build the necessary methods
  template <class T>
  auto GetComponent() const -> std::shared_ptr<T>
  {
    // Find the position of the component that is of the requested type
    auto componentIterator = std::find_if(
        components.begin(), components.end(), [](std::shared_ptr<Component> component)
        { return dynamic_cast<T *>(component.get()) != nullptr; });

    // Detect if not present
    if (componentIterator == components.end())
      return std::shared_ptr<T>();

    return std::dynamic_pointer_cast<T>(*componentIterator);
  }

  // Like GetComponent, but raises if it's not present
  template <class T>
  auto RequireComponent() const -> std::shared_ptr<T>
  {
    auto component = GetComponent<T>();

    if (!component)
    {
      throw std::runtime_error(std::string("Required component was not found.\nRequired component typeid name: ") + typeid(T).name());
    }

    return component;
  }

  auto GetComponent(const Component *componentPointer) const -> std::shared_ptr<Component>;

  // Temporary method to play sound & destroy after done playing
  void DestroyAfterSoundPlay();

  // Initialize
  void Start();

  // State reference
  GameState &gameState;

  // Returns this object's shared pointer
  std::shared_ptr<GameObject> GetShared() const;

  // Get's pointer to parent, and ensures it's valid, unless this is the root object. If the parent is the root object, returns nullptr
  std::shared_ptr<GameObject> GetParent() const;

  // Set the parent
  void SetParent(std::shared_ptr<GameObject> newParent);
  // === ABSOLUTE VALUES

  // Where this object exists in game space, in absolute coordinates
  Vector2 GetPosition() const;
  void SetPosition(const Vector2 newPosition);

  // Absolute scale of the object
  Vector2 GetScale() const;
  void SetScale(const Vector2 newScale);

  // Absolute rotation in radians
  double GetRotation() const;
  void SetRotation(const double newRotation);

  // Get's a list of this objects colliders
  std::vector<std::shared_ptr<Collider>> GetColliders();

  // Where this object exists in game space, relative to it's parent's position
  Vector2 localPosition;

  // Scale of the object
  Vector2 localScale{1, 1};

  // Object's rotation, in radians
  double localRotation{0};

  // Object's unique identifier
  const int id;

  // Child objects
  std::unordered_map<int, std::weak_ptr<GameObject>> children;

private:
  // Initialize with given state
  GameObject(GameState &gameState);

  // Whether this is the root object
  bool IsRoot() const { return id == 0; }

  // Get's pointer to parent, and ensures it's valid, unless this is the root object
  std::shared_ptr<GameObject> InternalGetParent() const;

  // Unlinks from parent, destroys all children and destroys self
  void InternalDestroy();

  // Deletes reference to parent and paren't reference to self
  void UnlinkParent();

  // Registers new collider
  void RegisterCollider(std::shared_ptr<Collider> collider);

  // Vector with all components of this object
  std::vector<std::shared_ptr<Component>> components;

  // Whether is dead
  bool destroyRequested{false};

  // Whether has already run started
  bool started{false};

  // Parent object
  std::weak_ptr<GameObject> weakParent;

  // References to this object's colliders
  // Is only populated after the collider's Start() method is called
  std::vector<std::weak_ptr<Collider>> colliders;
};

#include "GameState.h"

#endif