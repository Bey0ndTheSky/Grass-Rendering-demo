#include "Renderer.h"
#include "Surface.h"
#include "UISystem.h"
#include "../nclgl/Camera.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/MeshAnimation.h"
#include "../nclgl/MeshMaterial.h"
#include <algorithm>

# define SHADOWSIZE 2048

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
    quad = Mesh::GenerateQuad();

    heightMap = new HeightMap(TEXTUREDIR "valleyTex.png", 32);
    camera = new Camera(-12, 225, Vector3());

    Vector3 dimensions = heightMap->GetHeightmapSize(); // *Vector3(121.0, 9.0, 121.0);
    camera->SetPosition(dimensions * Vector3(-0.05, 1, -0.05));

    for (int i = 0; i < 5; ++i) {
        camera->cameraPath.emplace_back(dimensions*camerapos[i]);
    }

    SetShaders();
    SetTextures();

    glGenTextures(1, &bufferDepthTex);
    glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height,
        0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

    for (int i = 0; i < 2; ++i) {
        glGenTextures(1, &bufferColourTex[i]);
        glBindTexture(GL_TEXTURE_2D, bufferColourTex[i]);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }

    glGenFramebuffers(1, &bufferFBO);     // We'll render the scene into this
    glGenFramebuffers(1, &processFBO);    // And do post processing in this

    glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[0], 0);

    // We can check FBO attachment success using this command!
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE ||
        !bufferDepthTex || !bufferColourTex[0]) {
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenTextures(1, &shadowTex);
    glBindTexture(GL_TEXTURE_2D, shadowTex);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &shadowFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_2D, shadowTex, 0);
    glDrawBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    projMatrix = Matrix4::Perspective(1.0f, 80000.0f,
        static_cast<float>(width) / static_cast<float>(height),
        45.0f);

    glEnable(GL_DEPTH_TEST);
    glClipControl(GL_LOWER_LEFT,
        GL_ZERO_TO_ONE);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    windTranslate = 0.0f;
    windStrength = 0.5f;

    currentFrame = 0;
    frameTime = 0.0f;

    light = new Light(dimensions * Vector3(0.2f, 20.0f, 0.5f),
        Vector4(1, 1, 1, 1), dimensions.x * 4.25f);

    init = true;
}

Renderer::~Renderer(void) {
    delete heightMap;
    delete camera;
    delete quad;
    delete light;

    glDeleteTextures(2, bufferColourTex);
    glDeleteTextures(1, &bufferDepthTex);
    glDeleteFramebuffers(1, &bufferFBO);
    glDeleteFramebuffers(1, &processFBO);
    glDeleteTextures(1, &shadowTex);
    glDeleteFramebuffers(1, &shadowFBO);

    for (Shader* shader : shaderVec) {
        delete shader;
    }

}

void Renderer::UpdateScene(float dt) {
    camera->UpdateCamera(dt);
    viewMatrix = camera->BuildViewMatrix();
    projMatrix = Matrix4::Perspective(1.0f, 80000.0f,
        static_cast<float>(width) / static_cast<float>(height),
        45.0f);

    frameTime -= dt;
    gravity = gravity > 0.981f ? gravity - 0.981f : gravity;
    gravity += dt;

    windTranslate += dt * (0.015f + cos(dt * 0.01f) * 0.01f);
    windStrength = 0.3f * sin(dt * 0.05f) * 0.29;

    frameFrustum.FromMatrix(projMatrix * viewMatrix);

    lightParam += dt * 0.005f;
    light->SetPosition(light->GetPosition() + Vector3(1, 5, 0) * dt * 0.005f * heightMap->GetHeightmapSize().x);
    light->SetColour(lerp(Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector4(1.0f, 0.5f, 0.0f, 1.0f), lightParam));
   
    postTex = 0;
}

void Renderer::RenderScene() {
    UISystem::GetInstance()->StartFrame();
    DrawScene();
    
    if (postProcess) { DrawPostProcess(); }
    PresentScene();
    UISystem::GetInstance()->EndFrame();
}

