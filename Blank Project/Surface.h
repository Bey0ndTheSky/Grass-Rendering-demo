#pragma once

#include "../nclgl/mesh.h"
#include "../nclgl/Heightmap.h"

class Frustum;

class Surface : public Mesh {
public:
    Surface(const std::string& name, int numPatches);
    ~Surface();

	Vector3 GetHeightmapSize() const { return size; }
    vector<Patch> GetPatches() const { return patches; }

protected:
    Vector3 size;
	float spacing;
    vector<Patch> patches;
};
