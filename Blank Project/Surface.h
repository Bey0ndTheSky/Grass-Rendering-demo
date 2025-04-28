#pragma once

#include "../nclgl/mesh.h"
class Frustum;

class Surface : public Mesh {
public:
    Surface(Vector3 Size, float spacing);
    ~Surface();

	Vector3 GetHeightmapSize() const { return size; }

protected:
    Vector3 size;
	float spacing;
};
