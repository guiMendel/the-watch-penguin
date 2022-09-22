#ifndef __GAME_STATE__
#define __GAME_STATE__

#include <functional>
#include <memory>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <SDL.h>
#include "GameObject.h"
#include "TileMap.h"
#include "Sprite.h"
#include "Music.h"
#include "InputManager.h"
#include "Vector2.h"

class Component;
class Collider;

// Abstract class that defines a state of the game
class GameState
{
  friend Collider;

public:
  GameState();

  // Clear objects on destroy
  ~GameState() { gameObjects.clear(); }

  // Whether the game should exit
  bool QuitRequested() { return quitRequested; }

  void Update(float deltaTime);

  void Render();

  // Removes an object from the object list
  void RemoveObject(int id);

  void RemoveObject(std::shared_ptr<GameObject> gameObject) { RemoveObject(gameObject->id); }

  std::shared_ptr<GameObject> RegisterObject(GameObject *gameObject);

  // Creates a new game object
  template <typename... Args>
  std::shared_ptr<GameObject> CreateObject(
      std::string name, std::function<void(std::shared_ptr<GameObject>)> recipe, Args &&...args)
  {
    // Create the object, which automatically registers it's pointer to the state's list
    int objectId = (new GameObject(name, std::forward<Args>(args)...))->id;
    auto object = gameObjects[objectId];

    // Initialize it
    if (recipe)
      recipe(object);

    // Call it's start method
    if (started)
    {
      object->Start();
    }

    return object;
  }

  std::weak_ptr<GameObject> GetPointer(const GameObject *gameObject);

  void Start();

  // Initializes the state's objects
  virtual void InitializeObjects() = 0;

  // Preloads all the assets so that they are ready when required
  virtual void LoadAssets() {}

  // Supplies a valid unique identifier for a game object
  int SupplyObjectId() { return nextObjectId++; }

  void RegisterLayerRenderer(std::shared_ptr<Component> component);

  std::shared_ptr<GameObject> GetRootObject() { return rootObject; }

protected:
  // Reference to input manager
  InputManager &inputManager;

  Music music;

  // Array with all of the state's objects
  std::unordered_map<int, std::shared_ptr<GameObject>> gameObjects;

  // Root object reference
  std::shared_ptr<GameObject> rootObject;

private:
  void UpdateObject(float deltaTime, std::shared_ptr<GameObject> object);
  void DeleteObjects();
  void DetectCollisions();

  // Adds a new collider to it's corresponding object entry
  void RegisterCollider(std::shared_ptr<Collider> collider);

  // Removes any expired colliders from structure & returns the remaining ones as shared
  std::unordered_map<int, std::vector<std::shared_ptr<Collider>>> ValidateColliders();

  // Indicates that the game must exit
  bool quitRequested{false};

  // Whether the state has executed the start method
  bool started{false};

  // ID counter for game objects
  int nextObjectId{1};

  // Structure that maps each component that renders to it's corresponding render layer
  std::unordered_map<RenderLayer, std::vector<std::weak_ptr<Component>>>
      layerStructure;

  // Structure that maps each object id to the list of it's colliders
  std::unordered_map<int, std::vector<std::weak_ptr<Collider>>> colliderStructure;
};

#include "Component.h"
#include "Collider.h"

#endif