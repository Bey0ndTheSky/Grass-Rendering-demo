#pragma once
#include <windows.h>
#include "Renderer.h"

class UISystem {

public:
	static UISystem* GetInstance() { return instance; }
	static void Initialize(HWND handle) {
		instance = (instance == nullptr) ? new UISystem(handle) : instance;
	}
	static void Shutdown() { if (instance) { delete instance; } }

	void StartFrame();
	void EndFrame();

	float getVertexScale() { return vertexScale; }
	float getheightScale() { return heightScale; }
	bool getColourMode() { return colourMode; }
	bool getGrassColour() { return grassColourMode; }

	float getGrassHeight() { return grassHeight; }
	float getGrassWidth() { return grassWidth; }
	float getWindStrength() { return windStrength; }
	float getLightPosition() { return lightPosition; }
	float getLightRadius() { return lightRadius; }
	Vector4 getLightColour() { return lightColour; }

protected:
	UISystem(HWND handle);
	virtual ~UISystem();

	void RenderFrame();
	void FramerateUI();
	void DebugUI();

	static UISystem* instance;
	HWND handle;

	float vertexScale = 1.0f;
	float heightScale = 0.0f;
	bool colourMode = false;
	
	bool grassColourMode = true;
	float grassHeight = 25.0f;
	float grassWidth = 5.0f;
	float windStrength = 0.3f;

	float lightPosition = 0.0f;
	float lightRadius = 4.25f;
	Vector4 lightColour = Vector4(1.0f, 1.0f, 0.4f, 1.0f);
};
