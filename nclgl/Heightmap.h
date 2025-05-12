#pragma once

#include <string>
#include "mesh.h"
class Frustum;

struct Patch {
    Vector3 points[4];
};

class HeightMap : public Mesh {
public:
    HeightMap(const std::string& name, int numPatches);
    ~HeightMap();

    Vector3 GetHeightmapSize() const { return heightmapSize; }
	Patch GetPatch(int i) const { return patches[i]; }
    Vector3 GetWorldCoordinatesFromTextureCoords(float u, float v);

protected:
    Vector3 heightmapSize;
	vector<Patch> patches;
};
