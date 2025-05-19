#pragma once

#include <string>
#include "mesh.h"
class Frustum;

struct Patch {
	Vector3 points[5];
	int index;

    void CalcCentre() {
        points[4] = (points[0] + points[1] + points[2] + points[3]) * 0.25f;
    }
};

class HeightMap : public Mesh {
public:
    HeightMap(const std::string& name, int numPatches);
    ~HeightMap();

    Vector3 GetHeightmapSize() const { return heightmapSize; }
    vector<Patch> GetPatches() const { return patches; }
    Vector3 GetWorldCoordinatesFromTextureCoords(float u, float v);

protected:
    Vector3 heightmapSize;
	vector<Patch> patches;
};
