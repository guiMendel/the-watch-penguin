#ifndef __TILEMAP__
#define __TILEMAP__

#include <string>
#include <vector>
#include <memory>
#include "GameObject.h"
#include "Component.h"
#include "TileSet.h"

class TileMap : public Component
{
public:
  // Defines the intensity of the parallax effect. 0 means no effect
  const float parallaxIntensity = 0.75f;

  // Default constructor. onlyLayer allows to only render a single tilemap layer, but if set to -1 render all of them
  TileMap(
      GameObject &associatedObject, std::string filename,
      std::shared_ptr<TileSet> tileSet, int onlyLayer = -1,
      RenderLayer renderLayer = RenderLayer::Tilemap);

  // Loads a map configuration file
  void Load(std::string filename);

  // Sets the associated tileset
  void SetTileSet(TileSet *newTileSet) { tileSet.reset(newTileSet); }

  // Returns a reference to the tile at the given position
  int &At(int x, int y, int z = 0) { return tileMatrix[x + y * mapWidth + z * mapWidth * mapHeight]; }

  int &At(Vector2 position, int layer) { return At((int)position.x, (int)position.y, layer); }

  // Renders a given layer
  void RenderTileLayer(int layer);

  int GetWidth() const { return mapWidth; }
  int GetHeight() const { return mapHeight; }
  int GetDepth() const { return mapDepth; }

  // === OVERRIDES

  void Render() override;

  void Update([[maybe_unused]] float deltaTime) override {}

  RenderLayer GetRenderLayer() override { return renderLayer; }

private:
  std::vector<int> tileMatrix;

  // Reference to the tileset
  std::shared_ptr<TileSet> tileSet;

  int mapWidth;
  int mapHeight;
  int mapDepth;

  // Which render layer to render to
  RenderLayer renderLayer;

  // Which tilemap layer to render (if -1, renders all)
  int targetLayer;
};

#endif