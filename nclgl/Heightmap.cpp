#include "HeightMap.h"
#include <iostream>
#include <algorithm>

HeightMap::HeightMap(const std::string& name, int numPatches) {
    int iWidth, iHeight, iChans;
    unsigned char* data = SOIL_load_image(name.c_str(), &iWidth, &iHeight, &iChans, 1);

    if (!data) {
        std::cout << "Heightmap can't load file!\n";
        return;
    }

    numVertices = iWidth * iHeight;
    numIndices = (iWidth - 1) * (iHeight - 1) * 6;
    vertices = new Vector3[numVertices];
    colours = new Vector4[numVertices];
    textureCoords = new Vector2[numVertices];
    indices = new GLuint[numIndices];

    Vector3 vertexScale = Vector3(1.0f, 1.0f, 1.0f);
    Vector2 textureScale = Vector2(1.0 / 50.0f, 1.0f / 50.0f);

    for (int j = 0; j < numPatches * numPatches; j++) {
        int patchWidth = j % numPatches;
        int patchHeight = j / numPatches;

        int startX = (iWidth / numPatches) * patchWidth;
        int endX = (patchWidth == numPatches - 1) ? iWidth : (patchWidth + 1) * (iWidth / numPatches);

        int startZ = (iHeight / numPatches) * patchHeight;
        int endZ = (patchHeight == numPatches - 1) ? iHeight : (patchHeight + 1) * (iHeight / numPatches);

		Patch p;
		p.points[0] = Vector3(startX, data[startZ * iWidth + startX], startZ);
        p.points[1] = Vector3(endX - 1, data[startZ * iWidth + (endX - 1)], startZ);
        p.points[2] = Vector3(startX, data[(endZ - 1) * iWidth + startX], endZ - 1);
        p.points[3] = Vector3(endX - 1, data[(endZ - 1) * iWidth + (endX - 1)], endZ - 1);

		patches.push_back(p);

        for (int z = startZ; z < endZ; ++z) {
            for (int x = startX; x < endX; ++x) {
                int offset = (z * iWidth) + x;
                vertices[offset] = Vector3(x, data[offset], z) * vertexScale;
                textureCoords[offset] = Vector2(x, z) * textureScale;
                colours[offset] = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
            }
        }
    }
    SOIL_free_image_data(data);

    int i = 0;


	for (int j = 0; j < numPatches * numPatches; j++) {
        int patchWidth = j % numPatches;
        int patchHeight = j / numPatches;

        int startX = (iWidth / numPatches) * patchWidth;
        int endX = (patchWidth == numPatches - 1) ? iWidth - 1 : (patchWidth + 1) * (iWidth / numPatches);

        int startZ = (iHeight / numPatches) * patchHeight;
        int endZ = (patchHeight == numPatches - 1) ? iHeight - 1 : (patchHeight + 1) * (iHeight / numPatches);

        SubMesh sm;
		sm.start = i;

        for (int z = startZ; z < endZ; ++z) {
            for (int x = startX; x < endX; ++x) {
                int a = (z * iWidth) + x;
                int b = (z * iWidth) + (x + 1);
                int c = ((z + 1) * iWidth) + (x + 1);
                int d = ((z + 1) * iWidth) + x;

                indices[i++] = a;
                indices[i++] = c;
                indices[i++] = b;

                indices[i++] = c;
                indices[i++] = a;
                indices[i++] = d;
            }
        }
        
        sm.count = i - sm.start;
		meshLayers.push_back(sm);
	}

    GenerateNormals();
    GenerateTangents();
    BufferData();

    heightmapSize.x = vertexScale.x * (iWidth - 1);
    heightmapSize.y = vertexScale.y * 255.0f; // each height is a byte!
    heightmapSize.z = vertexScale.z * (iHeight - 1);

    SetPrimitiveType(GL_PATCHES);
}

HeightMap::~HeightMap() {
}

Vector3 HeightMap::GetWorldCoordinatesFromTextureCoords(float u, float v) {

    int iWidth, iHeight, iChans;
    Vector3 vertexScale = Vector3(50.0f, 3.5f, 50.0f);
    Vector2 textureScale = Vector2(1 / 50.0f, 1.0f / 50.0f);

    unsigned char* data = SOIL_load_image(TEXTUREDIR "valleyTex.png", &iWidth, &iHeight, &iChans, 1);

    if (!data) {
        std::cout << "Heightmap can't load file!\n";
        return Vector3(0, 0, 0);
    }

    // First, we need to map (u, v) to the terrain grid
    int x = static_cast<int>(u * (heightmapSize.x / vertexScale.x)); // Get the x grid index
    int z = static_cast<int>(v * (heightmapSize.z / vertexScale.z)); // Get the z grid index

    // Fetch the height value at (x, z)
    int index = (z * iWidth) + x;
    float height = vertices[index].y;  // This stores the height value (y coordinate)

    // The world-space position of the point is calculated using the vertex scale.
    // We apply the scaling that was used when creating the terrain.
    Vector3 worldPosition = Vector3(x, height, z) * vertexScale;

    return worldPosition;
}
