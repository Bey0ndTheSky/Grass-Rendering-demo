#pragma once

#include "../nclgl/mesh.h"
class Frustum;

class Surface : public Mesh {
public:
    Surface(float width, Vector3 vertexCount);
    ~Surface();

	Vector3 GetHeightmapSize() const { return size; }

protected:
    Vector3 size;
	float vertexDensity;
};
