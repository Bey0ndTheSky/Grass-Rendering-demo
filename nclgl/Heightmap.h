#pragma once

#include <string>
#include "mesh.h"
class Frustum;

class HeightMap : public Mesh {
public:
    HeightMap(const std::string& name, int numPatches);
    ~HeightMap();

    Vector3 GetHeightmapSize() const { return heightmapSize; }
    Vector3 GetWorldCoordinatesFromTextureCoords(float u, float v);

protected:
    Vector3 heightmapSize;
};
