#include "Alien.h"
#include "Sprite.h"
#include "InputManager.h"
#include "Game.h"
#include "Minion.h"
#include "Debug.h"
#include "MainState.h"
#include <iostream>

using namespace std;

// Rotation speed, in radians
const float Alien::rotationSpeed{0.2f};

// Movement acceleration
const float Alien::acceleration{1.0f};
const float Alien::maxSpeed{100.0f};

// Total minion count
const int Alien::totalMinions{1};

// Total health points
const float Alien::healthPoints{500.0f};

// Helper functions
shared_ptr<GameObject> NearestMinion(vector<weak_ptr<GameObject>> &minions, Vector2 position);

void Alien::Start()
{
  gameObject.RequireComponent<Movement>();

  // Explosion on death
  gameObject.RequireComponent<Health>()->OnDeath.AddListener("alienExplosion", [this]()
                                                             {
    auto ExplosionRecipe = MainState::OneShotAnimationRecipe("./assets/image/aliendeath.png", Vector2(127.25f, 133), 0.4f);
    
    gameObject.gameState.CreateObject("Alien Explosion", ExplosionRecipe, gameObject.GetPosition()); });

  // Get game state reference
  auto &gameState = Game::GetInstance().GetState();

  // Add minions
  for (int i = 0; i < minionCount; i++)
  {
    gameState.CreateObject(
        "Minion",
        MainState::MinionRecipe(
            dynamic_pointer_cast<Alien>(GetShared()), 2 * M_PI * i / minionCount));
  }
}

void Alien::Update([[maybe_unused]] float deltaTime)
{
  auto checkButtonLinkedToAction = [this](int button, Action::Type actionType)
  {
    auto inputManager = InputManager::GetInstance();

    if (inputManager.MousePress(button))
      AddAction(actionType);
  };

  // Link mouse buttons to actions
  checkButtonLinkedToAction(LEFT_MOUSE_BUTTON, Action::Type::shoot);
  checkButtonLinkedToAction(RIGHT_MOUSE_BUTTON, Action::Type::move);

  // Rotate slowly
  gameObject.localRotation += rotationSpeed * deltaTime;
}

void Alien::Render()
{
}

void Alien::AddAction(Action::Type actionType)
{
  bool wasEmpty = actionQueue.empty();

  actionQueue.emplace(actionType, inputManager.GetMouseWorldCoordinates());

  // Execute them
  if (wasEmpty)
    ExecuteActions();
}

void Alien::AdvanceActionQueue()
{
  actionQueue.pop();

  ExecuteActions();
}

void Alien::ExecuteActions()
{
  if (actionQueue.empty())
    return;

  // Next action in queue
  auto currentAction = actionQueue.front();

  // If move action
  if (currentAction.type == Action::Type::move)
  {
    gameObject.GetComponent<Movement>()->MoveTo(
        // Upon arrival, pop this action
        currentAction.position, [this]()
        { AdvanceActionQueue(); });
  }

  // If shoot action
  else if (currentAction.type == Action::Type::shoot)
  {
    // Ignore if no minions
    if (!minions.empty())
    {
      // Pick nearest minion
      auto minion = NearestMinion(minions, currentAction.position);

      // Check that it's valid
      if (minion)
      {
        // Tell it to shoot at the target
        minion->GetComponent<Minion>()->Shoot(currentAction.position);
      }
    }

    AdvanceActionQueue();
  }
}

shared_ptr<GameObject> NearestMinion(vector<weak_ptr<GameObject>> &minions, Vector2 position)
{
  // Best distance so far
  float bestDistance = -1;

  shared_ptr<GameObject> bestMinion;

  // For each minion
  auto minionIterator = minions.begin();

  while (minionIterator != minions.end())
  {
    // Try to lock it
    if (auto minion = minionIterator->lock())
    {
      // Get it's distance
      float distance = Vector2::Distance(minion->GetPosition(), position);

      // If it's better
      if (bestDistance == -1 || distance < bestDistance)
      {
        bestDistance = distance;
        bestMinion = minion;
      }

      minionIterator++;
    }
    // If failed, it's gone, so forget it
    else
      minionIterator = minions.erase(minionIterator);
  }

  return bestMinion;
}