void Renderer::DrawScene() {
    DrawShadowScene();

    glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
    glEnable(GL_STENCIL_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    glStencilFunc(GL_ALWAYS, 2, ~0);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    viewMatrix = camera-> BuildViewMatrix();
    projMatrix = Matrix4::Perspective(1.0f, 80000.0f,
        static_cast<float>(width) / static_cast<float>(height),
        45.0f);

    DrawGround();

    //DrawSkybox();
    glDisable(GL_STENCIL_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Renderer::DrawPostProcess() {
   /*
    glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    shader = shaderVec[POST_PROCESS_SHADER];
    BindShader(shader);
    modelMatrix.ToIdentity();
    viewMatrix.ToIdentity();
    projMatrix.ToIdentity();
    textureMatrix.ToIdentity();
    UpdateShaderMatrices();

    glDisable(GL_DEPTH_TEST);

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(shader->GetProgram(), "sceneTex"), 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex[1], 0);
    glBindTexture(GL_TEXTURE_2D, bufferColourTex[0]);
    quad->Draw();

    postTex = !postTex;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glEnable(GL_DEPTH_TEST);*/
}

void Renderer::changeScene() {
    UISystem* ui = UISystem::GetInstance();
	float vertexScale = ui->getVertexScale() + 10;
    vertexScale = vertexScale * vertexScale;
    Vector3 scale = heightMap->GetHeightmapSize() * Vector3(vertexScale, ui->getheightScale(), vertexScale);
    light->SetPosition(Vector3(0.5f, 150.0f, 0.5f) * scale);
    light->SetColour(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    light->SetRadius(scale.x * 4.55f);
    currentFrame = 0;
    frameTime = 0.0f;
	lightParam = 0.0f;
}

void Renderer::DrawGround() {
    modelMatrix.ToIdentity();
    textureMatrix.ToIdentity();

    shader = shaderVec[GROUND_SHADER];
    BindShader(shader);

    UISystem* ui = UISystem::GetInstance();
    Vector3 scale = Vector3(ui->getVertexScale(), ui->getheightScale() + 0.10, ui->getVertexScale());
    modelMatrix = Matrix4::Scale(scale) * modelMatrix;

    glUniform3fv(glGetUniformLocation(shader->GetProgram(), "VertexScale"), 1, (float*)&scale);

    glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, terrainTex);
    
    glUniform1i(glGetUniformLocation(shader->GetProgram(), "shadowTex"), 4);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, shadowTex);

    

    UpdateShaderMatrices();
    SetShaderLight(*light);

	heightMap->SetPrimitiveType(GL_TRIANGLES);
    heightMap->Draw();

    shader = shaderVec[GRASS_SHADER];
    BindShader(shader);
        
    glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, terrainTex);

    glUniform1i(glGetUniformLocation(shader->GetProgram(), "DisplacementMap"), 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, dispTex);

    glUniform1i(glGetUniformLocation(shader->GetProgram(), "windMap"), 2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, windTex);

    glUniform1i(glGetUniformLocation(shader->GetProgram(), "shadowTex"), 4);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, shadowTex);

	
    //textureMatrix = Matrix4::Scale(Vector3(1.0f / scale.x, 1.0f / scale.y, 1.0f / scale.z)) * textureMatrix;


    glUniform3fv(glGetUniformLocation(shader->GetProgram(), "VertexScale"), 1, (float*)&scale);
    glUniform1i(glGetUniformLocation(shader->GetProgram(), "colourMode"), ui->getColourMode());
    glUniform1i(glGetUniformLocation(shader->GetProgram(), "useGrassColour"), ui->getGrassColour());
    glUniform1f(glGetUniformLocation(shader->GetProgram(), "dispFactor"), 2.0f);
    glUniform1f(glGetUniformLocation(shader->GetProgram(), "grassHeight"), 25.0f);
    glUniform1f(glGetUniformLocation(shader->GetProgram(), "bladeWidth"), 5.0f);

    glUniform1f(glGetUniformLocation(shader->GetProgram(), "windTraslate"), windTranslate);
    glUniform1f(glGetUniformLocation(shader->GetProgram(), "windStrength"), windStrength);

    glUniform3fv(glGetUniformLocation(shader->GetProgram(), "cameraPosition"), 1, (float*)&camera->GetPosition());
    glUniform4f(glGetUniformLocation(shader->GetProgram(), "colourBase"), 0.0f, 0.8f, 0.0f, 1.0f);  // Green
    glUniform4f(glGetUniformLocation(shader->GetProgram(), "colourTop"), 1.0f, 1.0f, 0.0f, 1.0f);  // Yellow
    
    UpdateShaderMatrices();
    SetShaderLight(*light);
    //heightMap->Draw(); DRAW THE ENTIRE THING
    heightMap->SetPrimitiveType(GL_PATCHES);

    for (int i = 0; i < heightMap->GetSubMeshCount(); ++i) {
		bool cull = true;
        for (int j = 0; j < 4; ++j) {
			Vector3 scaledPoint = heightMap->GetPatch(i).points[j] * scale;
			cull &= !frameFrustum.InsideFrustum(scaledPoint, scale.x);
		}

		//if (!cull) heightMap->DrawSubMesh(i);
	}
    
    modelMatrix.ToIdentity();
    textureMatrix.ToIdentity();
}

