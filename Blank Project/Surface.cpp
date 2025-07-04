#include "Surface.h"

Surface::Surface(const std::string& name, int numPatches) : Mesh() {
    int iWidth, iHeight, iChans;
    unsigned char* data = SOIL_load_image(name.c_str(), &iWidth, &iHeight, &iChans, 1);

    if (!data) {
        std::cout << "Heightmap can't load file!\n";
        return;
    }
    
    float patchSizeX = size.x / numPatches;
    float patchSizeZ = size.z / numPatches;

    int gridWidth = numPatches + 1;
    numVertices = gridWidth * gridWidth;
    numIndices = numPatches * numPatches * 6; // 6 indices per quad

    vertices = new Vector3[numVertices];
    colours = new Vector4[numVertices];
    textureCoords = new Vector2[numVertices];
    indices = new GLuint[numIndices];

    Vector3 vertexScale = Vector3(50.0f, 1.0f, 50.0f);
    Vector2 textureScale = Vector2(1.0 / 50.0f, 1.0f / 50.0f);

    for (int z = 0; z < gridWidth; ++z) {
        for (int x = 0; x < gridWidth; ++x) {
            int offset = (z * gridWidth) + x;

            float xPos = (x * patchSizeX);
            float zPos = (z * patchSizeZ);

            vertices[offset] = Vector3(xPos, data[offset], zPos);
            textureCoords[offset] = Vector2(x, z) * textureScale;
            colours[offset] = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
        }
    }

    patches.resize(numPatches * numPatches);

    for (int j = 0; j < numPatches * numPatches; j++) {
        int patchX = j % numPatches;
        int patchZ = j / numPatches;

        Patch& p = patches[j];

        int startX = patchX;
        int startZ = patchZ;

        p.points[0] = Vector3(startX * patchSizeX, 0, startZ * patchSizeZ);      
        p.points[1] = Vector3((startX + 1) * patchSizeX, 0, startZ * patchSizeZ);     
        p.points[2] = Vector3(startX * patchSizeX, 0, (startZ + 1) * patchSizeZ);    
        p.points[3] = Vector3((startX + 1) * patchSizeX, 0, (startZ + 1) * patchSizeZ);
    }

    
    int i = 0;
    
    for (int z = 0; z < numPatches; ++z) {
        for (int x = 0; x < numPatches; ++x) {
            SubMesh sm;
            sm.start = i;

            int a = (z * gridWidth) + x;           
            int b = (z * gridWidth) + (x + 1);    
            int c = ((z + 1) * gridWidth) + (x + 1);
            int d = ((z + 1) * gridWidth) + x;   

            // Two triangles to make a quad
            indices[i++] = a;
            indices[i++] = c;
            indices[i++] = b;

            indices[i++] = c;
            indices[i++] = a;
            indices[i++] = d;

            sm.count = i - sm.start;
            meshLayers.push_back(sm);
        }
    }

    GenerateNormals();
    GenerateTangents();
    BufferData();

    SetPrimitiveType(GL_PATCHES);
}

Surface::~Surface() {
}
