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
        GRASS_SHADER,
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
    HeightMap* heightMap;
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

    bool postProcess = true;
    int postTex = 0;
    float lightParam = 0.0f;
    int currentFrame;
    float frameTime;

    float gravity;
    const int PARTICLE_NUM = 2000;

    float windTranslate;
    float windStrength;

    Vector3* particles;

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