/*void Renderer::DrawSkybox() {
    glDepthMask(GL_FALSE);

    glStencilFunc(GL_EQUAL, 0, ~0);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    shader = shaderVec[SKYBOX_SHADER];
    BindShader(shader);
    glUniform1i(glGetUniformLocation(shader->GetProgram(), "cubeTex"), 2);
    glUniform1i(glGetUniformLocation(shader->GetProgram(), "useColour"), GL_FALSE);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap1 : cubeMap2);
    UpdateShaderMatrices();
    quad->Draw();

    glDepthMask(GL_TRUE);
}*/

void Renderer::DrawShadowScene() {
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);

    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    shader = shaderVec[SHADOW_SHADER];
    BindShader(shader);

    viewMatrix = Matrix4::BuildViewMatrix(light->GetPosition(), Vector3(0, 0, 0));
    projMatrix = Matrix4::Perspective(1, 80000, 1, 45);
    shadowMatrix = projMatrix * viewMatrix;

    //heightMap->Draw();

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glViewport(0, 0, width, height);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::SetShaders() {
    shaderVec = {
    new Shader("HeightmapVertex.glsl", "HeightmapFragment.glsl"),
    new Shader("HeightmapVertex.glsl", "HeightmapFragment.glsl", "heightmapGeometry.glsl" , "groundTCS.glsl", "groundTES.glsl"), // "heightmapGeometry.glsl"
    new Shader("skyboxVertex.glsl", "skyboxFragment.glsl"),
    //new Shader("TexturedVertex.glsl", "fxaa.glsl"),
    new Shader("TexturedVertex.glsl", "TexturedFragment.glsl"),
    new Shader("shadowVert.glsl","shadowFrag.glsl")
    };

    for (Shader* shader : shaderVec) {
        if (!shader->LoadSuccess()) {
            std::cerr << "Shader loading failed!" << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }
}

void Renderer::SetTextures() {

    terrainTex = SOIL_load_OGL_texture(TEXTUREDIR "Grass_lighted_down.PNG",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS);

    windTex = SOIL_load_OGL_texture(
        TEXTUREDIR "wind.png", SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS
    );

    dispTex = SOIL_load_OGL_texture(
        TEXTUREDIR "valleyTex.png", SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS
    );

    SetTextureRepeating(terrainTex, true);
    SetTextureRepeating(windTex, true);

    if (!terrainTex || !windTex || !dispTex) {
        std::cerr << "Texture loading failed!" << std::endl;
        std::exit(EXIT_FAILURE);
    }

}

void Renderer::LockCamera() {
    camera->LockCamera();
}

void Renderer::PresentScene() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    shader = shaderVec[RENDER_SHADER];
    BindShader(shader);
    modelMatrix.ToIdentity();
    viewMatrix.ToIdentity();
    projMatrix.ToIdentity();
    textureMatrix.ToIdentity();
    UpdateShaderMatrices();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bufferColourTex[postTex]);
    glUniform1i(glGetUniformLocation(shader->GetProgram(), "diffuseTex"), 0);

    quad->Draw();
}