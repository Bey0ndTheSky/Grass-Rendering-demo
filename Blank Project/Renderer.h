#pragma once
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Frustum.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/light.h"
#include <memory>

class Mesh;
class MeshAnimation;
class MeshMaterial;
class HeightMap;
class Surface;
class Camera;

enum ShaderIndices{
        GROUND_SHADER,
        SKYBOX_SHADER,
        POST_PROCESS_SHADER,
        RENDER_SHADER,
        SHADOW_SHADER
};

class Renderer : public OGLRenderer {
public:
    Renderer(Window& parent);
    ~Renderer(void);

    void DrawScene();
    void RenderScene() override;
    void UpdateScene(float dt) override;
    void PresentScene();
    void DrawPostProcess();
    void DrawGround();
    void DrawSkybox();
    void DrawShadowScene();
    void SetTextures();
    void SetShaders();
    void changeScene();
    void LockCamera();
    void TogglePostProcess() { this->postProcess = !this->postProcess; }

protected:
    Surface* heightMap;
    std::vector<Shader*> shaderVec;
    Shader* shader;
   
    Camera* camera;
    GLuint terrainTex;
    GLuint windTex;
    GLuint dispTex;
    
    GLuint bufferFBO;
    GLuint processFBO;
    GLuint shadowFBO;

    GLuint bufferColourTex[2];
    GLuint bufferDepthTex;
    GLuint shadowTex;

    Frustum frameFrustum;

    Mesh* quad;
    Mesh* mesh;
    Mesh* snow;
    Light* light;

    bool postProcess = false;
    int postTex = 0;
    float lightParam = 0.0f;
    int currentFrame;
    float frameTime;

    float waterRotate;
    float waterCycle;
    float gravity;
    const int PARTICLE_NUM = 2000;

    float windTranslate;
    float windStrength;

    Vector3* particles;
   
    Vector3 flowerPos[100] = {
        Vector3(10.0f, 0.0f, 0.0f),
        Vector3(9.51f, 0.0f, 3.09f),
        Vector3(8.09f, 0.0f, 5.88f),
        Vector3(5.88f, 0.0f, 8.09f),
        Vector3(3.09f, 0.0f, 9.51f),
        Vector3(0.0f, 0.0f, 10.0f),
        Vector3(-3.09f, 0.0f, 9.51f),
        Vector3(-5.88f, 0.0f, 8.09f),
        Vector3(-8.09f, 0.0f, 5.88f),
        Vector3(-9.51f, 0.0f, 3.09f),
        Vector3(-10.0f, 0.0f, 0.0f),
        Vector3(-9.51f, 0.0f, -3.09f),
        Vector3(-8.09f, 0.0f, -5.88f),
        Vector3(-5.88f, 0.0f, -8.09f),
        Vector3(-3.09f, 0.0f, -9.51f),
        Vector3(0.0f, 0.0f, -10.0f),
        Vector3(3.09f, 0.0f, -9.51f),
        Vector3(5.88f, 0.0f, -8.09f),
        Vector3(8.09f, 0.0f, -5.88f),
        Vector3(9.51f, 0.0f, -3.09f),

        Vector3(8.0f, 0.0f, 0.0f),
        Vector3(7.64f, 0.0f, 2.76f),
        Vector3(6.28f, 0.0f, 4.95f),
        Vector3(4.95f, 0.0f, 6.28f),
        Vector3(2.76f, 0.0f, 7.64f),
        Vector3(0.0f, 0.0f, 8.0f),
        Vector3(-2.76f, 0.0f, 7.64f),
        Vector3(-4.95f, 0.0f, 6.28f),
        Vector3(-6.28f, 0.0f, 4.95f),
        Vector3(-7.64f, 0.0f, 2.76f),
        Vector3(-8.0f, 0.0f, 0.0f),
        Vector3(-7.64f, 0.0f, -2.76f),
        Vector3(-6.28f, 0.0f, -4.95f),
        Vector3(-4.95f, 0.0f, -6.28f),
        Vector3(-2.76f, 0.0f, -7.64f),
        Vector3(0.0f, 0.0f, -8.0f),
        Vector3(2.76f, 0.0f, -7.64f),
        Vector3(4.95f, 0.0f, -6.28f),
        Vector3(6.28f, 0.0f, -4.95f),
        Vector3(7.64f, 0.0f, -2.76f),

        Vector3(5.0f, 0.0f, 0.0f),
        Vector3(4.85f, 0.0f, 1.55f),
        Vector3(4.29f, 0.0f, 3.83f),
        Vector3(3.83f, 0.0f, 4.29f),
        Vector3(1.55f, 0.0f, 4.85f),
        Vector3(0.0f, 0.0f, 5.0f),
        Vector3(-1.55f, 0.0f, 4.85f),
        Vector3(-3.83f, 0.0f, 4.29f),
        Vector3(-4.29f, 0.0f, 3.83f),
        Vector3(-4.85f, 0.0f, 1.55f),
        Vector3(-5.0f, 0.0f, 0.0f),
        Vector3(-4.85f, 0.0f, -1.55f),
        Vector3(-4.29f, 0.0f, -3.83f),
        Vector3(-3.83f, 0.0f, -4.29f),
        Vector3(-1.55f, 0.0f, -4.85f),
        Vector3(0.0f, 0.0f, -5.0f),
        Vector3(1.55f, 0.0f, -4.85f),
        Vector3(3.83f, 0.0f, -4.29f),
        Vector3(4.29f, 0.0f, -3.83f),
        Vector3(4.85f, 0.0f, -1.55f),

        Vector3(3.0f, 0.0f, 0.0f),
        Vector3(2.85f, 0.0f, 0.90f),
        Vector3(2.55f, 0.0f, 1.95f),
        Vector3(2.05f, 0.0f, 2.55f),
        Vector3(1.35f, 0.0f, 2.85f),
        Vector3(0.0f, 0.0f, 3.0f),
        Vector3(-1.35f, 0.0f, 2.85f),
        Vector3(-2.05f, 0.0f, 2.55f),
        Vector3(-2.55f, 0.0f, 1.95f),
        Vector3(-2.85f, 0.0f, 0.90f),
        Vector3(-3.0f, 0.0f, 0.0f),
        Vector3(-2.85f, 0.0f, -0.90f),
        Vector3(-2.55f, 0.0f, -1.95f),
        Vector3(-2.05f, 0.0f, -2.55f),
        Vector3(-1.35f, 0.0f, -2.85f),
        Vector3(0.0f, 0.0f, -3.0f),
        Vector3(1.35f, 0.0f, -2.85f),
        Vector3(2.05f, 0.0f, -2.55f),
        Vector3(2.55f, 0.0f, -1.95f),
        Vector3(2.85f, 0.0f, -0.90f),

        Vector3(1.0f, 0.0f, 0.0f),
        Vector3(0.98f, 0.0f, 0.18f),
        Vector3(0.92f, 0.0f, 0.39f),
        Vector3(0.82f, 0.0f, 0.58f),
        Vector3(0.68f, 0.0f, 0.76f),
        Vector3(0.5f, 0.0f, 0.87f),
        Vector3(0.31f, 0.0f, 0.92f),
        Vector3(0.11f, 0.0f, 0.98f),
        Vector3(-0.11f, 0.0f, 0.98f),
        Vector3(-0.31f, 0.0f, 0.92f),
        Vector3(-0.5f, 0.0f, 0.87f),
        Vector3(-0.68f, 0.0f, 0.76f),
        Vector3(-0.82f, 0.0f, 0.58f),
        Vector3(-0.92f, 0.0f, 0.39f),
        Vector3(-0.98f, 0.0f, 0.18f),
        Vector3(-1.0f, 0.0f, 0.0f),
        Vector3(-0.98f, 0.0f, -0.18f),
        Vector3(-0.92f, 0.0f, -0.39f),
        Vector3(-0.82f, 0.0f, -0.58f),
        Vector3(-0.68f, 0.0f, -0.76f)
    };

