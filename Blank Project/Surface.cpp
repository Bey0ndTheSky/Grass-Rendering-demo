#include "Surface.h"
#include "../nclgl/Frustum.h"

Surface::Surface(Vector3 Size, float spacing) : Mesh() {
    this->size = Size;
    this->spacing = spacing;

	// Calculate grid dimensions based on vertex density
	int gridWidth = (Size.x / spacing) + 1;
    numVertices = gridWidth * gridWidth;
    numIndices = (gridWidth - 1) * (gridWidth - 1) * 6;
    vertices = new Vector3[numVertices];
    colours = new Vector4[numVertices];
    textureCoords = new Vector2[numVertices];
    indices = new GLuint[numIndices];

    Vector2 textureScale = Vector2(1.0f / gridWidth, 1.0f / gridWidth);

    // Generate vertices in a grid pattern
    for (int z = 0; z < gridWidth; ++z) {
        for (int x = 0; x < gridWidth; ++x) {
            int offset = (z * gridWidth) + x;
            vertices[offset] = Vector3(x * spacing, 0.0f, z * spacing);
            textureCoords[offset] = Vector2(x, z) * textureScale;
            colours[offset] = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
        }
    }

    int i = 0;

    for (int z = 0; z < gridWidth - 1; ++z) {
        for (int x = 0; x < gridWidth - 1; ++x) {
            int a = (z * gridWidth) + x;
            int b = (z * gridWidth) + (x + 1);
            int c = ((z + 1) * gridWidth) + (x + 1);
            int d = ((z + 1) * gridWidth) + x;

            indices[i++] = a;
            indices[i++] = c;
            indices[i++] = b;

            indices[i++] = c;
            indices[i++] = a;
            indices[i++] = d;
        }
    }

    GenerateNormals();
    GenerateTangents();
    BufferData();

    SetPrimitiveType(GL_PATCHES);

	//size.x = gridWidth;
    //size.y = 1.0f * 255.0f;
    //size.z = gridWidth;
}

Surface::~Surface() {
}