    Vector3 headstonePos[21] = {
        // Row 1
        Vector3(-1200.0f, 0.0f, 400.0f),
        Vector3(-800.0f, 7.0f, 400.0f),
        Vector3(-400.0f, 24.0f, 400.0f),
        Vector3(0.0f, 41.0f, 400.0f),
        Vector3(400.0f, 47.0f, 400.0f),
        Vector3(800.0f,51.5f, 400.0f),
        Vector3(1200.0f, 37.0f, 400.0f),

        // Row 2
        Vector3(-1200.0f, 0.0f, 0.0f),
        Vector3(-800.0f, 10.5f, 0.0f),
        Vector3(-400.0f, 26.0f, 0.0f),
        Vector3(0.0f, 42.0f, 0.0f),
        Vector3(400.0f, 47.0f, 0.0f),
        Vector3(800.0f, 36.5f, 0.0f),
        Vector3(1200.0f, 30.0f, 0.0f),

        // Row 3
        Vector3(-1200.0f, 0.0f, -400.0f),
        Vector3(-800.0f, 14.0f, -400.0f),
        Vector3(-400.0f, 8.0f, -400.0f),
        Vector3(0.0f, 4.0f, -400.0f),
        Vector3(400.0f, 54.5f, -400.0f),
        Vector3(800.0f, 69.0f, -400.0f),
        Vector3(1200.0f, 75.0f, -400.0f)
    };

    Vector3 camerapos[5] = {
        Vector3(0.15f, 2.0f, 0.2f),
        Vector3(0.15f, 4.0f, 0.8f),
        Vector3(0.3f, 3.0f, 0.3f),
        Vector3(0.8f, 3.0f, 0.6f),
        Vector3(0.15f, 3.0f, 0.8f),
    };

    Vector4 lerp(const Vector4& a, const Vector4& b, float t) {
        return t > 1.0f ? b : a + (b - a) * t;
    }
};
